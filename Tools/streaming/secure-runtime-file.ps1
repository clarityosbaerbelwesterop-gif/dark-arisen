param(
    [Parameter(Mandatory = $true)][string]$Path
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$Identity = [System.Security.Principal.WindowsIdentity]::GetCurrent().Name
& icacls.exe $Path /inheritance:r /grant:r "${Identity}:(F)" "SYSTEM:(F)" | Out-Null
if ($LASTEXITCODE -ne 0) {
    throw "Unable to restrict ACLs on $Path"
}

