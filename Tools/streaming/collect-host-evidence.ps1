[CmdletBinding()]
param(
    [string]$RuntimeRoot = "$env:ProgramData\DarkArisen",
    [Alias("PublicHost")][string]$StreamHost = $env:DARKARISEN_STREAM_HOST,
    [string]$OutputPath
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"
$PrivateTailnet = $env:DARKARISEN_PRIVATE_OVERLAY -eq "1"

if ([string]::IsNullOrWhiteSpace($StreamHost)) {
    throw "DARKARISEN_STREAM_HOST or -StreamHost is required."
}
if ([string]::IsNullOrWhiteSpace($OutputPath)) {
    $EvidenceDirectory = Join-Path (Resolve-Path (Join-Path $PSScriptRoot "..\..")) `
        "Saved\StreamingEvidence"
    New-Item -ItemType Directory -Path $EvidenceDirectory -Force | Out-Null
    $OutputPath = Join-Path $EvidenceDirectory "host-preflight.json"
}

$RequiredServices = @(
    "DarkArisenTurn",
    "DarkArisenSignalling",
    "DarkArisenIdleShutdown"
)
if ($PrivateTailnet) {
    $RequiredServices += "Tailscale"
} else {
    $RequiredServices += "DarkArisenGateway"
}
$ServiceEvidence = foreach ($Name in $RequiredServices) {
    $Service = Get-Service -Name $Name -ErrorAction SilentlyContinue
    [ordered]@{
        name = $Name
        status = if ($Service) { [string]$Service.Status } else { "Missing" }
        passed = $Service -and $Service.Status -eq "Running"
    }
}

$StreamerConnections = @(
    Get-NetTCPConnection -LocalPort 8888 -State Established -ErrorAction SilentlyContinue
)
$PlayerConnections = @(
    Get-NetTCPConnection -LocalPort 8080 -State Established -ErrorAction SilentlyContinue |
        Where-Object { $_.RemoteAddress -in @("127.0.0.1", "::1") }
)
$Status = [ordered]@{
    streamer_connections = $StreamerConnections.Count
    player_proxy_connections = $PlayerConnections.Count
}
$StatusPassed = $StreamerConnections.Count -eq 1 -and $PlayerConnections.Count -le 1

$GameLog = Join-Path $RuntimeRoot "DarkArisen-streaming.log"
$NvencPassed = $false
if (Test-Path -LiteralPath $GameLog -PathType Leaf) {
    $GameLogText = Get-Content -LiteralPath $GameLog -Raw
    $NvencPassed = $GameLogText -match "(?i)(NVENC|NVIDIA Video Codec).*(initializ|created|active|selected)" -and
        $GameLogText -notmatch "(?i)(failed|unable).*(NVENC|hardware encoder)|(software encoder)"
}

$PrivateFirewall = Get-NetFirewallRule -DisplayName "Dark Arisen Private TCP" `
    -ErrorAction SilentlyContinue
$FirewallPassed = $PrivateFirewall -and
    $PrivateFirewall.Enabled -eq "True" -and
    $PrivateFirewall.Action -eq "Block"
if ($PrivateTailnet) {
    $TailnetFirewallRules = @(
        "Dark Arisen Tailnet HTTPS",
        "Dark Arisen Tailnet TURN TCP",
        "Dark Arisen Tailnet TURN UDP"
    ) | ForEach-Object {
        Get-NetFirewallRule -DisplayName $_ -ErrorAction SilentlyContinue
    }
    $FirewallPassed = $FirewallPassed -and
        $TailnetFirewallRules.Count -eq 3 -and
        ($TailnetFirewallRules.Enabled -notcontains "False") -and
        ($TailnetFirewallRules.Action -notcontains "Block") -and
        ($TailnetFirewallRules.Direction -notcontains "Outbound")

    $ExpectedTailnetRanges = @("100.64.0.0/10", "fd7a:115c:a1e0::/48")
    foreach ($Rule in $TailnetFirewallRules) {
        $AddressFilter = $Rule | Get-NetFirewallAddressFilter
        $RemoteAddresses = @($AddressFilter.RemoteAddress | ForEach-Object { "$_" })
        if ($RemoteAddresses.Count -ne $ExpectedTailnetRanges.Count) {
            $FirewallPassed = $false
            continue
        }
        $AddressDifference = @(
            Compare-Object -ReferenceObject ($ExpectedTailnetRanges | Sort-Object) `
                -DifferenceObject ($RemoteAddresses | Sort-Object)
        )
        if ($AddressDifference.Count -ne 0) { $FirewallPassed = $false }
    }
}

$AccessEvidence = $null
$AccessPassed = $false
if ($PrivateTailnet) {
    $TailscalePath = "C:\Program Files\Tailscale\tailscale.exe"
    $FunnelStatus = & $TailscalePath funnel status 2>&1 | Out-String
    $FunnelExitCode = $LASTEXITCODE
    $ServeStatus = & $TailscalePath serve status 2>&1 | Out-String
    $ServeExitCode = $LASTEXITCODE
    $FunnelDisabled = $FunnelExitCode -eq 0 -and
        $FunnelStatus -notmatch "(?i)(available on the internet|https?://)"
    $ServeEnabled = $ServeExitCode -eq 0 -and
        $ServeStatus -match "(?i)(available within your tailnet|https://)"
    $AccessPassed = $FunnelDisabled -and $ServeEnabled
    $AccessEvidence = [ordered]@{
        mode = "tailnet-only"
        passed = $AccessPassed
        funnel_disabled = $FunnelDisabled
        serve_enabled = $ServeEnabled
    }
} else {
    $PublicStatus = 0
    try {
        $Request = [System.Net.HttpWebRequest]::Create("https://$StreamHost/")
        $Request.Method = "HEAD"
        $Request.Timeout = 10000
        $Response = $Request.GetResponse()
        $PublicStatus = [int]$Response.StatusCode
        $Response.Dispose()
    } catch [System.Net.WebException] {
        if ($_.Exception.Response) {
            $PublicStatus = [int]$_.Exception.Response.StatusCode
            $_.Exception.Response.Dispose()
        }
    }
    $AccessPassed = $PublicStatus -eq 401
    $AccessEvidence = [ordered]@{
        mode = "public-password"
        passed = $AccessPassed
        status_code = $PublicStatus
    }
}

$Passed = ($ServiceEvidence.passed -notcontains $false) -and
    $StatusPassed -and $NvencPassed -and $FirewallPassed -and $AccessPassed

$Sha256 = [System.Security.Cryptography.SHA256]::Create()
try {
    $HostBytes = [System.Text.Encoding]::UTF8.GetBytes($StreamHost.ToLowerInvariant())
    $StreamHostHashBytes = $Sha256.ComputeHash($HostBytes)
    $StreamHostSha256 = ([System.BitConverter]::ToString($StreamHostHashBytes)).Replace("-", "").ToLowerInvariant()
} finally {
    $Sha256.Dispose()
}

$Evidence = [ordered]@{
    schema = "dark-arisen.streaming-host-evidence.v2"
    collected_at_utc = (Get-Date).ToUniversalTime().ToString("o")
    stream_host_sha256 = $StreamHostSha256
    passed = $Passed
    services = $ServiceEvidence
    signalling = [ordered]@{
        passed = $StatusPassed
        status = $Status
    }
    nvenc = [ordered]@{
        passed = $NvencPassed
        log = $GameLog
    }
    private_ports_blocked = $FirewallPassed
    access = $AccessEvidence
}

$OutputDirectory = Split-Path -Parent $OutputPath
if ($OutputDirectory) { New-Item -ItemType Directory -Path $OutputDirectory -Force | Out-Null }
$Evidence | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $OutputPath -Encoding UTF8

if (-not $Passed) {
    throw "Streaming host preflight failed. Evidence was written to $OutputPath"
}

Write-Host "Streaming host preflight passed. Evidence: $OutputPath"
