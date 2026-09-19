#include "DarkArisenOps.h"

#include "Dom/JsonObject.h"
#include "HAL/FileManager.h"
#include "Misc/Paths.h"

namespace DarkArisenOps
{
namespace
{
void RequireFile(const FString& Root, const FString& Relative, TArray<FString>& Errors)
{
    if (!FPaths::FileExists(FPaths::Combine(Root, Relative)))
    {
        Errors.Add(FString::Printf(TEXT("missing required file: %s"), *Relative));
    }
}

void RequireFragments(
    const FString& Root,
    const FString& Relative,
    std::initializer_list<const TCHAR*> Fragments,
    TArray<FString>& Errors)
{
    FString Text;
    const FString Path = FPaths::Combine(Root, Relative);
    if (!ReadText(Path, Text))
    {
        Errors.Add(FString::Printf(TEXT("cannot read required file: %s"), *Relative));
        return;
    }
    for (const TCHAR* Fragment : Fragments)
    {
        if (!Text.Contains(Fragment, ESearchCase::CaseSensitive))
        {
            Errors.Add(FString::Printf(TEXT("%s requirement missing: %s"), *Relative, Fragment));
        }
    }
}

void ForbidFragments(
    const FString& Root,
    const FString& Relative,
    std::initializer_list<const TCHAR*> Fragments,
    TArray<FString>& Errors)
{
    FString Text;
    const FString Path = FPaths::Combine(Root, Relative);
    if (!ReadText(Path, Text))
    {
        return;
    }
    for (const TCHAR* Fragment : Fragments)
    {
        if (Text.Contains(Fragment, ESearchCase::IgnoreCase))
        {
            Errors.Add(FString::Printf(TEXT("%s contains forbidden fragment: %s"), *Relative, Fragment));
        }
    }
}

void ValidateLanguagePolicy(const FString& Root, TArray<FString>& Errors)
{
    static const TSet<FString> ForbiddenExtensions = {
        TEXT(".py"), TEXT(".pyw"), TEXT(".ps1"), TEXT(".sh"), TEXT(".bash"),
        TEXT(".ts"), TEXT(".tsx"), TEXT(".js"), TEXT(".jsx"), TEXT(".cmd"), TEXT(".bat")};

    const TArray<FString> ScanRelatives = {TEXT("Source"), TEXT("Tools")};
    for (const FString& ScanRelative : ScanRelatives)
    {
        const FString ScanRoot = FPaths::Combine(Root, ScanRelative);
        TArray<FString> Files;
        IFileManager::Get().FindFilesRecursive(Files, *ScanRoot, TEXT("*"), true, false, false);
        for (const FString& File : Files)
        {
            const FString Extension = TEXT(".") + FPaths::GetExtension(File, false).ToLower();
            if (ForbiddenExtensions.Contains(Extension))
            {
                Errors.Add(FString::Printf(TEXT("non-C++ executable source is forbidden: %s"), *File));
            }
            if (Extension == TEXT(".cs"))
            {
                const bool bAllowedUbtMetadata = File.EndsWith(TEXT(".Target.cs"), ESearchCase::IgnoreCase)
                    || File.EndsWith(TEXT(".Build.cs"), ESearchCase::IgnoreCase);
                if (!bAllowedUbtMetadata)
                {
                    Errors.Add(FString::Printf(TEXT("C# is allowed only for Unreal Build Tool metadata: %s"), *File));
                }
            }
        }
    }
}

void ValidateProjectContract(const FString& Root, TArray<FString>& Errors)
{
    const TArray<FString> Required = {
        TEXT("DarkArisen.uproject"),
        TEXT("Config/DefaultEngine.ini"),
        TEXT("Config/DefaultGame.ini"),
        TEXT("Config/DefaultInput.ini"),
        TEXT("Source/DarkArisen.Target.cs"),
        TEXT("Source/DarkArisenEditor.Target.cs"),
        TEXT("Source/DarkArisen/DarkArisen.Build.cs"),
        TEXT("Source/DarkArisen/DarkArisen.cpp"),
        TEXT("Source/DarkArisen/DesignLaws.h"),
        TEXT("Source/DarkArisen/CoreLoopTuning.h"),
        TEXT("Source/DarkArisen/Tests/DesignLawsSpec.cpp"),
        TEXT("Source/DarkArisen/Tests/M3M4SystemsSpec.cpp"),
        TEXT("Source/DarkArisen/Tests/M3M4CompletionSpec.cpp"),
        TEXT("Source/DarkArisen/Tests/M5M6SystemsSpec.cpp"),
        TEXT("Source/DarkArisen/Tests/M7M8SystemsSpec.cpp"),
        TEXT("Source/DarkArisen/ContentScale/ContentScaleManifestDataAsset.h"),
        TEXT("Source/DarkArisen/ContentScale/ContentScaleManifestDataAsset.cpp"),
        TEXT("Source/DarkArisen/ContentScale/StandingMissionPoolComponent.h"),
        TEXT("Source/DarkArisen/ContentScale/StandingMissionPoolComponent.cpp"),
        TEXT("Source/DarkArisen/Release/AlphaReleaseContracts.h"),
        TEXT("Source/DarkArisen/Release/AlphaReleaseContracts.cpp"),
        TEXT("Source/DarkArisenOps.Target.cs"),
        TEXT("Source/DarkArisenOps/DarkArisenOps.Build.cs"),
        TEXT("Source/DarkArisenOps/Private/DarkArisenOps.cpp"),
        TEXT("Source/DarkArisenOps/Private/ValidationOps.cpp"),
        TEXT("Source/DarkArisenOps/Private/BuildOps.cpp"),
        TEXT("Source/DarkArisenOps/Private/StreamingOps.cpp"),
        TEXT("Docs/M7_M8_SOURCE_PLAN.md"),
        TEXT("Docs/M7_TIER1_BOSS_REGISTER.md"),
        TEXT("Docs/NATIVE_CPP_POLICY.md"),
        TEXT(".github/workflows/ci.yml"),
        TEXT(".github/workflows/deploy-streaming.yml")};
    for (const FString& Relative : Required)
    {
        RequireFile(Root, Relative, Errors);
    }

    TSharedPtr<FJsonObject> Project;
    if (LoadJsonObject(FPaths::Combine(Root, TEXT("DarkArisen.uproject")), Project))
    {
        if (Project->GetStringField(TEXT("EngineAssociation")) != TEXT("5.8"))
        {
            Errors.Add(TEXT("DarkArisen.uproject must remain pinned to UE 5.8"));
        }
        bool bFoundRuntime = false;
        for (const TSharedPtr<FJsonValue>& Value : Project->GetArrayField(TEXT("Modules")))
        {
            const TSharedPtr<FJsonObject> Module = Value->AsObject();
            bFoundRuntime |= Module.IsValid() && Module->GetStringField(TEXT("Name")) == TEXT("DarkArisen");
        }
        if (!bFoundRuntime)
        {
            Errors.Add(TEXT("DarkArisen runtime module is not declared"));
        }
        bool bPixelStreaming = false;
        for (const TSharedPtr<FJsonValue>& Value : Project->GetArrayField(TEXT("Plugins")))
        {
            const TSharedPtr<FJsonObject> Plugin = Value->AsObject();
            if (Plugin.IsValid() && Plugin->GetStringField(TEXT("Name")) == TEXT("PixelStreaming2"))
            {
                bPixelStreaming = Plugin->GetBoolField(TEXT("Enabled"));
            }
        }
        if (!bPixelStreaming)
        {
            Errors.Add(TEXT("PixelStreaming2 must remain enabled"));
        }
    }

    RequireFragments(Root, TEXT(".gitattributes"), {
        TEXT("*.uasset filter=lfs diff=lfs merge=lfs -text"),
        TEXT("*.umap filter=lfs diff=lfs merge=lfs -text"),
        TEXT("*.fbx filter=lfs diff=lfs merge=lfs -text"),
        TEXT("*.wav filter=lfs diff=lfs merge=lfs -text")}, Errors);

    RequireFragments(Root, TEXT("Config/DefaultEngine.ini"), {
        TEXT("t.MaxFPS=60"),
        TEXT("GlobalDefaultGameMode=/Script/DarkArisen.GreyboxGameMode"),
        TEXT("r.Lumen.HardwareRayTracing=False")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/DesignLaws.h"), {
        TEXT("TargetFramesPerSecond = 60"),
        TEXT("DeflectionWindowFrames = 6"),
        TEXT("RacheTimeScale = 0.30f"),
        TEXT("RacheMaximumRealSeconds = 5.0f"),
        TEXT("AuthoredCutsceneCount = 19"),
        TEXT("ProhibitedCutsceneMomentCount = 22")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/CoreLoopTuning.h"), {
        TEXT("InteractionPromptSeconds = 4.0f"),
        TEXT("DefaultTakeSeconds = 0.6f"),
        TEXT("PostureBreakVulnerabilitySeconds = 1.4f"),
        TEXT("MediumStartupFrames = 13"),
        TEXT("GreatRecoveryFrames = 44")}, Errors);
}

void ValidateGameplayContracts(const FString& Root, TArray<FString>& Errors)
{
    RequireFragments(Root, TEXT("Source/DarkArisen/Components/CombatComponent.cpp"), {
        TEXT("DesignLaws::DeflectionWindowSeconds"),
        TEXT("ResolveQueuedMeleeHitFromAnimation"),
        TEXT("RouteQueuedMeleeHitToAnimationNotify"),
        TEXT("SweepMultiByChannel")}, Errors);
    RequireFragments(Root, TEXT("Source/DarkArisen/Components/HealthComponent.cpp"), {
        TEXT("ERallyDamageClass::Poison: return 0.0f"),
        TEXT("ERallyRecoveryAction::Critical: return 1.0f")}, Errors);
    RequireFragments(Root, TEXT("Source/DarkArisen/JakeCharacter.cpp"), {
        TEXT("CombatComponent->StartRache()"),
        TEXT("TryPlayActionMontage(LightAttackMontage)"),
        TEXT("CreateDefaultSubobject<UQuestJournalComponent>"),
        TEXT("CreateDefaultSubobject<UHeatExposureComponent>"),
        TEXT("CreateDefaultSubobject<UWaterBreathComponent>")}, Errors);
    RequireFragments(Root, TEXT("Source/DarkArisen/Components/QuestJournalComponent.h"), {
        TEXT("bSilentAtActivation"),
        TEXT("bRequiresSpokenAgreement"),
        TEXT("MutuallyExclusiveQuestIds"),
        TEXT("SearchJournal")}, Errors);
    RequireFragments(Root, TEXT("Source/DarkArisen/Rexa/RexaSettlementRoster.h"), {
        TEXT("RequiredResidentCount = 40"),
        TEXT("RequiredIndigenousCount = 16"),
        TEXT("RequiredMixedCount = 12"),
        TEXT("RequiredImperialCount = 8"),
        TEXT("RequiredSailorTraderCount = 4")}, Errors);
    RequireFragments(Root, TEXT("Source/DarkArisen/Ship/ShipVoyageComponent.h"), {
        TEXT("ActiveHands = 40"),
        TEXT("GetRequiredDeckCount() { return 4; }"),
        TEXT("no water fast-travel API exists here")}, Errors);
    RequireFragments(Root, TEXT("Source/DarkArisen/Systems/ProgressionEconomyComponent.h"), {
        TEXT("RequiredSkillNodeCount = 68"),
        TEXT("RequiredTeacherCount = 23"),
        TEXT("RequiredStandingGatedNodeCount = 11"),
        TEXT("MaximumAvailableMarks = 94"),
        TEXT("FullTreeMarkCost = 141")}, Errors);
    RequireFragments(Root, TEXT("Source/DarkArisen/World/DarkArisenWorldRulesSubsystem.h"), {
        TEXT("RealSecondsPerGameHour = 150.0f"),
        TEXT("CanManualSave() const { return true; }")}, Errors);
    RequireFragments(Root, TEXT("Source/DarkArisen/ColonialWar/ColonialWarStateSubsystem.h"), {
        TEXT("UColonialWarStateSubsystem")}, Errors);
    RequireFragments(Root, TEXT("Source/DarkArisen/Highmoore/HighmooreHorseComponent.h"), {
        TEXT("UHighmooreHorseComponent")}, Errors);
    RequireFragments(Root, TEXT("Source/DarkArisen/Highmoore/PrincessQuestStateComponent.h"), {
        TEXT("UPrincessQuestStateComponent")}, Errors);

    TArray<FString> SourceFiles;
    IFileManager::Get().FindFilesRecursive(SourceFiles, *FPaths::Combine(Root, TEXT("Source/DarkArisen")), TEXT("*.cpp"), true, false, false);
    IFileManager::Get().FindFilesRecursive(SourceFiles, *FPaths::Combine(Root, TEXT("Source/DarkArisen")), TEXT("*.h"), true, false, false);
    FString SourceText;
    for (const FString& File : SourceFiles)
    {
        FString Text;
        if (ReadText(File, Text))
        {
            SourceText += TEXT("\n") + Text;
        }
    }
    const TArray<FString> Forbidden = {
        TEXT("CancelIntoDodge"), TEXT("CancelIntoBlock"), TEXT("HighlightNearbyLoot"), TEXT("TakeAllLoot"),
        TEXT("FastTravel"), TEXT("TeleportToWaypoint"), TEXT("CompassWidget"),
        TEXT("ResetSkillTree"), TEXT("RefundMarks"), TEXT("AffinityMeter"), TEXT("RelationshipMeter"),
        TEXT("AutosaveBeforeDecision"), TEXT("AutosaveBeforeBoss"), TEXT("GenerateRadiantQuest")};
    for (const FString& Token : Forbidden)
    {
        if (SourceText.Contains(Token, ESearchCase::CaseSensitive))
        {
            Errors.Add(FString::Printf(TEXT("forbidden gameplay API present: %s"), *Token));
        }
    }
}

void ValidateM7M8Contracts(const FString& Root, TArray<FString>& Errors)
{
    RequireFragments(Root, TEXT("Source/DarkArisen/ContentScale/ContentScaleManifestDataAsset.h"), {
        TEXT("RequiredNamedDungeons = 41"),
        TEXT("RequiredMinorDungeons = 20"),
        TEXT("RequiredDungeonTotal = 61"),
        TEXT("RequiredThreads = 17"),
        TEXT("RequiredTurns = 132"),
        TEXT("RequiredStandingVariants = 147"),
        TEXT("RequiredTier1Bosses = 9"),
        TEXT("RequiredCutscenes = 19"),
        TEXT("RequiredProtectedPlayableMoments = 22"),
        TEXT("bGeneratedOrRadiant"),
        TEXT("bProtectedPlayableMoment"),
        TEXT("FCreditsReadinessManifest")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/ContentScale/ContentScaleManifestDataAsset.cpp"), {
        TEXT("boss.herrera"), TEXT("boss.reyes"), TEXT("boss.cruz"), TEXT("boss.de_silva"),
        TEXT("boss.vega"), TEXT("boss.blackwood"), TEXT("boss.sterling"), TEXT("boss.ashcroft"), TEXT("boss.thorne"),
        TEXT("EM7StandingMissionType::Escort, 21"),
        TEXT("EM7StandingMissionType::ConvoyRaid, 18"),
        TEXT("EM7StandingMissionType::Recovery, 24"),
        TEXT("EM7StandingMissionType::Champion, 12"),
        TEXT("EM7StandingMissionType::Transport, 16"),
        TEXT("EM7StandingMissionType::PrivateerCommission, 14"),
        TEXT("EM7StandingMissionType::RoadWork, 15"),
        TEXT("EM7StandingMissionType::Hunt, 13"),
        TEXT("EM7StandingMissionType::Salvage, 14"),
        TEXT("generated/radiant content is forbidden"),
        TEXT("only Crystal Caves may exceed 90 minutes"),
        TEXT("protected playable moment may never be Sequencer-owned")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/ContentScale/StandingMissionPoolComponent.h"), {
        TEXT("TArray<FStandingMissionVariantDefinition> AuthoredVariants"),
        TEXT("TArray<float> SameRegionTypePayMultipliers"),
        TEXT("TSet<FName> CompletedVariantIds"),
        TEXT("FindNextAvailableVariant"),
        TEXT("IsPoolExhausted")}, Errors);
    RequireFragments(Root, TEXT("Source/DarkArisen/ContentScale/StandingMissionPoolComponent.cpp"), {
        TEXT("SameRegionTypePayMultipliers.Num() != 6"),
        TEXT("SameRegionTypePayMultipliers[0], 1.0f"),
        TEXT("SameRegionTypePayMultipliers[5], 0.60f"),
        TEXT("Standing gain never degrades"),
        TEXT("CompletedVariantIds.Contains")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Release/AlphaReleaseContracts.h"), {
        TEXT("Win64"), TEXT("LinuxX64"),
        TEXT("bInputRemappingEnabled = true"),
        TEXT("DeflectionWindowFrames = DarkArisen::DesignLaws::DeflectionWindowFrames"),
        TEXT("CurrentSchemaVersion = 1"),
        TEXT("bContainsDeploymentCredentials"),
        TEXT("bContainsTurnSecret"),
        TEXT("bContainsPrivateHostname"),
        TEXT("bContainsPublicLinkConfiguration"),
        TEXT("bContainsSonyOrNativePs5Material"),
        TEXT("FAlphaAcceptanceLedger")}, Errors);
    RequireFragments(Root, TEXT("Source/DarkArisen/Release/AlphaReleaseContracts.cpp"), {
        TEXT("difficulty/accessibility may never change the six-frame deflection window"),
        TEXT("future save schema"),
        TEXT("Alpha release requires exactly two candidates"),
        TEXT("Alpha release platform set must be exactly Win64 + Linux x86_64"),
        TEXT("acceptance ledger Windows hash does not match the exact candidate"),
        TEXT("credits_music_and_licences_approved"),
        TEXT("flo_alpha_approval")}, Errors);

    RequireFragments(Root, TEXT("Source/DarkArisen/Tests/M7M8SystemsSpec.cpp"), {
        TEXT("DarkArisen.M7.ContentManifestAndProtectedMoments"),
        TEXT("Any dungeon marker fails closed"),
        TEXT("A protected playable moment cannot become a cutscene"),
        TEXT("Radiant/generated M7 content fails closed"),
        TEXT("DarkArisen.M7.FiniteStandingPool"),
        TEXT("Sixth run pays exactly 60 percent"),
        TEXT("No radiant replacement is synthesized after exhaustion"),
        TEXT("DarkArisen.M8.AccessibilityAndSaveMigration"),
        TEXT("Difficulty cannot change six-frame deflection"),
        TEXT("Future save version fails closed"),
        TEXT("DarkArisen.M8.ReleaseParityPrivacyAndAcceptance"),
        TEXT("Different platform commits fail parity"),
        TEXT("TURN secret in candidate manifest fails privacy gate")}, Errors);
}

void ValidateInfrastructureContract(const FString& Root, TArray<FString>& Errors)
{
    FString Workflows;
    const TArray<FString> WorkflowPaths = {
        TEXT(".github/workflows/ci.yml"),
        TEXT(".github/workflows/deploy-streaming.yml")};
    for (const FString& Relative : WorkflowPaths)
    {
        FString Text;
        ReadText(FPaths::Combine(Root, Relative), Text);
        Workflows += TEXT("\n") + Text;
    }
    if (!Workflows.Contains(TEXT("self-hosted")))
    {
        Errors.Add(TEXT("workflows must remain self-hosted"));
    }
    const TArray<FString> HostedLabels = {TEXT("ubuntu-latest"), TEXT("windows-latest"), TEXT("macos-latest")};
    for (const FString& Hosted : HostedLabels)
    {
        if (Workflows.Contains(Hosted, ESearchCase::IgnoreCase))
        {
            Errors.Add(FString::Printf(TEXT("GitHub-hosted runner label is forbidden: %s"), *Hosted));
        }
    }
    if (Workflows.Contains(TEXT("actions/upload-artifact"), ESearchCase::IgnoreCase))
    {
        Errors.Add(TEXT("private game packages must never be uploaded as GitHub Actions artifacts"));
    }
    if (!Workflows.Contains(TEXT("DarkArisenOps")))
    {
        Errors.Add(TEXT("CI/deployment must route executable project operations through DarkArisenOps C++"));
    }
    for (const FString& ForbiddenRuntime : {TEXT("python "), TEXT("python3 "), TEXT(".ps1"), TEXT(".sh --"), TEXT("node ")})
    {
        if (Workflows.Contains(ForbiddenRuntime, ESearchCase::IgnoreCase))
        {
            Errors.Add(FString::Printf(TEXT("workflow reintroduces a non-C++ project runtime: %s"), *ForbiddenRuntime));
        }
    }

    RequireFragments(Root, TEXT("Tools/streaming/aws/private-single-player.yaml"), {
        TEXT("I_ACKNOWLEDGE_AWS_CHARGES"),
        TEXT("HttpTokens: required"),
        TEXT("DeletionPolicy: Retain"),
        TEXT("Encrypted: true"),
        TEXT("NO_PUBLIC_GAME_RDP_SSH_WINRM_HTTPS_OR_TURN_INGRESS")}, Errors);
    RequireFragments(Root, TEXT("Tools/streaming/aws/tailnet-policy.example.hujson"), {
        TEXT("tag:dark-arisen"),
        TEXT("tcp:443"),
        TEXT("udp:49160-49200")}, Errors);
    ForbidFragments(Root, TEXT("Tools/streaming/aws/private-single-player.yaml"), {
        TEXT("AWS::EC2::KeyPair"), TEXT("KeyName:"), TEXT("MasterUserPassword")}, Errors);
}
}

int32 ValidateCommand(const FParsedArgs& Args)
{
    const FString Root = RepoRoot(Args);
    TArray<FString> Errors;
    ValidateLanguagePolicy(Root, Errors);
    ValidateProjectContract(Root, Errors);
    ValidateGameplayContracts(Root, Errors);
    ValidateM7M8Contracts(Root, Errors);
    ValidateInfrastructureContract(Root, Errors);

    if (!Errors.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("Native C++ repository validation failed with %d error(s):"), Errors.Num());
        for (const FString& Error : Errors)
        {
            UE_LOG(LogTemp, Error, TEXT("- %s"), *Error);
        }
        return 1;
    }
    UE_LOG(LogTemp, Display, TEXT("Native C++ repository validation passed for M0-M8 source contracts."));
    return 0;
}
}
