#include "DarkArisenOps.h"

#include "Misc/Paths.h"

namespace DarkArisenOps
{
namespace
{
void RequireFile(const FString& Root, const FString& Relative, TArray<FString>& Errors)
{
    if (!FPaths::FileExists(FPaths::Combine(Root, Relative)))
    {
        Errors.Add(FString::Printf(TEXT("missing world-content file: %s"), *Relative));
    }
}

void RequireFragments(
    const FString& Root,
    const FString& Relative,
    std::initializer_list<const TCHAR*> Fragments,
    TArray<FString>& Errors)
{
    FString Text;
    if (!ReadText(FPaths::Combine(Root, Relative), Text))
    {
        Errors.Add(FString::Printf(TEXT("cannot read world-content file: %s"), *Relative));
        return;
    }
    for (const TCHAR* Fragment : Fragments)
    {
        if (!Text.Contains(Fragment, ESearchCase::CaseSensitive))
        {
            Errors.Add(FString::Printf(TEXT("%s missing world contract: %s"), *Relative, Fragment));
        }
    }
}
}

int32 ValidateWorldContentCommand(const FParsedArgs& Args)
{
    const FString Root = RepoRoot(Args);
    TArray<FString> Errors;

    for (const FString& Relative : {
        TEXT("Source/DarkArisen/World/AuthoredWorldRegionRegistry.h"),
        TEXT("Source/DarkArisen/World/AuthoredWorldRegionRegistry.cpp"),
        TEXT("Source/DarkArisen/World/AuthoredWorldContentSubsystem.h"),
        TEXT("Source/DarkArisen/World/AuthoredWorldContentSubsystem.cpp"),
        TEXT("Source/DarkArisen/Tests/AuthoredWorldRegionRegistrySpec.cpp")})
    {
        RequireFile(Root, Relative, Errors);
    }

    RequireFragments(Root, TEXT("Source/DarkArisen/World/AuthoredWorldRegionRegistry.h"), {
        TEXT("RequiredRegionCount = 8"),
        TEXT("bRuntimeMapAuthored = false"),
        TEXT("RuntimeMapPackage")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/World/AuthoredWorldRegionRegistry.cpp"), {
        TEXT("region.rexa-moran"),
        TEXT("region.fjordlund"),
        TEXT("region.ashenmoor"),
        TEXT("region.quiet-coast"),
        TEXT("region.pale-isle-silvera"),
        TEXT("region.at-sea"),
        TEXT("region.highmoore"),
        TEXT("region.06"),
        TEXT("design-gap.region-level-assets"),
        TEXT("may not claim a runtime map until a reviewed .umap exists")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/World/AuthoredWorldContentSubsystem.cpp"), {
        TEXT("FAuthoredWorldRegionRegistry::TryGet"),
        TEXT("RuntimeMapPackage.TrimStartAndEnd().IsEmpty()"),
        TEXT("LoadedRegion.bSourceRegistryKnown")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Tests/AuthoredWorldRegionRegistrySpec.cpp"), {
        TEXT("Exactly eight production/world regions are registered"),
        TEXT("Source does not falsely claim reviewed region .umap exists")}, Errors);

    if (!Errors.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("Authored world-content validation failed with %d error(s):"), Errors.Num());
        for (const FString& Error : Errors)
        {
            UE_LOG(LogTemp, Error, TEXT("- %s"), *Error);
        }
        return 1;
    }

    UE_LOG(LogTemp, Display, TEXT("Authored world-content source validation passed."));
    return 0;
}
}
