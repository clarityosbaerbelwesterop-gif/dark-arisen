[CmdletBinding()]
param(
    [string]$StatusUrl = "",
    [int]$PlayerPort = 8080,
    [int]$IdleMinutes = 30,
    [string]$ShutdownCommand = $env:DARKARISEN_SHUTDOWN_COMMAND
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

if ($IdleMinutes -lt 1) { throw "IdleMinutes must be positive." }
if ([string]::IsNullOrWhiteSpace($ShutdownCommand) -or
    -not (Test-Path -LiteralPath $ShutdownCommand -PathType Leaf)) {
    throw "A provider shutdown command is mandatory; Windows shutdown alone may continue billing."
}

function Get-PlayerCount {
    if (-not [string]::IsNullOrWhiteSpace($StatusUrl)) {
        $Status = Invoke-RestMethod -Uri $StatusUrl -Method Get -TimeoutSec 5
        return [int]$Status.player_count
    }

    # Caddy or Tailscale Serve proxies each active player's signalling
    # WebSocket from loopback to the private player port. The streamer uses a
    # different port, so it cannot keep the billed host alive by itself.
    $Connections = @(
        Get-NetTCPConnection -LocalPort $PlayerPort -State Established -ErrorAction Stop |
            Where-Object { $_.RemoteAddress -in @("127.0.0.1", "::1") }
    )
    return $Connections.Count
}

$IdleSince = Get-Date
while ($true) {
    try {
        if ((Get-PlayerCount) -gt 0) {
            $IdleSince = Get-Date
        } elseif (((Get-Date) - $IdleSince).TotalMinutes -ge $IdleMinutes) {
            & $ShutdownCommand
            if ($LASTEXITCODE -ne 0) { throw "Provider shutdown adapter failed." }
            exit 0
        }
    } catch {
        # A failed local health check must not reset the idle timer or hide a dead stack.
        Write-Warning $_
    }
    Start-Sleep -Seconds 30
}
