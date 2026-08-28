[CmdletBinding()]
param(
    [string]$InfrastructureRoot = $env:PIXEL_STREAMING_INFRA_ROOT,
    [switch]$ForceRebuild
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$Repository = "https://github.com/EpicGames/PixelStreamingInfrastructure.git"
$Branch = "UE5.5"
$PinnedCommit = "c3e3abea6590a19e1c0ab4d2954efd6a1d949db3"

if ([string]::IsNullOrWhiteSpace($InfrastructureRoot)) {
    throw "PIXEL_STREAMING_INFRA_ROOT or -InfrastructureRoot is required."
}

$InfrastructureRoot = [System.IO.Path]::GetFullPath($InfrastructureRoot)
if (-not (Test-Path -LiteralPath $InfrastructureRoot)) {
    & git clone --branch $Branch --single-branch $Repository $InfrastructureRoot
    if ($LASTEXITCODE -ne 0) { throw "Unable to clone Pixel Streaming Infrastructure." }
}

$ManagedFrontendFiles = @(
    "Frontend/implementations/typescript/src/player.ts",
    "Frontend/implementations/typescript/src/player.html"
)
& git -C $InfrastructureRoot restore --source=HEAD --worktree -- $ManagedFrontendFiles
if ($LASTEXITCODE -ne 0) { throw "Unable to restore the managed frontend overlay." }

$UnmanagedChanges = & git -C $InfrastructureRoot status --porcelain
if ($LASTEXITCODE -ne 0) { throw "Unable to inspect Pixel Streaming Infrastructure." }
if ($UnmanagedChanges) {
    throw "InfrastructureRoot contains unmanaged changes. Preserve or remove them before bootstrapping."
}

& git -C $InfrastructureRoot fetch origin $PinnedCommit --depth 1
if ($LASTEXITCODE -ne 0) { throw "Unable to fetch pinned infrastructure commit $PinnedCommit." }
& git -C $InfrastructureRoot checkout --detach $PinnedCommit
if ($LASTEXITCODE -ne 0) { throw "Unable to pin Pixel Streaming Infrastructure to $PinnedCommit." }

$FrontendSource = Join-Path $PSScriptRoot "frontend"
$FrontendTarget = Join-Path $InfrastructureRoot "Frontend\implementations\typescript\src"
Copy-Item -LiteralPath (Join-Path $FrontendSource "player.ts") -Destination (Join-Path $FrontendTarget "player.ts") -Force
Copy-Item -LiteralPath (Join-Path $FrontendSource "player.html") -Destination (Join-Path $FrontendTarget "player.html") -Force

$SetupScript = Join-Path $InfrastructureRoot "SignallingWebServer\platform_scripts\cmd\setup.bat"
$SetupArguments = if ($ForceRebuild) {
    @("--deps", "--rebuild")
} else {
    @("--build", "--build-wilbur")
}
& $SetupScript @SetupArguments
if ($LASTEXITCODE -ne 0) { throw "Pixel Streaming dependency or frontend build failed." }

$ExpectedOutputs = @(
    (Join-Path $InfrastructureRoot "SignallingWebServer\dist\index.js"),
    (Join-Path $InfrastructureRoot "SignallingWebServer\www\player.html")
)
foreach ($Output in $ExpectedOutputs) {
    if (-not (Test-Path -LiteralPath $Output -PathType Leaf)) {
        throw "Expected Pixel Streaming output is missing: $Output"
    }
}

Write-Host "Pixel Streaming Infrastructure UE5.5 is pinned and built."
