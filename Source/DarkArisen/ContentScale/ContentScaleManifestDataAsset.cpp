// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "ContentScale/ContentScaleManifestDataAsset.h"

namespace
{
constexpr float DungeonMinuteTolerance = 0.01f;

bool AddUniqueStableId(const FName StableId, TSet<FName>& Seen, TArray<FString>& OutErrors, const TCHAR* Category)
{
    if (StableId.IsNone())
    {
        OutErrors.Add(FString::Printf(TEXT("%s entry has no stable id"), Category));
        return false;
    }
    if (Seen.Contains(StableId))
    {
        OutErrors.Add(FString::Printf(TEXT("duplicate %s stable id: %s"), Category, *StableId.ToString()));
        return false;
    }
    Seen.Add(StableId);
    return true;
}

void RequireSource(const FString& Source, const FName StableId, TArray<FString>& OutErrors, const TCHAR* Category)
{
    if (Source.TrimStartAndEnd().IsEmpty())
    {
        OutErrors.Add(FString::Printf(TEXT("%s %s has no governing source"), Category, *StableId.ToString()));
    }
}

/**
 * The authored cutscenes `cutscene catalog.md` Section 3 names individually.
 *
 * Section 3 locks nineteen, but names only these fourteen. Entries #15-#19 are deferred to
 * "bosses/crimson_armada.md and the main-story documents", and `bosses/crimson_armada.md` does
 * not exist in this repository. The five final-act sequences are therefore a genuine authoring
 * gap, not an omission here, and they are deliberately not invented.
 *
 * DESIGN-GAP: cutscenes #15-#19 have no governing document. Until one exists, the manifest is
 * pinned by identity for these fourteen and by count alone for the remaining five.
 */
const TSet<FName>& CanonicalAuthoredCutscenes()
{
    static const TSet<FName> Ids = {
        // Section 3.1 — Chapters 1-3, the opening.
        TEXT("cutscene.opening.the-brother"),
        TEXT("cutscene.opening.la-liberacion"),
        // Section 3.2 — Chapters 4-6, the archipelago.
        TEXT("cutscene.archipelago.first-letter"),
        TEXT("cutscene.archipelago.the-grove"),
        TEXT("cutscene.archipelago.dream-fight-entry"),
        TEXT("cutscene.archipelago.alejandro-strain"),
        // Section 3.3 — Chapters 6-8, Highmoore.
        TEXT("cutscene.highmoore.emergence"),
        TEXT("cutscene.highmoore.voice-from-behind"),
        TEXT("cutscene.highmoore.ejection"),
        TEXT("cutscene.highmoore.false-letter"),
        TEXT("cutscene.highmoore.arrow"),
        TEXT("cutscene.highmoore.real-letter"),
        TEXT("cutscene.highmoore.she-wrote-two"),
        TEXT("cutscene.highmoore.wizards-question")};
    return Ids;
}

const TSet<FName>& CanonicalProtectedPlayableMoments()
{
    static const TSet<FName> Ids = {
        TEXT("playable.arrow.after-thirty-seconds"),
        TEXT("playable.lake.standing"),
        TEXT("playable.archer.kill"),
        TEXT("playable.real-letter.reading"),
        TEXT("playable.cassian.kill"),
        TEXT("playable.belos.nine-minutes"),
        TEXT("playable.belos.walk-out"),
        TEXT("playable.stair.approach"),
        TEXT("playable.mountain.return"),
        TEXT("playable.churchyard"),
        TEXT("playable.boss-death.hold"),
        TEXT("playable.ibarra.garden"),
        TEXT("playable.guardian.settling"),
        TEXT("playable.light-elf.ending"),
        TEXT("playable.vidal.confession"),
        TEXT("playable.kettle.office"),
        TEXT("playable.aldric.ninety-seconds"),
        TEXT("playable.rowland.offer"),
        TEXT("playable.liberation.aftermath"),
        TEXT("playable.great-cabin.crew-scene"),
        TEXT("playable.maerwyn.breakfast-line"),
        TEXT("playable.stopped-shaft.first-sight")};
    return Ids;
}
}

bool FCreditsReadinessManifest::IsProductionReady() const
{
    return bNamesAndOrderApproved
        && bAttributionComplete
        && !MusicSource.TrimStartAndEnd().IsEmpty()
        && !MusicLicenceReference.TrimStartAndEnd().IsEmpty()
        && bMusicSourceApproved
        && bMusicLicenceApproved
        && bMusicCostApproved
        && bMusicNonReactive;
}

bool UContentScaleManifestDataAsset::IsTierBOrAbove(const EContentDungeonTier Tier)
{
    return Tier == EContentDungeonTier::TierB
        || Tier == EContentDungeonTier::TierC
        || Tier == EContentDungeonTier::TierD
        || Tier == EContentDungeonTier::TierE
        || Tier == EContentDungeonTier::CrystalCaves;
}

