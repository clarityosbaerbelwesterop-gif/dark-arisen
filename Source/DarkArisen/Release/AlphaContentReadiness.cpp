// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Release/AlphaContentReadiness.h"

TArray<FAlphaContentFamilyStatus> FAlphaContentReadiness::BuildCurrentSnapshot()
{
    return {
        {
            TEXT("alpha-content.named-dungeons"), TEXT("Named dungeons"), 41, 40, 0,
            TEXT("AuthoredDungeonCatalog; dungeon system global.md"),
            TEXT("One Region 06 named/Tier-E slot remains unresolved; no reviewed production dungeon .umap asset set exists.")
        },
        {
            TEXT("alpha-content.minor-dungeons"), TEXT("Minor/Tier-A dungeon identities"), 20, 0, 0,
            TEXT("AuthoredMinorDungeonCatalog; dungeon system global.md"),
            TEXT("Twenty regional slots are reserved, but individual names/placements/mechanics are not authored in canon.")
        },
        {
            TEXT("alpha-content.crystal-caves"), TEXT("Crystal Caves passage"), 1, 1, 0,
            TEXT("crystal caves.md; CrystalCavesPassageComponent"),
            TEXT("C++ passage contract exists; reviewed production passage level/assets are not evidenced.")
        },
        {
            TEXT("alpha-content.threads"), TEXT("Narrative Threads"), 17, 17, 0,
            TEXT("AuthoredQuestCatalog; side quest catalog.md"),
            TEXT("Thread identities are registered; complete playable quest/dialogue/level asset production is not evidenced.")
        },
        {
            TEXT("alpha-content.turns"), TEXT("Finite Turns"), 132, 3, 0,
            TEXT("MissionScaleRequirements; side quest catalog.md"),
            TEXT("Only three individual Turn identities are grounded; 129 require authored people/place/outcome content.")
        },
        {
            TEXT("alpha-content.standing"), TEXT("Finite Standing variants"), 147, 1, 0,
            TEXT("MissionScaleRequirements; mission types.md"),
            TEXT("Only San Telmo Bell is a complete native identity; 146 variants and a complete region/type allocation remain unauthored.")
        },
        {
            TEXT("alpha-content.tier1-bosses"), TEXT("Tier-1 commanders"), 9, 9, 0,
            TEXT("Tier1CommanderCatalog; colonial war bosses.md"),
            TEXT("All nine encounter contracts are source-grounded, but production character/arena/animation/audio assets are not evidenced.")
        },
        {
            TEXT("alpha-content.world-regions"), TEXT("World region production maps"), 8, 8, 0,
            TEXT("AuthoredWorldRegionRegistry"),
            TEXT("All eight production identities are grounded; zero reviewed region .umap packages are evidenced.")
        },
        {
            TEXT("alpha-content.cutscenes"), TEXT("Authored cutscenes"), 19, 14, 0,
            TEXT("PresentationProductionCatalog; cutscene catalog.md"),
            TEXT("Five final-act cutscene identities remain unresolved and no reviewed Sequencer/capture asset set is evidenced.")
        },
        {
            TEXT("alpha-content.protected-playable"), TEXT("Protected playable moments"), 22, 22, 0,
            TEXT("PresentationProductionCatalog; cutscene catalog.md Section 6"),
            TEXT("All twenty-two ownership laws are grounded; runtime level/animation implementation evidence remains absent.")
        },
        {
            TEXT("alpha-content.named-animations"), TEXT("Named animation requirements"), 11, 11, 0,
            TEXT("AnimationProductionCatalog; animation system.md Section 7"),
            TEXT("Eleven requirements are specified; no reviewed imported animation asset set is evidenced.")
        },
        {
            TEXT("alpha-content.audio-core"), TEXT("Core audio/music requirements"), 12, 12, 0,
            TEXT("AudioProductionCatalog; cutscene catalog.md; highmoore music.md by non-conflicting detail"),
            TEXT("Audio laws are grounded; no reviewed final masters/imported audio asset set or approved music rights are evidenced.")
        },
        {
            TEXT("alpha-content.core-crew-voice"), TEXT("Core crew shipping voice roles"), 5, 5, 0,
            TEXT("VoiceProductionCatalog; named crew deep dives.md"),
            TEXT("Five role directions and auditions exist, but zero shipping voice assets have commercial-rights/import/runtime acceptance.")
        },
        {
            TEXT("alpha-content.platform-builds"), TEXT("Alpha platform Shipping candidates"), 2, 2, 0,
            TEXT("AlphaReleaseContracts"),
            TEXT("Win64 and Linux x86_64 target identities exist; no same-commit verified Shipping packages are evidenced while runners are offline.")
        }
    };
}

bool FAlphaContentReadiness::IsContentProductionComplete(TArray<FString>& OutBlockers)
{
    OutBlockers.Reset();
    for (const FAlphaContentFamilyStatus& Family : BuildCurrentSnapshot())
    {
        if (!Family.IsSourceIdentityComplete() || !Family.IsProductionComplete())
        {
            OutBlockers.Add(FString::Printf(
                TEXT("%s: source %d/%d, production %d/%d — %s"),
                *Family.DisplayName,
                Family.SourceGroundedCount,
                Family.RequiredCount,
                Family.ProductionAssetCount,
                Family.RequiredCount,
                *Family.BlockingReason));
        }
    }
    return OutBlockers.IsEmpty();
}

bool FAlphaContentReadiness::ValidateSnapshot(TArray<FString>& OutErrors)
{
    OutErrors.Reset();
    const TArray<FAlphaContentFamilyStatus> Snapshot = BuildCurrentSnapshot();
    if (Snapshot.Num() != 14)
    {
        OutErrors.Add(FString::Printf(TEXT("Alpha content reconciliation requires 14 tracked families; found %d."), Snapshot.Num()));
    }

    TSet<FName> Seen;
    for (const FAlphaContentFamilyStatus& Family : Snapshot)
    {
        if (Family.StableId.IsNone()
            || Family.DisplayName.IsEmpty()
            || Family.RequiredCount <= 0
            || Family.SourceGroundedCount < 0
            || Family.SourceGroundedCount > Family.RequiredCount
            || Family.ProductionAssetCount < 0
            || Family.ProductionAssetCount > Family.RequiredCount
            || Family.GoverningSource.IsEmpty()
            || Family.BlockingReason.IsEmpty())
        {
            OutErrors.Add(TEXT("Alpha content family has invalid reconciliation metadata."));
        }
        if (Seen.Contains(Family.StableId))
        {
            OutErrors.Add(FString::Printf(TEXT("Duplicate Alpha content family id: %s"), *Family.StableId.ToString()));
        }
        Seen.Add(Family.StableId);
    }

    TArray<FString> Blockers;
    if (IsContentProductionComplete(Blockers))
    {
        OutErrors.Add(TEXT("Current source snapshot must not self-certify Alpha content production while real assets/runtime evidence are absent."));
    }
    if (Blockers.IsEmpty())
    {
        OutErrors.Add(TEXT("Current Alpha content snapshot must expose concrete blockers."));
    }

    return OutErrors.IsEmpty();
}
