[CmdletBinding()]
param(
    [string]$RuntimeRoot = "$env:ProgramData\DarkArisen",
    [string]$PublicHost = $env:DARKARISEN_STREAM_HOST,
    [string]$OutputPath
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

if ([string]::IsNullOrWhiteSpace($PublicHost)) {
    throw "DARKARISEN_STREAM_HOST or -PublicHost is required."
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
    "DarkArisenGateway",
    "DarkArisenIdleShutdown"
)
$ServiceEvidence = foreach ($Name in $RequiredServices) {
    $Service = Get-Service -Name $Name -ErrorAction SilentlyContinue
    [ordered]@{
        name = $Name
        status = if ($Service) { [string]$Service.Status } else { "Missing" }
        passed = $Service -and $Service.Status -eq "Running"
    }
}

$Status = $null
$StatusPassed = $false
try {
    $Status = Invoke-RestMethod -Uri "http://127.0.0.1:8080/api/status" -TimeoutSec 5
    $StatusPassed = [int]$Status.streamer_count -eq 1 -and [int]$Status.player_count -le 1
} catch {
    $Status = [ordered]@{ error = $_.Exception.Message }
}

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

$PublicStatus = 0
$PublicUnauthorized = $false
try {
    $Request = [System.Net.HttpWebRequest]::Create("https://$PublicHost/")
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
$PublicUnauthorized = $PublicStatus -eq 401

$Passed = ($ServiceEvidence.passed -notcontains $false) -and
    $StatusPassed -and $NvencPassed -and $FirewallPassed -and $PublicUnauthorized

$Evidence = [ordered]@{
    schema = "dark-arisen.streaming-host-evidence.v1"
    collected_at_utc = (Get-Date).ToUniversalTime().ToString("o")
    public_host = $PublicHost
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
    public_https_requires_authentication = [ordered]@{
        passed = $PublicUnauthorized
        status_code = $PublicStatus
    }
}

$OutputDirectory = Split-Path -Parent $OutputPath
if ($OutputDirectory) { New-Item -ItemType Directory -Path $OutputDirectory -Force | Out-Null }
$Evidence | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $OutputPath -Encoding UTF8

if (-not $Passed) {
    throw "Streaming host preflight failed. Evidence was written to $OutputPath"
}

Write-Host "Streaming host preflight passed. Evidence: $OutputPath"
