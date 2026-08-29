// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "ContentScale/ContentScaleManifestDataAsset.h"
#include "ContentScale/StandingMissionPoolComponent.h"
#include "Release/AlphaReleaseContracts.h"

namespace
{
void AddScaledEntries(
    UContentScaleManifestDataAsset* Manifest,
    const EScaledContentKind Kind,
    const EM7StandingMissionType StandingType,
    const int32 Count,
    const TCHAR* Prefix)
{
    for (int32 Index = 0; Index < Count; ++Index)
    {
        FScaledContentEntry Entry;
        Entry.StableId = FName(*FString::Printf(TEXT("%s.%03d"), Prefix, Index + 1));
        Entry.Kind = Kind;
        Entry.StandingType = StandingType;
        Entry.GoverningSource = TEXT("automation.synthetic-approved-source");
        Manifest->QuestAndMissionEntries.Add(Entry);
    }
}

UContentScaleManifestDataAsset* BuildStructurallyCompleteManifest()
{
    UContentScaleManifestDataAsset* Manifest = NewObject<UContentScaleManifestDataAsset>();
    Manifest->ManifestRevision = TEXT("m7.synthetic.structural-test");

    AddScaledEntries(Manifest, EScaledContentKind::Thread, EM7StandingMissionType::None, 17, TEXT("thread"));
    AddScaledEntries(Manifest, EScaledContentKind::Turn, EM7StandingMissionType::None, 132, TEXT("turn"));
    AddScaledEntries(Manifest, EScaledContentKind::StandingVariant, EM7StandingMissionType::Escort, 21, TEXT("standing.escort"));
    AddScaledEntries(Manifest, EScaledContentKind::StandingVariant, EM7StandingMissionType::ConvoyRaid, 18, TEXT("standing.convoy"));
    AddScaledEntries(Manifest, EScaledContentKind::StandingVariant, EM7StandingMissionType::Recovery, 24, TEXT("standing.recovery"));
    AddScaledEntries(Manifest, EScaledContentKind::StandingVariant, EM7StandingMissionType::Champion, 12, TEXT("standing.champion"));
    AddScaledEntries(Manifest, EScaledContentKind::StandingVariant, EM7StandingMissionType::Transport, 16, TEXT("standing.transport"));
    AddScaledEntries(Manifest, EScaledContentKind::StandingVariant, EM7StandingMissionType::PrivateerCommission, 14, TEXT("standing.privateer"));
    AddScaledEntries(Manifest, EScaledContentKind::StandingVariant, EM7StandingMissionType::RoadWork, 15, TEXT("standing.road"));
    AddScaledEntries(Manifest, EScaledContentKind::StandingVariant, EM7StandingMissionType::Hunt, 13, TEXT("standing.hunt"));
    AddScaledEntries(Manifest, EScaledContentKind::StandingVariant, EM7StandingMissionType::Salvage, 14, TEXT("standing.salvage"));

    for (int32 Index = 0; Index < 20; ++Index)
    {
        FDungeonManifestEntry Entry;
        Entry.StableId = FName(*FString::Printf(TEXT("dungeon.minor.%02d"), Index + 1));
        Entry.Tier = EContentDungeonTier::Minor;
        Entry.GoverningSource = TEXT("dungeon system global.md");
        Entry.AuthoredMaximumMinutes = 30.0f;
        Manifest->Dungeons.Add(Entry);
    }
    for (int32 Index = 0; Index < 40; ++Index)
    {
        FDungeonManifestEntry Entry;
        Entry.StableId = FName(*FString::Printf(TEXT("dungeon.named.%02d"), Index + 1));
        Entry.Tier = Index < 20 ? EContentDungeonTier::TierA : EContentDungeonTier::TierB;
        Entry.GoverningSource = TEXT("dungeon system global.md");
        Entry.bReturnShortcutFromInside = Entry.Tier == EContentDungeonTier::TierB;
        Entry.AuthoredMaximumMinutes = 45.0f;
        Manifest->Dungeons.Add(Entry);
    }
    FDungeonManifestEntry Crystal;
    Crystal.StableId = TEXT("dungeon.crystal-caves");
    Crystal.Tier = EContentDungeonTier::CrystalCaves;
    Crystal.GoverningSource = TEXT("crystal caves.md");
    Crystal.bReturnShortcutFromInside = true;
    Crystal.AuthoredMaximumMinutes = 120.0f;
    Manifest->Dungeons.Add(Crystal);

    const TArray<FName> BossIds = {
        TEXT("boss.herrera"), TEXT("boss.reyes"), TEXT("boss.cruz"), TEXT("boss.de_silva"),
        TEXT("boss.vega"), TEXT("boss.blackwood"), TEXT("boss.sterling"), TEXT("boss.ashcroft"), TEXT("boss.thorne")};
    for (const FName BossId : BossIds)
    {
        FTier1BossManifestEntry Boss;
        Boss.StableId = BossId;
        Boss.GoverningSource = TEXT("colonial war bosses.md");
        Manifest->Tier1Bosses.Add(Boss);
    }

    // The fourteen `cutscene catalog.md` Section 3 names individually. The manifest is pinned to
    // these by identity, so the fixture must use the canonical ids rather than generated ones.
    const TArray<FName> CanonicalCutsceneIds = {
        TEXT("cutscene.opening.the-brother"), TEXT("cutscene.opening.la-liberacion"),
        TEXT("cutscene.archipelago.first-letter"), TEXT("cutscene.archipelago.the-grove"),
        TEXT("cutscene.archipelago.dream-fight-entry"), TEXT("cutscene.archipelago.alejandro-strain"),
        TEXT("cutscene.highmoore.emergence"), TEXT("cutscene.highmoore.voice-from-behind"),
        TEXT("cutscene.highmoore.ejection"), TEXT("cutscene.highmoore.false-letter"),
        TEXT("cutscene.highmoore.arrow"), TEXT("cutscene.highmoore.real-letter"),
        TEXT("cutscene.highmoore.she-wrote-two"), TEXT("cutscene.highmoore.wizards-question")};
    for (const FName CutsceneId : CanonicalCutsceneIds)
    {
        FPresentationMomentManifestEntry Cutscene;
        Cutscene.StableId = CutsceneId;
        Cutscene.GoverningSource = TEXT("cutscene catalog.md Section 3");
        Cutscene.bSequencerOwnedCutscene = true;
        Manifest->PresentationMoments.Add(Cutscene);
    }
    // Entries #15-#19 are deferred to a governing document that does not exist in this repository,
    // so the fixture stands them in by count only. They are not canonical ids and must not become
    // one until `bosses/crimson_armada.md` is authored.
    for (int32 Index = 0; Index < 5; ++Index)
    {
        FPresentationMomentManifestEntry Cutscene;
        Cutscene.StableId = FName(*FString::Printf(TEXT("cutscene.endgame.unnamed-%02d"), Index + 15));
        Cutscene.GoverningSource = TEXT("cutscene catalog.md Section 3.4 (governing document absent)");
        Cutscene.bSequencerOwnedCutscene = true;
        Manifest->PresentationMoments.Add(Cutscene);
    }
    const TArray<FName> ProtectedIds = {
        TEXT("playable.arrow.after-thirty-seconds"), TEXT("playable.lake.standing"), TEXT("playable.archer.kill"),
        TEXT("playable.real-letter.reading"), TEXT("playable.cassian.kill"), TEXT("playable.belos.nine-minutes"),
        TEXT("playable.belos.walk-out"), TEXT("playable.stair.approach"), TEXT("playable.mountain.return"),
        TEXT("playable.churchyard"), TEXT("playable.boss-death.hold"), TEXT("playable.ibarra.garden"),
        TEXT("playable.guardian.settling"), TEXT("playable.light-elf.ending"), TEXT("playable.vidal.confession"),
        TEXT("playable.kettle.office"), TEXT("playable.aldric.ninety-seconds"), TEXT("playable.rowland.offer"),
        TEXT("playable.liberation.aftermath"), TEXT("playable.great-cabin.crew-scene"),
        TEXT("playable.maerwyn.breakfast-line"), TEXT("playable.stopped-shaft.first-sight")};
    for (const FName ProtectedId : ProtectedIds)
    {
        FPresentationMomentManifestEntry Moment;
        Moment.StableId = ProtectedId;
        Moment.GoverningSource = TEXT("cutscene catalog.md Section 6");
        Moment.bProtectedPlayableMoment = true;
        Manifest->PresentationMoments.Add(Moment);
    }

    const TArray<FName> Authorities = {
        TEXT("DarkArisenWorldRulesSubsystem"), TEXT("ColonialWarStateSubsystem"), TEXT("ProgressionEconomyComponent"),
        TEXT("PrincessQuestStateComponent"), TEXT("RexaSettlementDirector")};
    for (int32 Index = 0; Index < Authorities.Num(); ++Index)
    {
        FContentAuthorityDependency Dependency;
        Dependency.StableId = FName(*FString::Printf(TEXT("dependency.%d"), Index));
        Dependency.RequiredAuthority = Authorities[Index];
        Dependency.GoverningSource = TEXT("Docs/M7_M8_SOURCE_PLAN.md");
        Manifest->AuthorityDependencies.Add(Dependency);
    }
    return Manifest;
}

FAlphaReleaseManifest BuildValidReleaseManifest()
{
    const FString Commit = FString::ChrN(40, TEXT('a'));
    const FString SourceHash = FString::ChrN(64, TEXT('1'));
    const FString WindowsHash = FString::ChrN(64, TEXT('2'));
    const FString LinuxHash = FString::ChrN(64, TEXT('3'));

    FAlphaReleaseManifest Manifest;
    Manifest.SourceCommit = Commit;
    Manifest.ContentManifestRevision = TEXT("m8.synthetic.content");
    Manifest.SourceArchiveSha256 = SourceHash;

    FAlphaCandidateDescriptor Windows;
    Windows.Platform = EAlphaTargetPlatform::Win64;
    Windows.SourceCommit = Commit;
    Windows.ContentManifestRevision = Manifest.ContentManifestRevision;
    Windows.Sha256 = WindowsHash;
    Windows.bShipping = true;
    Manifest.Candidates.Add(Windows);

    FAlphaCandidateDescriptor Linux;
    Linux.Platform = EAlphaTargetPlatform::LinuxX64;
    Linux.SourceCommit = Commit;
    Linux.ContentManifestRevision = Manifest.ContentManifestRevision;
    Linux.Sha256 = LinuxHash;
    Linux.bShipping = true;
    Manifest.Candidates.Add(Linux);
    return Manifest;
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenM7ContentManifestSpec,
    "DarkArisen.M7.ContentManifestAndProtectedMoments",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenM7ContentManifestSpec::RunTest(const FString& Parameters)
{
    UContentScaleManifestDataAsset* Manifest = BuildStructurallyCompleteManifest();
    TestNotNull(TEXT("Synthetic M7 manifest constructs"), Manifest);
    if (!Manifest) return false;

    TArray<FString> Errors;
    TestTrue(TEXT("Exact 61/17/132/147/9/19/22 structural manifest passes"), Manifest->ValidateManifest(Errors));
    TestEqual(TEXT("Synthetic manifest has no structural errors"), Errors.Num(), 0);

    Manifest->Dungeons[0].bHasMapMarker = true;
    TestFalse(TEXT("Any dungeon marker fails closed"), Manifest->ValidateManifest(Errors));
    Manifest->Dungeons[0].bHasMapMarker = false;

    FPresentationMomentManifestEntry& ArrowAftermath = Manifest->PresentationMoments[19];
    ArrowAftermath.bSequencerOwnedCutscene = true;
    TestFalse(TEXT("A protected playable moment cannot become a cutscene"), Manifest->ValidateManifest(Errors));
    ArrowAftermath.bSequencerOwnedCutscene = false;

    // The count alone used to be the whole check, so nineteen arbitrary ids passed. The authored
    // fourteen are now pinned by identity; renaming one keeps the count at nineteen and must
    // still fail.
    FPresentationMomentManifestEntry& NamedCutscene = Manifest->PresentationMoments[0];
    const FName CanonicalId = NamedCutscene.StableId;
    NamedCutscene.StableId = TEXT("cutscene.substituted-for-a-canonical-entry");
    TestFalse(
        TEXT("A canonical Section-3 cutscene cannot be substituted while the count still reads nineteen"),
        Manifest->ValidateManifest(Errors));
    NamedCutscene.StableId = CanonicalId;
    TestTrue(TEXT("Restoring the canonical cutscene id passes again"), Manifest->ValidateManifest(Errors));

    Manifest->QuestAndMissionEntries[0].bGeneratedOrRadiant = true;
    TestFalse(TEXT("Radiant/generated M7 content fails closed"), Manifest->ValidateManifest(Errors));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenM7StandingFinitePoolSpec,
    "DarkArisen.M7.FiniteStandingPool",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenM7StandingFinitePoolSpec::RunTest(const FString& Parameters)
{
    UStandingMissionPoolComponent* Pool = NewObject<UStandingMissionPoolComponent>();
    TestNotNull(TEXT("Standing pool constructs"), Pool);
    if (!Pool) return false;

    Pool->SameRegionTypePayMultipliers = {1.0f, 0.90f, 0.80f, 0.70f, 0.65f, 0.60f};
    for (int32 Index = 0; Index < 6; ++Index)
    {
        FStandingMissionVariantDefinition Variant;
        Variant.StableId = FName(*FString::Printf(TEXT("standing.test.%d"), Index + 1));
        Variant.RegionId = TEXT("region.test");
        Variant.Type = EM7StandingMissionType::Escort;
        Variant.FullPay = 1000;
        Variant.StandingGain = 7;
        Variant.GoverningSource = TEXT("automation synthetic finite-pool fixture");
        Pool->AuthoredVariants.Add(Variant);
    }

    TestFalse(TEXT("Six fixtures are intentionally not mistaken for the complete 147-variant production pool"), [&]()
    {
        TArray<FString> Errors;
        return Pool->ValidateAuthoredPool(Errors);
    }());

    for (int32 Index = 0; Index < 6; ++Index)
    {
        const FStandingMissionVariantDefinition* Next = Pool->FindNextAvailableVariant(TEXT("region.test"), EM7StandingMissionType::Escort);
        TestNotNull(TEXT("Finite authored variant remains available before exhaustion"), Next);
        if (!Next) return false;
        FStandingMissionCompletionResult Result;
        TestTrue(TEXT("Authored Standing variant completes once"), Pool->CompleteVariant(Next->StableId, Result));
        TestEqual(TEXT("Standing gain never degrades"), Result.StandingGain, 7);
        if (Index == 0)
        {
            TestTrue(TEXT("First run pays 100 percent"), FMath::IsNearlyEqual(Result.AppliedPayMultiplier, 1.0f));
        }
        if (Index == 5)
        {
            TestTrue(TEXT("Sixth run pays exactly 60 percent"), FMath::IsNearlyEqual(Result.AppliedPayMultiplier, 0.60f));
            TestEqual(TEXT("Sixth 1000-pay fixture yields 600"), Result.PaidAmount, 600);
        }
    }
    TestTrue(TEXT("Region/type pool is exhausted when authored variants run out"), Pool->IsPoolExhausted(TEXT("region.test"), EM7StandingMissionType::Escort));
    TestNull(TEXT("No radiant replacement is synthesized after exhaustion"), Pool->FindNextAvailableVariant(TEXT("region.test"), EM7StandingMissionType::Escort));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenM8AccessibilityAndSaveSpec,
    "DarkArisen.M8.AccessibilityAndSaveMigration",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenM8AccessibilityAndSaveSpec::RunTest(const FString& Parameters)
{
    FAccessibilityDifficultyContract Accessibility;
    Accessibility.IncomingDamageMultiplier = 0.5f;
    Accessibility.PlayerHealthMultiplier = 2.0f;
    TArray<FString> Errors;
    TestTrue(TEXT("Damage/health difficulty modifiers preserve valid accessibility contract"), Accessibility.Validate(Errors));
    Accessibility.DeflectionWindowFrames = 5;
    TestFalse(TEXT("Difficulty cannot change six-frame deflection"), Accessibility.Validate(Errors));

    TArray<FSaveMigrationStep> NoHistoricalMigrations;
    TArray<FSaveMigrationStep> Path;
    FString Error;
    TestTrue(TEXT("Current v1 save loads without fabricated historical migration"),
        FSaveSchemaMigrationRegistry::TryBuildMigrationPath(1, NoHistoricalMigrations, Path, Error));
    TestEqual(TEXT("Current save needs zero migration steps"), Path.Num(), 0);
    TestFalse(TEXT("Future save version fails closed"),
        FSaveSchemaMigrationRegistry::TryBuildMigrationPath(2, NoHistoricalMigrations, Path, Error));
    TestFalse(TEXT("Unknown pre-v1 save fails closed"),
        FSaveSchemaMigrationRegistry::TryBuildMigrationPath(0, NoHistoricalMigrations, Path, Error));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenM8ReleaseParitySpec,
    "DarkArisen.M8.ReleaseParityPrivacyAndAcceptance",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenM8ReleaseParitySpec::RunTest(const FString& Parameters)
{
    FAlphaReleaseManifest Manifest = BuildValidReleaseManifest();
    TArray<FString> Errors;
    TestTrue(TEXT("Same-commit Win64/Linux Shipping manifest passes"), Manifest.Validate(Errors));

    Manifest.Candidates[1].SourceCommit = FString::ChrN(40, TEXT('b'));
    TestFalse(TEXT("Different platform commits fail parity"), Manifest.Validate(Errors));
    Manifest.Candidates[1].SourceCommit = Manifest.SourceCommit;

    Manifest.Candidates[0].bContainsTurnSecret = true;
    TestFalse(TEXT("TURN secret in candidate manifest fails privacy gate"), Manifest.Validate(Errors));
    Manifest.Candidates[0].bContainsTurnSecret = false;

    FAlphaAcceptanceLedger Ledger;
    Ledger.CandidateCommit = Manifest.SourceCommit;
    Ledger.WindowsCandidateSha256 = Manifest.Candidates[0].Sha256;
    Ledger.LinuxCandidateSha256 = Manifest.Candidates[1].Sha256;
    TestFalse(TEXT("Source cannot self-certify an Alpha before observed gates exist"), Ledger.IsAcceptedForPromotion(Manifest, Errors));

    Ledger.bFullContentM0M8 = true;
    Ledger.bWindowsUe55BuildAndAutomation = true;
    Ledger.bLinuxUe55BuildAndAutomation = true;
    Ledger.bWindowsStartToCreditsPlaythrough = true;
    Ledger.bLinuxStartToCreditsPlaythrough = true;
    Ledger.bContentManifestReconciled = true;
    Ledger.bPerformanceFloor60Fps = true;
    Ledger.bPrivacyAccessDenied = true;
    Ledger.bCreditsMusicAndLicencesApproved = true;
    Ledger.bFloAlphaApproval = true;
    TestTrue(TEXT("Exact hashes plus all supplied acceptance gates permit promotion"), Ledger.IsAcceptedForPromotion(Manifest, Errors));

    FCreditsReadinessManifest Credits;
    TestFalse(TEXT("Credits are blocked without names, attribution and music rights"), Credits.IsProductionReady());
    Credits.bNamesAndOrderApproved = true;
    Credits.bAttributionComplete = true;
    Credits.MusicSource = TEXT("approved-source-reference");
    Credits.MusicLicenceReference = TEXT("approved-licence-reference");
    Credits.bMusicSourceApproved = true;
    Credits.bMusicLicenceApproved = true;
    Credits.bMusicCostApproved = true;
    Credits.bMusicNonReactive = true;
    TestTrue(TEXT("Credits become production-ready only after all explicit approvals"), Credits.IsProductionReady());
    return true;
}

#endif
