[CmdletBinding()]
param(
    [string]$InstanceId = $env:DARKARISEN_AWS_INSTANCE_ID,
    [string]$Region = $env:AWS_REGION,
    [switch]$ValidateOnly
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

if ([string]::IsNullOrWhiteSpace($InstanceId)) { throw "DARKARISEN_AWS_INSTANCE_ID is required." }
if ([string]::IsNullOrWhiteSpace($Region)) { throw "AWS_REGION is required." }

if ($ValidateOnly) {
    & aws ec2 describe-instances --instance-ids $InstanceId --region $Region `
        --query "Reservations[0].Instances[0].InstanceId" --output text | Out-Null
    if ($LASTEXITCODE -ne 0) { throw "AWS instance identity or resource validation failed." }
    exit 0
}

& aws ec2 stop-instances --instance-ids $InstanceId --region $Region --output json | Out-Null
if ($LASTEXITCODE -ne 0) { throw "AWS instance stop failed." }
