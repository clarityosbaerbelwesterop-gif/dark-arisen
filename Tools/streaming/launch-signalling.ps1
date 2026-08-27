[CmdletBinding()]
param(
    [string]$InfrastructureRoot = $env:PIXEL_STREAMING_INFRA_ROOT,
    [string]$RuntimeRoot = "$env:ProgramData\DarkArisen"
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Require-EnvironmentValue([string]$Name) {
    $Value = [Environment]::GetEnvironmentVariable($Name)
    if ([string]::IsNullOrWhiteSpace($Value)) { throw "$Name is required." }
    return $Value
}

if ([string]::IsNullOrWhiteSpace($InfrastructureRoot)) {
    throw "PIXEL_STREAMING_INFRA_ROOT or -InfrastructureRoot is required."
}

$TurnHost = Require-EnvironmentValue "DARKARISEN_TURN_HOST"
$TurnUsername = Require-EnvironmentValue "DARKARISEN_TURN_USERNAME"
$TurnCredential = Require-EnvironmentValue "DARKARISEN_TURN_CREDENTIAL"

$Node = Join-Path $InfrastructureRoot "SignallingWebServer\platform_scripts\cmd\node\node.exe"
$Server = Join-Path $InfrastructureRoot "SignallingWebServer\dist\index.js"
$WebRoot = Join-Path $InfrastructureRoot "SignallingWebServer\www"
foreach ($Path in @($Node, $Server, (Join-Path $WebRoot "player.html"))) {
    if (-not (Test-Path -LiteralPath $Path -PathType Leaf)) { throw "Missing runtime file: $Path" }
}

New-Item -ItemType Directory -Path $RuntimeRoot -Force | Out-Null
$PeerOptions = @{
    iceServers = @(
        @{
            urls = @(
                "stun:$TurnHost",
                "turn:$TurnHost`?transport=udp",
                "turn:$TurnHost`?transport=tcp"
            )
            username = $TurnUsername
            credential = $TurnCredential
        }
    )
}

$Configuration = @{
    log_folder = (Join-Path $RuntimeRoot "signalling-logs")
    log_level_console = "info"
    log_level_file = "info"
    streamer_port = "8888"
    player_port = "8080"
    sfu_port = "8889"
    max_players = "1"
    serve = $true
    http_root = $WebRoot
    homepage = "player.html"
    https = $false
    https_redirect = $false
    rest_api = $true
    peer_options = $PeerOptions
    log_config = $false
    stdin = $false
}

$ConfigPath = Join-Path $RuntimeRoot "signalling.runtime.json"
$Configuration | ConvertTo-Json -Depth 6 | Set-Content -LiteralPath $ConfigPath -Encoding UTF8
& (Join-Path $PSScriptRoot "secure-runtime-file.ps1") -Path $ConfigPath

& $Node $Server --config_file $ConfigPath
if ($LASTEXITCODE -ne 0) { throw "Signalling server exited with code $LASTEXITCODE." }
