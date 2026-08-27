[CmdletBinding()]
param(
    [string]$InfrastructureRoot = $env:PIXEL_STREAMING_INFRA_ROOT,
    [string]$RuntimeRoot = "$env:ProgramData\DarkArisen",
    [int]$ListeningPort = 3478,
    [int]$RelayPortStart = 49160,
    [int]$RelayPortEnd = 49200
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

$Username = Require-EnvironmentValue "DARKARISEN_TURN_USERNAME"
$Credential = Require-EnvironmentValue "DARKARISEN_TURN_CREDENTIAL"
$Realm = Require-EnvironmentValue "DARKARISEN_TURN_REALM"
$PublicIp = Require-EnvironmentValue "DARKARISEN_PUBLIC_IP"
$LocalIp = Require-EnvironmentValue "DARKARISEN_LOCAL_IP"

$TurnServer = Join-Path $InfrastructureRoot "SignallingWebServer\platform_scripts\cmd\coturn\turnserver.exe"
if (-not (Test-Path -LiteralPath $TurnServer -PathType Leaf)) {
    throw "coturn is missing; run bootstrap-infrastructure.ps1 first."
}

New-Item -ItemType Directory -Path $RuntimeRoot -Force | Out-Null
$ConfigPath = Join-Path $RuntimeRoot "turnserver.runtime.conf"
@"
listening-port=$ListeningPort
min-port=$RelayPortStart
max-port=$RelayPortEnd
listening-ip=$LocalIp
relay-ip=$LocalIp
external-ip=$PublicIp/$LocalIp
realm=$Realm
fingerprint
lt-cred-mech
user=$Username`:$Credential
no-cli
no-tls
no-dtls
no-multicast-peers
stale-nonce=600
"@ | Set-Content -LiteralPath $ConfigPath -Encoding ASCII
& (Join-Path $PSScriptRoot "secure-runtime-file.ps1") -Path $ConfigPath

& $TurnServer -c $ConfigPath
if ($LASTEXITCODE -ne 0) { throw "coturn exited with code $LASTEXITCODE." }