TSet<FName> UContentScaleManifestDataAsset::GetCanonicalTier1BossIds()
{
    return {
        TEXT("boss.herrera"),
        TEXT("boss.reyes"),
        TEXT("boss.cruz"),
        TEXT("boss.de_silva"),
        TEXT("boss.vega"),
        TEXT("boss.blackwood"),
        TEXT("boss.sterling"),
        TEXT("boss.ashcroft"),
        TEXT("boss.thorne")};
}

TMap<EM7StandingMissionType, int32> UContentScaleManifestDataAsset::GetRequiredStandingTypeCounts()
{
    return {
        {EM7StandingMissionType::Escort, 21},
        {EM7StandingMissionType::ConvoyRaid, 18},
        {EM7StandingMissionType::Recovery, 24},
        {EM7StandingMissionType::Champion, 12},
        {EM7StandingMissionType::Transport, 16},
        {EM7StandingMissionType::PrivateerCommission, 14},
        {EM7StandingMissionType::RoadWork, 15},
        {EM7StandingMissionType::Hunt, 13},
        {EM7StandingMissionType::Salvage, 14}};
}

bool UContentScaleManifestDataAsset::ValidateDialogueReadiness(TArray<FString>& OutErrors) const
{
    TSet<FName> SeenRoles;
    for (const FDialogueProductionReadinessRecord& Record : DialogueReadiness)
    {
        if (!AddUniqueStableId(Record.RoleId, SeenRoles, OutErrors, TEXT("dialogue role")))
        {
            continue;
        }
        if (Record.DialogueLockId.IsNone())
        {
            OutErrors.Add(FString::Printf(TEXT("dialogue role %s has no dialogue-lock id"), *Record.RoleId.ToString()));
        }
        if (Record.CastingRole.TrimStartAndEnd().IsEmpty())
        {
            OutErrors.Add(FString::Printf(TEXT("dialogue role %s has no casting role"), *Record.RoleId.ToString()));
        }
        if (Record.PronunciationReference.TrimStartAndEnd().IsEmpty())
        {
            OutErrors.Add(FString::Printf(TEXT("dialogue role %s has no pronunciation reference"), *Record.RoleId.ToString()));
        }
        if (Record.bSubtitleReady && !Record.bDialogueLocked)
        {
            OutErrors.Add(FString::Printf(TEXT("dialogue role %s cannot be subtitle-ready before dialogue lock"), *Record.RoleId.ToString()));
        }
    }
    return OutErrors.IsEmpty();
}

