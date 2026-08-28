[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$S3Key,
    [Parameter(Mandatory = $true)][string]$Sha256,
    [string]$Bucket = "ec2-windows-nvidia-drivers",
    [string]$Region = "us-east-1",
    [switch]$AcceptNvidiaGridEula,
    [switch]$ValidateOnly,
    [switch]$NoRestart
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

if ($Bucket -ne "ec2-windows-nvidia-drivers") { throw "Only the AWS NVIDIA driver bucket is allowed." }
if ($S3Key -notmatch "^latest/[A-Za-z0-9._/-]+\.exe$") { throw "Use one exact reviewed latest/*.exe key." }
if ($Sha256 -notmatch "^[A-Fa-f0-9]{64}$") { throw "Sha256 must contain 64 hexadecimal characters." }

$CopyCommand = Get-Command Copy-S3Object -ErrorAction SilentlyContinue
if ($null -eq $CopyCommand) {
    foreach ($ModuleName in @("AWS.Tools.S3", "AWSPowerShell")) {
        Import-Module $ModuleName -ErrorAction SilentlyContinue
        $CopyCommand = Get-Command Copy-S3Object -ErrorAction SilentlyContinue
        if ($null -ne $CopyCommand) { break }
    }
}
if ($null -eq $CopyCommand) { throw "AWS Tools for PowerShell with Copy-S3Object is required." }

if ($ValidateOnly) {
    Write-Host "GRID driver installation parameters passed without downloading or installing software."
    exit 0
}
if (-not $AcceptNvidiaGridEula) {
    throw "-AcceptNvidiaGridEula is required after reviewing the AWS/NVIDIA GRID EULA."
}

$RuntimeRoot = Join-Path $env:ProgramData "DarkArisen\Nvidia"
New-Item -ItemType Directory -Path $RuntimeRoot -Force | Out-Null
$InstallerPath = Join-Path $RuntimeRoot ([System.IO.Path]::GetFileName($S3Key))

Copy-S3Object -BucketName $Bucket -Key $S3Key -LocalFile $InstallerPath -Region $Region
if (-not (Test-Path -LiteralPath $InstallerPath -PathType Leaf)) { throw "GRID driver download failed." }

$ActualHash = (Get-FileHash -LiteralPath $InstallerPath -Algorithm SHA256).Hash
if ($ActualHash -ne $Sha256.ToUpperInvariant()) { throw "GRID driver SHA-256 mismatch." }
$Signature = Get-AuthenticodeSignature -FilePath $InstallerPath
if ($Signature.Status -ne "Valid" -or $Signature.SignerCertificate.Subject -notmatch "NVIDIA") {
    throw "GRID driver does not have the expected valid NVIDIA signature."
}

$Installer = Start-Process -FilePath $InstallerPath -ArgumentList "-s" -Wait -PassThru
if ($Installer.ExitCode -notin @(0, 3010)) {
    throw "GRID driver installation failed with exit code $($Installer.ExitCode)."
}

$NvidiaSmi = Join-Path $env:ProgramFiles "NVIDIA Corporation\NVSMI\nvidia-smi.exe"
if (-not (Test-Path -LiteralPath $NvidiaSmi -PathType Leaf)) {
    throw "nvidia-smi is missing after driver installation."
}
& $NvidiaSmi --query-gpu=name,driver_version --format=csv,noheader
if ($LASTEXITCODE -ne 0) { throw "The NVIDIA GPU/driver verification failed." }

if ($NoRestart) {
    Write-Warning "GRID driver installed; restart is still required before Unreal/NVENC validation."
} else {
    Restart-Computer -Force
}

