[CmdletBinding()]
param(
    [string]$EngineRoot = $env:UE55_ROOT,
    [string]$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")),
    [Parameter(Mandatory = $true)]
    [ValidatePattern("^[0-9a-fA-F]{40}$")]
    [string]$Revision,
    [string]$OutputRoot
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Invoke-NativeChecked {
    param(
        [Parameter(Mandatory = $true)][string]$FilePath,
        [Parameter(Mandatory = $true)][string[]]$Arguments
    )

    & $FilePath @Arguments
    if ($LASTEXITCODE -ne 0) {
        throw "Native command failed with exit code $LASTEXITCODE`: $FilePath"
    }
}

if (-not $IsWindows -or -not [Environment]::Is64BitOperatingSystem) {
    throw "The Windows Alpha candidate must be packaged on Windows x64."
}
if ([string]::IsNullOrWhiteSpace($EngineRoot)) {
    throw "UE55_ROOT or -EngineRoot must point to an Unreal Engine 5.5 Windows installation."
}

$ProjectRoot = [System.IO.Path]::GetFullPath($ProjectRoot)
$EngineRoot = [System.IO.Path]::GetFullPath($EngineRoot)
if ([string]::IsNullOrWhiteSpace($OutputRoot)) {
    $OutputRoot = Join-Path $ProjectRoot "Artifacts\AlphaCandidates"
}
$OutputRoot = [System.IO.Path]::GetFullPath($OutputRoot)
if ([System.IO.Path]::GetPathRoot($OutputRoot) -eq $OutputRoot) {
    throw "OutputRoot may not be a filesystem root."
}

$ProjectFile = Join-Path $ProjectRoot "DarkArisen.uproject"
$RunUat = Join-Path $EngineRoot "Engine\Build\BatchFiles\RunUAT.bat"
$BuildVersionPath = Join-Path $EngineRoot "Engine\Build\Build.version"
foreach ($Path in @($ProjectFile, $RunUat, $BuildVersionPath)) {
    if (-not (Test-Path -LiteralPath $Path -PathType Leaf)) {
        throw "Required packaging file is missing: $Path"
    }
}

$ActualRevision = (& git.exe -C $ProjectRoot rev-parse HEAD).Trim().ToLowerInvariant()
if ($LASTEXITCODE -ne 0) { throw "git.exe rev-parse HEAD failed." }
$Revision = $Revision.ToLowerInvariant()
if ($ActualRevision -ne $Revision) {
    throw "Requested revision $Revision does not match checked-out commit $ActualRevision."
}
$TrackedChanges = @(& git.exe -C $ProjectRoot status --porcelain --untracked-files=no)
if ($LASTEXITCODE -ne 0) { throw "git status failed." }
if ($TrackedChanges.Count -gt 0) {
    throw "Tracked source changes are present. Package only an exact committed revision."
}

$StageRoot = Join-Path $ProjectRoot "Saved\AlphaPackaging\Windows\$Revision"
if (Test-Path -LiteralPath $StageRoot) {
    throw "Candidate staging path already exists; inspect or move it before retrying: $StageRoot"
}
New-Item -ItemType Directory -Path $StageRoot -Force | Out-Null
New-Item -ItemType Directory -Path $OutputRoot -Force | Out-Null

Invoke-NativeChecked -FilePath $RunUat -Arguments @(
    "BuildCookRun",
    "-project=$ProjectFile",
    "-noP4",
    "-platform=Win64",
    "-clientconfig=Shipping",
    "-build",
    "-cook",
    "-allmaps",
    "-stage",
    "-pak",
    "-archive",
    "-archivedirectory=$StageRoot",
    "-prereqs",
    "-nodebuginfo",
    "-utf8output"
)

$Executable = Get-ChildItem -LiteralPath $StageRoot -Filter "DarkArisen.exe" -File -Recurse |
    Select-Object -First 1
$Pak = Get-ChildItem -LiteralPath $StageRoot -Filter "*.pak" -File -Recurse |
    Select-Object -First 1
if (-not $Executable) { throw "Packaged DarkArisen.exe is missing from $StageRoot" }
if (-not $Pak) { throw "Packaged .pak content is missing from $StageRoot" }

$ForbiddenNames = Get-ChildItem -LiteralPath $StageRoot -File -Recurse | Where-Object {
    $_.Name -match '(?i)(^\.env($|\.)|^id_rsa$|^credentials.*|.*\.(pem|pfx|p12|key)$|.*\.local\.(json|psd1)$)'
}
if ($ForbiddenNames) {
    throw "Candidate contains a forbidden credential-like file: $($ForbiddenNames[0].FullName)"
}
$TextFiles = Get-ChildItem -LiteralPath $StageRoot -File -Recurse | Where-Object {
    $_.Extension -match '(?i)^\.(ini|json|txt|cfg|xml|yaml|yml)$'
}
foreach ($TextFile in $TextFiles) {
    if (Select-String -LiteralPath $TextFile.FullName -Pattern 'AKIA[0-9A-Z]{16}|tskey-[A-Za-z0-9_-]+|-----BEGIN (RSA |EC |OPENSSH )?PRIVATE KEY-----' -Quiet) {
        throw "Candidate contains credential material in $($TextFile.FullName)"
    }
}

$BuildVersion = Get-Content -LiteralPath $BuildVersionPath -Raw | ConvertFrom-Json
if ($BuildVersion.MajorVersion -ne 5 -or $BuildVersion.MinorVersion -ne 5) {
    throw "Packaging engine is not Unreal Engine 5.5."
}
$PayloadFiles = @(Get-ChildItem -LiteralPath $StageRoot -File -Recurse)
$PayloadBytes = ($PayloadFiles | Measure-Object -Property Length -Sum).Sum
$Manifest = [ordered]@{
    schema_version = 1
    artifact_kind = "candidate"
    alpha_status = "not-accepted"
    project = "DarkArisen"
    platform = "Win64"
    configuration = "Shipping"
    candidate_commit = $Revision
    engine_version = "$($BuildVersion.MajorVersion).$($BuildVersion.MinorVersion).$($BuildVersion.PatchVersion)"
    pixel_streaming = $true
    content_file_count = $PayloadFiles.Count
    content_bytes = $PayloadBytes
    generated_utc = [DateTime]::UtcNow.ToString("o")
    notice = "Private single-reviewer candidate. Not an accepted Alpha until promoted with complete evidence."
}
$ManifestPath = Join-Path $StageRoot "DarkArisen-build-manifest.json"
$Manifest | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath $ManifestPath -Encoding utf8NoBOM

$ShortRevision = $Revision.Substring(0, 12)
$ZipName = "DarkArisen-Candidate-Windows-Arcware-$ShortRevision.zip"
$ZipPath = Join-Path $OutputRoot $ZipName
if (Test-Path -LiteralPath $ZipPath) {
    throw "Candidate ZIP already exists: $ZipPath"
}
Add-Type -AssemblyName System.IO.Compression.FileSystem
[System.IO.Compression.ZipFile]::CreateFromDirectory(
    $StageRoot,
    $ZipPath,
    [System.IO.Compression.CompressionLevel]::Optimal,
    $false
)
$Hash = (Get-FileHash -LiteralPath $ZipPath -Algorithm SHA256).Hash.ToLowerInvariant()
Set-Content -LiteralPath "$ZipPath.sha256" -Value "$Hash  $ZipName" -Encoding ascii

Write-Host "Windows Alpha candidate created privately: $ZipPath"
Write-Host "SHA-256: $Hash"
Write-Host "It remains a Candidate until Tools/ci/promote-alpha-candidates.py verifies complete acceptance evidence."
