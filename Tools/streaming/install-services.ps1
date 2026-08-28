[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$NssmPath,
    [string]$CaddyPath,
    [string]$InfrastructureRoot = $env:PIXEL_STREAMING_INFRA_ROOT,
    [string]$RuntimeRoot = "$env:ProgramData\DarkArisen",
    [Parameter(Mandatory = $true)][string]$ShutdownCommand,
    [switch]$PrivateTailnet,
    [string]$TailscalePath = "C:\Program Files\Tailscale\tailscale.exe"
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Require-MachineEnvironmentValue([string]$Name) {
    $Value = [Environment]::GetEnvironmentVariable($Name, "Machine")
    if ([string]::IsNullOrWhiteSpace($Value)) {
        throw "$Name must be configured as a machine-scoped environment value."
    }
    [Environment]::SetEnvironmentVariable($Name, $Value, "Process")
}

foreach ($Path in @($NssmPath, $ShutdownCommand)) {
    if (-not (Test-Path -LiteralPath $Path -PathType Leaf)) { throw "Required executable/script is missing: $Path" }
}
if ($PrivateTailnet) {
    if (-not (Test-Path -LiteralPath $TailscalePath -PathType Leaf)) {
        throw "Tailscale CLI is required in private mode: $TailscalePath"
    }
} elseif ([string]::IsNullOrWhiteSpace($CaddyPath) -or
    -not (Test-Path -LiteralPath $CaddyPath -PathType Leaf)) {
    throw "Caddy is required when -PrivateTailnet is not selected."
}
if ([string]::IsNullOrWhiteSpace($InfrastructureRoot)) { throw "PIXEL_STREAMING_INFRA_ROOT is required." }

$RequiredEnvironmentNames = @(
    "DARKARISEN_STREAM_HOST",
    "DARKARISEN_TURN_HOST",
    "DARKARISEN_TURN_USERNAME",
    "DARKARISEN_TURN_CREDENTIAL",
    "DARKARISEN_TURN_REALM",
    "DARKARISEN_LOCAL_IP"
)
if ($PrivateTailnet) {
    $RequiredEnvironmentNames += "DARKARISEN_PRIVATE_OVERLAY"
} else {
    $RequiredEnvironmentNames += @(
        "DARKARISEN_STREAM_USER",
        "DARKARISEN_STREAM_PASSWORD_HASH",
        "DARKARISEN_ACME_EMAIL",
        "DARKARISEN_PUBLIC_IP"
    )
}
foreach ($Name in $RequiredEnvironmentNames) {
    Require-MachineEnvironmentValue $Name
}
if ($PrivateTailnet -and $env:DARKARISEN_PRIVATE_OVERLAY -ne "1") {
    throw "DARKARISEN_PRIVATE_OVERLAY must be 1 in private tailnet mode."
}

$PowerShell = (Get-Command powershell.exe).Source
New-Item -ItemType Directory -Path $RuntimeRoot -Force | Out-Null
$LogsRoot = Join-Path $RuntimeRoot "service-logs"
New-Item -ItemType Directory -Path $LogsRoot -Force | Out-Null
$RuntimeCaddyfile = $null
if (-not $PrivateTailnet) {
    $RuntimeCaddyfile = Join-Path $RuntimeRoot "Caddyfile"
    Copy-Item -LiteralPath (Join-Path $PSScriptRoot "Caddyfile.example") -Destination $RuntimeCaddyfile -Force
    & $CaddyPath validate --config $RuntimeCaddyfile --adapter caddyfile
    if ($LASTEXITCODE -ne 0) { throw "Caddy configuration validation failed." }
}

& $ShutdownCommand -ValidateOnly
if ($LASTEXITCODE -ne 0) { throw "Provider shutdown adapter validation failed." }

function Install-NssmService {
    param([string]$Name, [string]$Application, [string]$Arguments)
    & $NssmPath stop $Name confirm 2>$null | Out-Null
    & $NssmPath remove $Name confirm 2>$null | Out-Null
    & $NssmPath install $Name $Application $Arguments | Out-Null
    if ($LASTEXITCODE -ne 0) { throw "Unable to install service $Name" }
    & $NssmPath set $Name AppDirectory $PSScriptRoot | Out-Null
    & $NssmPath set $Name AppExit Default Restart | Out-Null
    & $NssmPath set $Name AppRestartDelay 5000 | Out-Null
    & $NssmPath set $Name AppStdout (Join-Path $LogsRoot "$Name.out.log") | Out-Null
    & $NssmPath set $Name AppStderr (Join-Path $LogsRoot "$Name.err.log") | Out-Null
    & $NssmPath set $Name AppRotateFiles 1 | Out-Null
    & $NssmPath set $Name AppRotateBytes 10485760 | Out-Null
    & $NssmPath set $Name Start SERVICE_AUTO_START | Out-Null
}

Install-NssmService "DarkArisenTurn" $PowerShell `
    "-NoLogo -NoProfile -ExecutionPolicy Bypass -File `"$(Join-Path $PSScriptRoot 'launch-turn.ps1')`" -InfrastructureRoot `"$InfrastructureRoot`" -RuntimeRoot `"$RuntimeRoot`""
Install-NssmService "DarkArisenSignalling" $PowerShell `
    "-NoLogo -NoProfile -ExecutionPolicy Bypass -File `"$(Join-Path $PSScriptRoot 'launch-signalling.ps1')`" -InfrastructureRoot `"$InfrastructureRoot`" -RuntimeRoot `"$RuntimeRoot`""
Install-NssmService "DarkArisenIdleShutdown" $PowerShell `
    "-NoLogo -NoProfile -ExecutionPolicy Bypass -File `"$(Join-Path $PSScriptRoot 'idle-shutdown.ps1')`" -ShutdownCommand `"$ShutdownCommand`""
if ($PrivateTailnet) {
    foreach ($PublicRuleName in @("Dark Arisen TURN UDP", "Dark Arisen TURN TCP", "Dark Arisen HTTPS")) {
        Get-NetFirewallRule -DisplayName $PublicRuleName -ErrorAction SilentlyContinue |
            Remove-NetFirewallRule -ErrorAction SilentlyContinue
    }
    if (Get-Service -Name "DarkArisenGateway" -ErrorAction SilentlyContinue) {
        Stop-Service -Name "DarkArisenGateway" -Force -ErrorAction SilentlyContinue
        & $NssmPath remove "DarkArisenGateway" confirm | Out-Null
    }

    $TailnetRanges = @("100.64.0.0/10", "fd7a:115c:a1e0::/48")
    foreach ($PrivateRuleName in @(
        "Dark Arisen Tailnet HTTPS",
        "Dark Arisen Tailnet TURN TCP",
        "Dark Arisen Tailnet TURN UDP"
    )) {
        Get-NetFirewallRule -DisplayName $PrivateRuleName -ErrorAction SilentlyContinue |
            Remove-NetFirewallRule -ErrorAction SilentlyContinue
    }
    New-NetFirewallRule -DisplayName "Dark Arisen Tailnet HTTPS" -Direction Inbound `
        -Action Allow -Protocol TCP -LocalPort 443 -RemoteAddress $TailnetRanges | Out-Null
    New-NetFirewallRule -DisplayName "Dark Arisen Tailnet TURN TCP" -Direction Inbound `
        -Action Allow -Protocol TCP -LocalPort 3478 -RemoteAddress $TailnetRanges | Out-Null
    New-NetFirewallRule -DisplayName "Dark Arisen Tailnet TURN UDP" -Direction Inbound `
        -Action Allow -Protocol UDP -LocalPort "3478,49160-49200" -RemoteAddress $TailnetRanges | Out-Null

    & $TailscalePath funnel reset | Out-Null
    if ($LASTEXITCODE -ne 0) { throw "Unable to prove that Tailscale Funnel is disabled." }
    & $TailscalePath serve --bg --https=443 http://127.0.0.1:8080 | Out-Null
    if ($LASTEXITCODE -ne 0) { throw "Unable to configure tailnet-only HTTPS." }
} else {
    Install-NssmService "DarkArisenGateway" $CaddyPath `
        "run --config `"$RuntimeCaddyfile`" --adapter caddyfile"
    if (-not (Get-NetFirewallRule -DisplayName "Dark Arisen TURN UDP" -ErrorAction SilentlyContinue)) {
        New-NetFirewallRule -DisplayName "Dark Arisen TURN UDP" -Direction Inbound -Action Allow -Protocol UDP -LocalPort "3478,49160-49200" | Out-Null
    }
    if (-not (Get-NetFirewallRule -DisplayName "Dark Arisen TURN TCP" -ErrorAction SilentlyContinue)) {
        New-NetFirewallRule -DisplayName "Dark Arisen TURN TCP" -Direction Inbound -Action Allow -Protocol TCP -LocalPort 3478 | Out-Null
    }
    if (-not (Get-NetFirewallRule -DisplayName "Dark Arisen HTTPS" -ErrorAction SilentlyContinue)) {
        New-NetFirewallRule -DisplayName "Dark Arisen HTTPS" -Direction Inbound -Action Allow -Protocol TCP -LocalPort "80,443" | Out-Null
    }
}
Get-NetFirewallRule -DisplayName "Dark Arisen Private TCP" -ErrorAction SilentlyContinue |
    Remove-NetFirewallRule -ErrorAction SilentlyContinue
New-NetFirewallRule -DisplayName "Dark Arisen Private TCP" -Direction Inbound `
    -Action Block -Protocol TCP -LocalPort "8080,8888,8889" | Out-Null

$Services = @("DarkArisenTurn", "DarkArisenSignalling", "DarkArisenIdleShutdown")
if (-not $PrivateTailnet) { $Services += "DarkArisenGateway" }
foreach ($Service in $Services) {
    Start-Service -Name $Service
}

Write-Host "Streaming services installed with automatic restart. PrivateTailnet=$PrivateTailnet"
