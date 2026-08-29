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
        Errors.Add(FString::Printf(TEXT("missing reward file: %s"), *Relative));
    }
}

void RequireFragments(const FString& Root, const FString& Relative, std::initializer_list<const TCHAR*> Fragments, TArray<FString>& Errors)
{
    FString Text;
    if (!ReadText(FPaths::Combine(Root, Relative), Text))
    {
        Errors.Add(FString::Printf(TEXT("cannot read reward file: %s"), *Relative));
        return;
    }
    for (const TCHAR* Fragment : Fragments)
    {
        if (!Text.Contains(Fragment, ESearchCase::CaseSensitive))
        {
            Errors.Add(FString::Printf(TEXT("%s missing reward contract: %s"), *Relative, Fragment));
        }
    }
}
}

int32 ValidateRewardContentCommand(const FParsedArgs& Args)
{
    const FString Root = RepoRoot(Args);
    TArray<FString> Errors;

    for (const FString& Relative : {
        TEXT("Source/DarkArisen/ContentScale/AuthoredRewardCatalog.h"),
        TEXT("Source/DarkArisen/ContentScale/AuthoredRewardCatalog.cpp"),
        TEXT("Source/DarkArisen/Tests/AuthoredRewardCatalogSpec.cpp")})
    {
        RequireFile(Root, Relative, Errors);
    }

    RequireFragments(Root, TEXT("Source/DarkArisen/ContentScale/AuthoredRewardCatalog.h"), {
        TEXT("StateTreasureSlotCount = 9"),
        TEXT("ExplicitNothing"),
        TEXT("bIdentityAuthored = false"),
        TEXT("bWithheldOrUnresolved = false")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/ContentScale/AuthoredRewardCatalog.cpp"), {
        TEXT("BuildAllKnownProfiles"),
        TEXT("Expected one reward/outcome binding for each of 40 grounded dungeon profiles"),
        TEXT("reward.state-treasure.slot-%02d"),
        TEXT("This slot does not invent its identity"),
        TEXT("reward.unique.crystal-katana"),
        TEXT("bRewardExplicitlyNone"),
        TEXT("bRewardWithheldOrUnresolved")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Tests/AuthoredRewardCatalogSpec.cpp"), {
        TEXT("First House explicitly rewards nothing"),
        TEXT("Undercity route is tactical access, not generic loot"),
        TEXT("Unnamed state treasure is not falsely authored"),
        TEXT("Crystal Katana remains the named unique reward")}, Errors);

    if (!Errors.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("Reward validation failed with %d error(s):"), Errors.Num());
        for (const FString& Error : Errors)
        {
            UE_LOG(LogTemp, Error, TEXT("- %s"), *Error);
        }
        return 1;
    }

    UE_LOG(LogTemp, Display, TEXT("Authored reward source validation passed."));
    return 0;
}
}