bool UContentScaleManifestDataAsset::ValidateManifest(TArray<FString>& OutErrors) const
{
    OutErrors.Reset();

    if (ManifestRevision.IsNone())
    {
        OutErrors.Add(TEXT("content manifest revision is required"));
    }

    TSet<FName> QuestIds;
    int32 ThreadCount = 0;
    int32 TurnCount = 0;
    int32 StandingCount = 0;
    TMap<EM7StandingMissionType, int32> StandingCounts;

    for (const FScaledContentEntry& Entry : QuestAndMissionEntries)
    {
        AddUniqueStableId(Entry.StableId, QuestIds, OutErrors, TEXT("quest/mission"));
        RequireSource(Entry.GoverningSource, Entry.StableId, OutErrors, TEXT("quest/mission"));

        if (Entry.bGeneratedOrRadiant)
        {
            OutErrors.Add(FString::Printf(TEXT("generated/radiant content is forbidden: %s"), *Entry.StableId.ToString()));
        }

        switch (Entry.Kind)
        {
        case EScaledContentKind::Thread:
            ++ThreadCount;
            if (Entry.StandingType != EM7StandingMissionType::None)
            {
                OutErrors.Add(FString::Printf(TEXT("Thread %s may not declare a Standing type"), *Entry.StableId.ToString()));
            }
            break;
        case EScaledContentKind::Turn:
            ++TurnCount;
            if (Entry.StandingType != EM7StandingMissionType::None)
            {
                OutErrors.Add(FString::Printf(TEXT("Turn %s may not declare a Standing type"), *Entry.StableId.ToString()));
            }
            break;
        case EScaledContentKind::StandingVariant:
            ++StandingCount;
            if (Entry.StandingType == EM7StandingMissionType::None)
            {
                OutErrors.Add(FString::Printf(TEXT("Standing variant %s must declare one of the nine authored types"), *Entry.StableId.ToString()));
            }
            else
            {
                StandingCounts.FindOrAdd(Entry.StandingType)++;
            }
            break;
        default:
            OutErrors.Add(FString::Printf(TEXT("unknown content kind for %s"), *Entry.StableId.ToString()));
            break;
        }
    }

    if (ThreadCount != RequiredThreads)
    {
        OutErrors.Add(FString::Printf(TEXT("content manifest requires exactly %d Threads; found %d"), RequiredThreads, ThreadCount));
    }
    if (TurnCount != RequiredTurns)
    {
        OutErrors.Add(FString::Printf(TEXT("content manifest requires exactly %d Turns; found %d"), RequiredTurns, TurnCount));
    }
    if (StandingCount != RequiredStandingVariants)
    {
        OutErrors.Add(FString::Printf(TEXT("content manifest requires exactly %d Standing variants; found %d"), RequiredStandingVariants, StandingCount));
    }

    const TMap<EM7StandingMissionType, int32> RequiredStandingCounts = GetRequiredStandingTypeCounts();
    for (const TPair<EM7StandingMissionType, int32>& Requirement : RequiredStandingCounts)
    {
        const int32 Actual = StandingCounts.FindRef(Requirement.Key);
        if (Actual != Requirement.Value)
        {
            OutErrors.Add(FString::Printf(TEXT("Standing type %d requires exactly %d authored variants; found %d"), static_cast<int32>(Requirement.Key), Requirement.Value, Actual));
        }
    }

    TSet<FName> DungeonIds;
    int32 MinorCount = 0;
    int32 NamedCount = 0;
    int32 CrystalCavesCount = 0;
    for (const FDungeonManifestEntry& Entry : Dungeons)
    {
        AddUniqueStableId(Entry.StableId, DungeonIds, OutErrors, TEXT("dungeon"));
        RequireSource(Entry.GoverningSource, Entry.StableId, OutErrors, TEXT("dungeon"));

        if (Entry.Tier == EContentDungeonTier::Minor)
        {
            ++MinorCount;
        }
        else
        {
            ++NamedCount;
        }
        if (Entry.Tier == EContentDungeonTier::CrystalCaves)
        {
            ++CrystalCavesCount;
        }
        if (Entry.bHasMapMarker)
        {
            OutErrors.Add(FString::Printf(TEXT("dungeon marker is forbidden: %s"), *Entry.StableId.ToString()));
        }
        if (Entry.bHasAmbientMusic)
        {
            OutErrors.Add(FString::Printf(TEXT("ambient dungeon music is forbidden: %s"), *Entry.StableId.ToString()));
        }
        if (Entry.bContainsChildRemains)
        {
            OutErrors.Add(FString::Printf(TEXT("child remains are forbidden in every dungeon: %s"), *Entry.StableId.ToString()));
        }
        if (IsTierBOrAbove(Entry.Tier) && !Entry.bReturnShortcutFromInside)
        {
            OutErrors.Add(FString::Printf(TEXT("Tier B+ dungeon must open a return shortcut from inside: %s"), *Entry.StableId.ToString()));
        }

        if (Entry.Tier == EContentDungeonTier::CrystalCaves)
        {
            if (Entry.AuthoredMaximumMinutes <= 90.0f + DungeonMinuteTolerance || Entry.AuthoredMaximumMinutes > 120.0f + DungeonMinuteTolerance)
            {
                OutErrors.Add(TEXT("Crystal Caves is the sole >90-minute carve-out and must remain within its authored 90-120 minute range"));
            }
        }
        else if (Entry.AuthoredMaximumMinutes > 90.0f + DungeonMinuteTolerance)
        {
            OutErrors.Add(FString::Printf(TEXT("only Crystal Caves may exceed 90 minutes: %s"), *Entry.StableId.ToString()));
        }
    }

    if (Dungeons.Num() != RequiredDungeonTotal || NamedCount != RequiredNamedDungeons || MinorCount != RequiredMinorDungeons)
    {
        OutErrors.Add(FString::Printf(TEXT("dungeon manifest must be 61 total = 41 named + 20 minor; found %d total = %d named + %d minor"), Dungeons.Num(), NamedCount, MinorCount));
    }
    if (CrystalCavesCount != 1)
    {
        OutErrors.Add(FString::Printf(TEXT("dungeon manifest requires exactly one Crystal Caves carve-out; found %d"), CrystalCavesCount));
    }

    const TSet<FName> CanonicalBosses = GetCanonicalTier1BossIds();
    TSet<FName> SeenBosses;
    for (const FTier1BossManifestEntry& Entry : Tier1Bosses)
    {
        AddUniqueStableId(Entry.StableId, SeenBosses, OutErrors, TEXT("Tier-1 boss"));
        RequireSource(Entry.GoverningSource, Entry.StableId, OutErrors, TEXT("Tier-1 boss"));
        if (!CanonicalBosses.Contains(Entry.StableId))
        {
            OutErrors.Add(FString::Printf(TEXT("non-canonical Tier-1 boss id: %s"), *Entry.StableId.ToString()));
        }
    }
    if (Tier1Bosses.Num() != RequiredTier1Bosses || SeenBosses.Num() != CanonicalBosses.Num())
    {
        OutErrors.Add(TEXT("Tier-1 boss manifest must contain exactly The Nine Who Hold"));
    }
    for (const FName Canonical : CanonicalBosses)
    {
        if (!SeenBosses.Contains(Canonical))
        {
            OutErrors.Add(FString::Printf(TEXT("missing canonical Tier-1 boss: %s"), *Canonical.ToString()));
        }
    }

    TSet<FName> PresentationIds;
    TSet<FName> ProtectedIds;
    TSet<FName> CutsceneIds;
    int32 CutsceneCount = 0;
    int32 ProtectedCount = 0;
    for (const FPresentationMomentManifestEntry& Entry : PresentationMoments)
    {
        AddUniqueStableId(Entry.StableId, PresentationIds, OutErrors, TEXT("presentation moment"));
        RequireSource(Entry.GoverningSource, Entry.StableId, OutErrors, TEXT("presentation moment"));
        if (Entry.bSequencerOwnedCutscene == Entry.bProtectedPlayableMoment)
        {
            OutErrors.Add(FString::Printf(TEXT("presentation moment %s must be exactly one of cutscene or protected playable"), *Entry.StableId.ToString()));
        }
        if (Entry.bSequencerOwnedCutscene)
        {
            ++CutsceneCount;
            CutsceneIds.Add(Entry.StableId);
        }
        if (Entry.bProtectedPlayableMoment)
        {
            ++ProtectedCount;
            ProtectedIds.Add(Entry.StableId);
            if (Entry.bSequencerOwnedCutscene)
            {
                OutErrors.Add(FString::Printf(TEXT("protected playable moment may never be Sequencer-owned: %s"), *Entry.StableId.ToString()));
            }
        }
    }
    if (CutsceneCount != RequiredCutscenes || ProtectedCount != RequiredProtectedPlayableMoments)
    {
        OutErrors.Add(FString::Printf(TEXT("presentation manifest requires exactly 19 cutscenes and 22 protected playable moments; found %d/%d"), CutsceneCount, ProtectedCount));
    }
    const TSet<FName>& CanonicalCutscenes = CanonicalAuthoredCutscenes();
    for (const FName Canonical : CanonicalCutscenes)
    {
        if (!CutsceneIds.Contains(Canonical))
        {
            OutErrors.Add(FString::Printf(
                TEXT("authored cutscene register is missing canonical Section-3 entry: %s"),
                *Canonical.ToString()));
        }
    }

    const TSet<FName>& CanonicalProtected = CanonicalProtectedPlayableMoments();
    if (ProtectedIds.Num() != CanonicalProtected.Num())
    {
        OutErrors.Add(TEXT("protected playable moment register must contain all canonical Section-6 moments"));
    }
    for (const FName Canonical : CanonicalProtected)
    {
        if (!ProtectedIds.Contains(Canonical))
        {
            OutErrors.Add(FString::Printf(TEXT("missing canonical protected playable moment: %s"), *Canonical.ToString()));
        }
    }

    TSet<FName> DependencyIds;
    TSet<FName> Authorities;
    for (const FContentAuthorityDependency& Dependency : AuthorityDependencies)
    {
        AddUniqueStableId(Dependency.StableId, DependencyIds, OutErrors, TEXT("authority dependency"));
        RequireSource(Dependency.GoverningSource, Dependency.StableId, OutErrors, TEXT("authority dependency"));
        if (Dependency.RequiredAuthority.IsNone())
        {
            OutErrors.Add(FString::Printf(TEXT("authority dependency %s does not name its existing state owner"), *Dependency.StableId.ToString()));
        }
        else
        {
            Authorities.Add(Dependency.RequiredAuthority);
        }
    }
    for (const FName RequiredAuthority : {
        FName(TEXT("DarkArisenWorldRulesSubsystem")),
        FName(TEXT("ColonialWarStateSubsystem")),
        FName(TEXT("ProgressionEconomyComponent")),
        FName(TEXT("PrincessQuestStateComponent")),
        FName(TEXT("RexaSettlementDirector"))})
    {
        if (!Authorities.Contains(RequiredAuthority))
        {
            OutErrors.Add(FString::Printf(TEXT("M7 integration manifest must route through existing authority: %s"), *RequiredAuthority.ToString()));
        }
    }

    ValidateDialogueReadiness(OutErrors);
    return OutErrors.IsEmpty();
}
