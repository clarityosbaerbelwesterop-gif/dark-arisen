[CmdletBinding()]
param(
    [string]$StatusUrl = "http://127.0.0.1:8080/api/status",
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

$IdleSince = Get-Date
while ($true) {
    try {
        $Status = Invoke-RestMethod -Uri $StatusUrl -Method Get -TimeoutSec 5
        if ([int]$Status.player_count -gt 0) {
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

