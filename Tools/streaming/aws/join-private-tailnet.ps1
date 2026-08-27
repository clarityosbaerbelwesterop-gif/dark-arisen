[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$AuthKeyParameterArn,
    [string]$Region = $env:AWS_REGION,
    [string]$Hostname = "dark-arisen-alpha",
    [string]$Tag = "tag:dark-arisen",
    [string]$TailscaleExe = "C:\Program Files\Tailscale\tailscale.exe",
    [switch]$ValidateOnly
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

if ([string]::IsNullOrWhiteSpace($Region)) { throw "AWS region is required." }
if ($Tag -ne "tag:dark-arisen") { throw "The private host must use tag:dark-arisen." }
if ($Hostname -notmatch "^[a-z0-9][a-z0-9-]{0,62}$") { throw "Hostname is invalid." }
if ($AuthKeyParameterArn -notmatch "^arn:aws[a-zA-Z-]*:ssm:[a-z0-9-]+:[0-9]{12}:parameter/.+$") {
    throw "AuthKeyParameterArn must be an exact SSM Parameter Store ARN."
}
if (-not (Test-Path -LiteralPath $TailscaleExe -PathType Leaf)) {
    throw "Tailscale CLI is missing: $TailscaleExe"
}

$SsmCommand = Get-Command Get-SSMParameterValue -ErrorAction SilentlyContinue
if ($null -eq $SsmCommand) {
    foreach ($ModuleName in @("AWS.Tools.SimpleSystemsManagement", "AWSPowerShell")) {
        Import-Module $ModuleName -ErrorAction SilentlyContinue
        $SsmCommand = Get-Command Get-SSMParameterValue -ErrorAction SilentlyContinue
        if ($null -ne $SsmCommand) { break }
    }
}
if ($null -eq $SsmCommand) {
    throw "AWS Tools for PowerShell with Get-SSMParameterValue is required."
}

if ($ValidateOnly) {
    Write-Host "Private tailnet join prerequisites passed without reading a secret or changing state."
    exit 0
}

$RuntimeRoot = Join-Path $env:ProgramData "DarkArisen"
New-Item -ItemType Directory -Path $RuntimeRoot -Force | Out-Null
$AuthKeyPath = Join-Path $RuntimeRoot ("tailscale-auth-{0}.tmp" -f [Guid]::NewGuid().ToString("N"))
$AuthKey = $null

try {
    $Response = Get-SSMParameterValue -Name @($AuthKeyParameterArn) -WithDecryption $true -Region $Region
    if ($null -eq $Response.Parameters -or $Response.Parameters.Count -ne 1) {
        throw "The exact Tailscale SecureString parameter could not be read."
    }
    $AuthKey = $Response.Parameters[0].Value
    if ([string]::IsNullOrWhiteSpace($AuthKey) -or $AuthKey -notmatch "^tskey-auth-") {
        throw "The SecureString does not contain a Tailscale auth key."
    }

    [System.IO.File]::WriteAllText(
        $AuthKeyPath,
        $AuthKey,
        [System.Text.UTF8Encoding]::new($false)
    )
    $FileSecurity = [System.Security.AccessControl.FileSecurity]::new()
    $FileSecurity.SetAccessRuleProtection($true, $false)
    foreach ($SidValue in @("S-1-5-18", "S-1-5-32-544")) {
        $Sid = [System.Security.Principal.SecurityIdentifier]::new($SidValue)
        $Rule = [System.Security.AccessControl.FileSystemAccessRule]::new(
            $Sid,
            [System.Security.AccessControl.FileSystemRights]::FullControl,
            [System.Security.AccessControl.AccessControlType]::Allow
        )
        $FileSecurity.AddAccessRule($Rule)
    }
    Set-Acl -LiteralPath $AuthKeyPath -AclObject $FileSecurity

    & $TailscaleExe up `
        "--auth-key=file:$AuthKeyPath" `
        "--hostname=$Hostname" `
        "--advertise-tags=$Tag" `
        "--unattended=true" `
        "--accept-routes=false" `
        "--accept-dns=true" `
        "--shields-up=false"
    if ($LASTEXITCODE -ne 0) { throw "Tailscale authentication failed." }
} finally {
    $AuthKey = $null
    Remove-Variable AuthKey -ErrorAction SilentlyContinue
    if (Test-Path -LiteralPath $AuthKeyPath) {
        [System.IO.File]::WriteAllBytes($AuthKeyPath, [byte[]]@())
        Remove-Item -LiteralPath $AuthKeyPath -Force
    }
}

# Funnel is the public-internet feature. Reset it before configuring tailnet-only Serve.
& $TailscaleExe funnel reset | Out-Null
if ($LASTEXITCODE -ne 0) { throw "Unable to prove that Tailscale Funnel is disabled." }
& $TailscaleExe serve reset | Out-Null
if ($LASTEXITCODE -ne 0) { throw "Unable to reset Tailscale Serve." }
& $TailscaleExe serve --bg --https=443 http://127.0.0.1:8080 | Out-Null
if ($LASTEXITCODE -ne 0) { throw "Unable to configure tailnet-only HTTPS reverse proxy." }

$StatusJson = & $TailscaleExe status --json
if ($LASTEXITCODE -ne 0) { throw "Unable to read Tailscale status." }
$Status = $StatusJson | ConvertFrom-Json
$DnsName = "$($Status.Self.DNSName)".TrimEnd(".")
$TailnetIp = @($Status.TailscaleIPs | Where-Object { $_ -match "^100\." }) | Select-Object -First 1
if ([string]::IsNullOrWhiteSpace($DnsName) -or [string]::IsNullOrWhiteSpace($TailnetIp)) {
    throw "Tailscale did not return a stable DNS name and IPv4 address."
}

[Environment]::SetEnvironmentVariable("DARKARISEN_STREAM_HOST", $DnsName, "Machine")
[Environment]::SetEnvironmentVariable("DARKARISEN_TURN_HOST", $TailnetIp, "Machine")
[Environment]::SetEnvironmentVariable("DARKARISEN_LOCAL_IP", $TailnetIp, "Machine")
[Environment]::SetEnvironmentVariable("DARKARISEN_PRIVATE_OVERLAY", "1", "Machine")

@{
    dns_name = $DnsName
    funnel_enabled = $false
    https_url = "https://$DnsName"
    player_scope = "tailnet-policy-only"
    tailnet_ip = $TailnetIp
} | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $RuntimeRoot "private-tailnet.json") -Encoding UTF8

Write-Host "Private tailnet endpoint configured. Revoke the one-off key and delete its SSM parameter now."
