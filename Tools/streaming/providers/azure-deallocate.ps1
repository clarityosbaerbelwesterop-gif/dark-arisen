[CmdletBinding()]
param(
    [string]$ResourceId = $env:DARKARISEN_AZURE_VM_RESOURCE_ID,
    [switch]$ValidateOnly
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

if ([string]::IsNullOrWhiteSpace($ResourceId)) {
    throw "DARKARISEN_AZURE_VM_RESOURCE_ID is required."
}

& az login --identity --allow-no-subscriptions --output none
if ($LASTEXITCODE -ne 0) { throw "Azure managed-identity login failed." }

if ($ValidateOnly) {
    & az vm show --ids $ResourceId --query id --output none --only-show-errors
    if ($LASTEXITCODE -ne 0) { throw "Azure VM identity or resource validation failed." }
    exit 0
}

& az vm deallocate --ids $ResourceId --no-wait --only-show-errors
if ($LASTEXITCODE -ne 0) { throw "Azure VM deallocation failed." }
