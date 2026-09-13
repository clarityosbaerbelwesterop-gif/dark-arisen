#include "DarkArisenMaterializeAlphaCommandlet.h"

#include "AlphaGameplayGameMode.h"
#include "AlphaStartupGameMode.h"
#include "AssetImportTask.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "Animation/AnimationAsset.h"
#include "Animation/Skeleton.h"
#include "Components/BoxComponent.h"
#include "Dom/JsonObject.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Factories/WorldFactory.h"
#include "FileHelpers.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/WorldSettings.h"
#include "HAL/FileManager.h"
#include "IAssetTools.h"
#include "Misc/EngineVersion.h"
#include "Misc/FileHelper.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "Opening/LaLiberacionHelmInteractableActor.h"
#include "Opening/OpeningBoardingEncounterActor.h"
#include "Opening/OpeningCrewRecruitmentActor.h"
#include "Opening/OpeningFamilyInteractableActor.h"
#include "Opening/OpeningWaterCurrentVolume.h"
#include "Rexa/RexaSettlementAnchor.h"
#include "Rexa/RexaSettlementDirector.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Ship/LaLiberacionShip.h"
#include "Story/DarkArisenWorldDirector.h"
#include "UObject/Package.h"

namespace DarkArisenAlphaMaterialize
{
struct FCharacterAssets
{
    USkeletalMesh* Mesh = nullptr;
    USkeleton* Skeleton = nullptr;
    TArray<UAnimationAsset*> Animations;
};

static const TArray<FString> JakeRequired = {
    TEXT("Idle"), TEXT("Walk"), TEXT("Run"), TEXT("Sprint"), TEXT("TurnLeft"), TEXT("TurnRight"), TEXT("Stop"),
    TEXT("CombatIdle"), TEXT("CombatForward"), TEXT("CombatBack"), TEXT("CombatStrafeLeft"), TEXT("CombatStrafeRight"),
    TEXT("LightAttack1"), TEXT("LightAttack2"), TEXT("HeavyAttack"), TEXT("Deflect"), TEXT("DodgeForward"),
    TEXT("DodgeLeft"), TEXT("DodgeRight"), TEXT("Backstep"), TEXT("HitReactionFront"), TEXT("Stagger"), TEXT("Death"),
    TEXT("Swim"), TEXT("StrugglingSwim"), TEXT("WaterExit"), TEXT("BeachCrawl"), TEXT("BeachRecover"),
    TEXT("InteractReach"), TEXT("Pickup"), TEXT("HelmIdle")};

static const TArray<FString> BoarderRequired = {
    TEXT("Idle"), TEXT("Walk"), TEXT("Run"), TEXT("CombatIdle"), TEXT("LightAttack1"), TEXT("LightAttack2"),
    TEXT("HeavyAttack"), TEXT("HitReactionFront"), TEXT("Stagger"), TEXT("Death")};

bool ReadJson(const FString& Relative, TSharedPtr<FJsonObject>& Out)
{
    FString Text;
    const FString Path = FPaths::Combine(FPaths::ProjectDir(), Relative);
    if (!FFileHelper::LoadFileToString(Text, *Path))
    {
        UE_LOG(LogTemp, Error, TEXT("Materializer cannot read %s"), *Path);
        return false;
    }
    return FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Text), Out) && Out.IsValid();
}

FVector ReadAnchor(const FString& Relative, const FString& Key, const FVector& Fallback = FVector::ZeroVector)
{
    TSharedPtr<FJsonObject> Root;
    if (!ReadJson(Relative, Root)) return Fallback;
    const TSharedPtr<FJsonObject>* Anchors = nullptr;
    if (!Root->TryGetObjectField(TEXT("anchors"), Anchors) || !Anchors || !Anchors->IsValid()) return Fallback;
    const TArray<TSharedPtr<FJsonValue>>* Values = nullptr;
    if (!(*Anchors)->TryGetArrayField(Key, Values) || !Values || Values->Num() < 3) return Fallback;
    return FVector((*Values)[0]->AsNumber(), (*Values)[1]->AsNumber(), (*Values)[2]->AsNumber());
}

FName StableName(const FString& Prefix, const FString& Suffix)
{
    return FName(*(Prefix + Suffix));
}

TArray<UObject*> ImportSource(IAssetTools& AssetTools, const FString& Relative, const FString& Destination)
{
    const FString Source = FPaths::Combine(FPaths::ProjectDir(), Relative);
    if (!IFileManager::Get().FileExists(*Source))
    {
        UE_LOG(LogTemp, Error, TEXT("Materializer source missing: %s"), *Source);
        return {};
    }
    UAssetImportTask* Task = NewObject<UAssetImportTask>();
    Task->Filename = Source;
    Task->DestinationPath = Destination;
    Task->bAutomated = true;
    Task->bReplaceExisting = true;
    Task->bReplaceExistingSettings = false;
    Task->bSave = true;
    Task->bAsync = false;
    TArray<UAssetImportTask*> Tasks{Task};
    AssetTools.ImportAssetTasks(Tasks);
    return Task->GetObjects();
}

template<typename T>
T* FirstTyped(const TArray<UObject*>& Objects)
{
    for (UObject* Object : Objects)
    {
        if (T* Typed = Cast<T>(Object)) return Typed;
    }
    return nullptr;
}

bool RenameAsset(IAssetTools& AssetTools, UObject* Asset, const FString& PackagePath, const FString& Name)
{
    if (!Asset) return false;
    if (Asset->GetName() == Name && Asset->GetOutermost()->GetName().StartsWith(PackagePath)) return true;
    TArray<FAssetRenameData> Items;
    Items.Emplace(TWeakObjectPtr<UObject>(Asset), PackagePath, Name);
    return AssetTools.RenameAssets(Items);
}

bool ImportStatic(IAssetTools& AssetTools, const FString& Relative, const FString& Destination)
{
    UStaticMesh* Mesh = FirstTyped<UStaticMesh>(ImportSource(AssetTools, Relative, Destination));
    return Mesh && RenameAsset(AssetTools, Mesh, Destination, FPaths::GetBaseFilename(Relative));
}

bool ClipNameMatches(const UAnimationAsset* Asset, const FString& Required)
{
    if (!Asset) return false;
    FString Actual = Asset->GetName().Replace(TEXT("_"), TEXT(""));
    FString Need = Required.Replace(TEXT("_"), TEXT(""));
    return Actual.Equals(Need, ESearchCase::IgnoreCase) || Actual.EndsWith(Need, ESearchCase::IgnoreCase);
}

bool ImportCharacter(IAssetTools& AssetTools, const FString& MeshSource, const FString& Destination,
    const FString& MeshName, const TArray<FString>& Packs, const TArray<FString>& Required, FCharacterAssets& Out)
{
    Out.Mesh = FirstTyped<USkeletalMesh>(ImportSource(AssetTools, MeshSource, Destination));
    if (!Out.Mesh || !RenameAsset(AssetTools, Out.Mesh, Destination, MeshName)) return false;
    Out.Skeleton = Out.Mesh->GetSkeleton();
    if (!Out.Skeleton || !RenameAsset(AssetTools, Out.Skeleton, Destination, MeshName + TEXT("_Skeleton"))) return false;

    const FString AnimPath = Destination + TEXT("/Animations");
    for (const FString& Pack : Packs)
    {
        for (UObject* Object : ImportSource(AssetTools, TEXT("ContentSource/Animations/") + Pack, AnimPath))
        {
            if (UAnimationAsset* Animation = Cast<UAnimationAsset>(Object))
            {
                Animation->SetSkeleton(Out.Skeleton);
                Animation->MarkPackageDirty();
                Out.Animations.AddUnique(Animation);
            }
        }
    }
    if (Out.Animations.Num() != Required.Num())
    {
        UE_LOG(LogTemp, Error, TEXT("%s clip count mismatch: expected %d, imported %d"), *MeshName, Required.Num(), Out.Animations.Num());
        return false;
    }
    for (const FString& Clip : Required)
    {
        bool bFound = false;
        for (const UAnimationAsset* Animation : Out.Animations) bFound |= ClipNameMatches(Animation, Clip);
        if (!bFound)
        {
            UE_LOG(LogTemp, Error, TEXT("%s missing required animation %s"), *MeshName, *Clip);
            return false;
        }
    }
    return true;
}

UWorld* OpenOrCreateWorld(const FString& PackageName)
{
    const FString AssetName = FPackageName::GetShortName(PackageName);
    if (UWorld* Existing = LoadObject<UWorld>(nullptr, *(PackageName + TEXT(".") + AssetName))) return Existing;
    UPackage* Package = CreatePackage(*PackageName);
    UWorldFactory* Factory = NewObject<UWorldFactory>();
    Factory->WorldType = EWorldType::Game;
    Factory->bCreateWorldPartition = false;
    Factory->bEnableWorldPartitionStreaming = false;
    Factory->bInformEngineOfWorld = false;
    UWorld* World = Cast<UWorld>(Factory->FactoryCreateNew(UWorld::StaticClass(), Package, *AssetName,
        RF_Public | RF_Standalone, nullptr, GWarn));
    if (World) FAssetRegistryModule::AssetCreated(World);
    return World;
}

template<typename T>
T* EnsureActor(UWorld* World, const FName Name, const FVector& Location)
{
    for (TActorIterator<T> It(World); It; ++It)
    {
        if (It->GetFName() == Name)
        {
            It->SetActorLocation(Location);
            return *It;
        }
    }
    FActorSpawnParameters Spawn;
    Spawn.Name = Name;
    Spawn.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    return World->SpawnActor<T>(T::StaticClass(), Location, FRotator::ZeroRotator, Spawn);
}

bool PlaceMesh(UWorld* World, const FString& ObjectPath, const FName Name, const FVector& Location, bool bVisible = true)
{
    UStaticMesh* Mesh = LoadObject<UStaticMesh>(nullptr, *ObjectPath);
    AStaticMeshActor* Actor = Mesh ? EnsureActor<AStaticMeshActor>(World, Name, Location) : nullptr;
    if (!Actor) return false;
    Actor->GetStaticMeshComponent()->SetStaticMesh(Mesh);
    Actor->GetStaticMeshComponent()->SetVisibility(bVisible);
    Actor->SetActorHiddenInGame(!bVisible);
    return true;
}

bool SaveWorld(UWorld* World)
{
    if (!World) return false;
    World->MarkPackageDirty();
    TArray<UPackage*> Packages{World->GetOutermost()};
    return UEditorLoadingAndSavingUtils::SavePackages(Packages, false);
}

bool PrepareGameplayWorld(UWorld* World, const FString& Layout, const FString& StartAnchor)
{
    if (!World) return false;
    World->GetWorldSettings()->DefaultGameMode = AAlphaGameplayGameMode::StaticClass();
    EnsureActor<ADarkArisenWorldDirector>(World, TEXT("DarkArisenWorldDirector"), FVector::ZeroVector);
    EnsureActor<APlayerStart>(World, TEXT("PlayerStart"), ReadAnchor(Layout, StartAnchor));
    return true;
}

bool BuildStartup()
{
    UWorld* World = OpenOrCreateWorld(TEXT("/Game/Alpha/Maps/L_AlphaStartup"));
    if (!World) return false;
    World->GetWorldSettings()->DefaultGameMode = AAlphaStartupGameMode::StaticClass();
    return SaveWorld(World);
}

bool BuildHarlow()
{
    const FString Layout = TEXT("ContentSource/Ships/Harlow/HarlowShip_GameplayLayout.json");
    UWorld* World = OpenOrCreateWorld(TEXT("/Game/Alpha/Maps/L_HarlowOpening"));
    if (!PrepareGameplayWorld(World, Layout, TEXT("Spawn.Jake.HarlowOpening"))) return false;
    if (!PlaceMesh(World, TEXT("/Game/Alpha/Ships/Harlow/SM_HarlowMerchantShip_Alpha.SM_HarlowMerchantShip_Alpha"), TEXT("HarlowMerchantShip"), FVector::ZeroVector)) return false;
    if (!PlaceMesh(World, TEXT("/Game/Alpha/Ships/Harlow/SM_HarlowShip_WalkableCollision_Alpha.SM_HarlowShip_WalkableCollision_Alpha"), TEXT("HarlowWalkableCollision"), FVector::ZeroVector, false)) return false;
    if (!PlaceMesh(World, TEXT("/Game/Alpha/Ships/Draven/SM_DravenRaidSilhouette_Alpha.SM_DravenRaidSilhouette_Alpha"), TEXT("DravenRaidSilhouette"), ReadAnchor(Layout, TEXT("Draven.Arrival")) + FVector(9000,0,0))) return false;

    struct FFamilySpec { const TCHAR* Id; const TCHAR* Anchor; const TCHAR* Label; };
    const FFamilySpec Family[] = {
        {TEXT("character.marc"), TEXT("Character.Marc"), TEXT("Marc")},
        {TEXT("character.denise"), TEXT("Character.Denise"), TEXT("Denise")},
        {TEXT("character.ethan"), TEXT("Character.Ethan"), TEXT("Ethan")}};
    for (const FFamilySpec& Spec : Family)
    {
        AOpeningFamilyInteractableActor* Actor = EnsureActor<AOpeningFamilyInteractableActor>(World,
            StableName(TEXT("Family_"), Spec.Label), ReadAnchor(Layout, Spec.Anchor));
        if (!Actor) return false;
        Actor->CharacterId = FName(Spec.Id);
        Actor->CharacterName = FText::FromString(Spec.Label);
        Actor->DialogueId = FName(*FString::Printf(TEXT("Opening.Harlow.%s"), Spec.Label));
    }

    AOpeningBoardingEncounterActor* Encounter = EnsureActor<AOpeningBoardingEncounterActor>(World,
        TEXT("OpeningBoardingEncounter"), ReadAnchor(Layout, TEXT("Family.Gathering")));
    if (!Encounter) return false;
    Encounter->EncounterId = TEXT("Encounter.HarlowRaid.MainDeck");
    Encounter->RequiredBoarders = 3;
    Encounter->SpawnTransforms = {
        FTransform(ReadAnchor(Layout, TEXT("Boarding.Port.A"))),
        FTransform(ReadAnchor(Layout, TEXT("Boarding.Port.B"))),
        FTransform(ReadAnchor(Layout, TEXT("Boarding.Starboard.A")))};
    return SaveWorld(World);
}

bool BuildDriftwood()
{
    const FString Layout = TEXT("ContentSource/World/Moran/DriftwoodBeach/DriftwoodBeach_GameplayLayout.json");
    UWorld* World = OpenOrCreateWorld(TEXT("/Game/Alpha/Maps/L_DriftwoodBeach"));
    if (!PrepareGameplayWorld(World, Layout, TEXT("Spawn.Moran.DriftwoodBeach.Recovery"))) return false;
    if (!PlaceMesh(World, TEXT("/Game/Alpha/World/Moran/DriftwoodBeach/SM_DriftwoodTerrain_Alpha.SM_DriftwoodTerrain_Alpha"), TEXT("DriftwoodTerrain"), FVector::ZeroVector)) return false;
    if (!PlaceMesh(World, TEXT("/Game/Alpha/World/Moran/DriftwoodBeach/SM_HarlowWreckage_Alpha.SM_HarlowWreckage_Alpha"), TEXT("HarlowWreckage"), ReadAnchor(Layout, TEXT("Harlow.Wreckage")))) return false;
    if (!PlaceMesh(World, TEXT("/Game/Alpha/World/Moran/OuterReef/SM_OuterReef_Alpha.SM_OuterReef_Alpha"), TEXT("OuterReef"), FVector::ZeroVector)) return false;

    AOpeningWaterCurrentVolume* Entry = EnsureActor<AOpeningWaterCurrentVolume>(World, TEXT("WaterEntryCurrent"), ReadAnchor(Layout, TEXT("WaterEntry")));
    AOpeningWaterCurrentVolume* Reef = EnsureActor<AOpeningWaterCurrentVolume>(World, TEXT("OuterReefCurrent"), ReadAnchor(Layout, TEXT("OuterReef.SafeGap")));
    AOpeningWaterCurrentVolume* Shallows = EnsureActor<AOpeningWaterCurrentVolume>(World, TEXT("ShallowsExit"), ReadAnchor(Layout, TEXT("Shallows")));
    if (!Entry || !Reef || !Shallows) return false;
    Entry->bSignalsWaterEntry = true; Entry->CurrentAcceleration = FVector(130,20,0); Entry->Volume->SetBoxExtent(FVector(1800,1400,500));
    Reef->bSignalsOuterReef = true; Reef->CurrentAcceleration = FVector(80,-35,0); Reef->Volume->SetBoxExtent(FVector(1400,1000,450));
    Shallows->bShallowExit = true; Shallows->CurrentAcceleration = FVector(45,0,0); Shallows->Volume->SetBoxExtent(FVector(900,800,250));
    return SaveWorld(World);
}

UWorld* BuildSimpleMap(const FString& MapName, const FString& MeshObjectPath, const FString& Layout, const FString& StartAnchor)
{
    UWorld* World = OpenOrCreateWorld(TEXT("/Game/Alpha/Maps/") + MapName);
    if (!PrepareGameplayWorld(World, Layout, StartAnchor)) return nullptr;
    if (!MeshObjectPath.IsEmpty() && !PlaceMesh(World, MeshObjectPath, StableName(TEXT("Geo_"), MapName), FVector::ZeroVector)) return nullptr;
    return World;
}

bool BuildMoranAndRexa()
{
    UWorld* Camp = BuildSimpleMap(TEXT("L_DriftwoodCamp"), TEXT("/Game/Alpha/World/Moran/DriftwoodCamp/SM_DriftwoodCamp_Alpha.SM_DriftwoodCamp_Alpha"), TEXT("ContentSource/World/Moran/DriftwoodCamp/DriftwoodCamp_Layout.json"), TEXT("Arrival"));
    if (!Camp || !SaveWorld(Camp)) return false;

    const FString MiraLayout = TEXT("ContentSource/World/Moran/MirasCove/MirasCove_Layout.json");
    UWorld* Mira = BuildSimpleMap(TEXT("L_MirasCove"), TEXT("/Game/Alpha/World/Moran/MirasCove/SM_MirasCove_Alpha.SM_MirasCove_Alpha"), MiraLayout, TEXT("Arrival"));
    AOpeningCrewRecruitmentActor* MiraActor = Mira ? EnsureActor<AOpeningCrewRecruitmentActor>(Mira, TEXT("Crew_Mira"), ReadAnchor(MiraLayout, TEXT("Mira"))) : nullptr;
    if (!MiraActor) return false; MiraActor->CrewId = TEXT("crew.mira"); MiraActor->DisplayName = FText::FromString(TEXT("Mira")); if (!SaveWorld(Mira)) return false;

    const FString MangroveLayout = TEXT("ContentSource/World/Moran/Mangroves/Mangroves_Layout.json");
    UWorld* Mangrove = BuildSimpleMap(TEXT("L_MangroveShallows"), TEXT("/Game/Alpha/World/Moran/Mangroves/SM_MangrovesRoute_Alpha.SM_MangrovesRoute_Alpha"), MangroveLayout, TEXT("Entry"));
    AOpeningCrewRecruitmentActor* Tom = Mangrove ? EnsureActor<AOpeningCrewRecruitmentActor>(Mangrove, TEXT("Crew_BigTom"), ReadAnchor(MangroveLayout, TEXT("BigTom"))) : nullptr;
    if (!Tom) return false; Tom->CrewId = TEXT("crew.big_tom"); Tom->DisplayName = FText::FromString(TEXT("Big Tom")); if (!SaveWorld(Mangrove)) return false;

    UWorld* Koa = BuildSimpleMap(TEXT("L_KoaTradingPost"), TEXT("/Game/Alpha/World/Moran/Koa/SM_KoaTradingPost_Alpha.SM_KoaTradingPost_Alpha"), TEXT("ContentSource/World/Moran/Koa/KoaTradingPost_Layout.json"), TEXT("Arrival"));
    if (!Koa || !SaveWorld(Koa)) return false;

    const FString CoveLayout = TEXT("ContentSource/World/Moran/GalleonCove/GalleonCove_Layout.json");
    UWorld* Cove = BuildSimpleMap(TEXT("L_GalleonCove"), TEXT("/Game/Alpha/World/Moran/GalleonCove/SM_GalleonCove_Alpha.SM_GalleonCove_Alpha"), CoveLayout, TEXT("Approach"));
    AOpeningCrewRecruitmentActor* Esteban = Cove ? EnsureActor<AOpeningCrewRecruitmentActor>(Cove, TEXT("Crew_Esteban"), ReadAnchor(CoveLayout, TEXT("Esteban"))) : nullptr;
    if (!Esteban) return false; Esteban->CrewId = TEXT("crew.esteban"); Esteban->DisplayName = FText::FromString(TEXT("Esteban"));
    ALaLiberacionShip* Ship = EnsureActor<ALaLiberacionShip>(Cove, TEXT("LaLiberacion"), ReadAnchor(CoveLayout, TEXT("ImpoundBerth")));
    if (!Ship || !PlaceMesh(Cove, TEXT("/Game/Alpha/Ships/LaLiberacion/SM_LaLiberacion_Alpha.SM_LaLiberacion_Alpha"), TEXT("LaLiberacionVisual"), Ship->GetActorLocation())) return false;
    if (!EnsureActor<ALaLiberacionHelmInteractableActor>(Cove, TEXT("LaLiberacionHelm"), ReadAnchor(CoveLayout, TEXT("Helm")))) return false;
    if (!SaveWorld(Cove)) return false;

    UWorld* Wake = OpenOrCreateWorld(TEXT("/Game/Alpha/Maps/L_OpenSea_FirstWake"));
    if (!PrepareGameplayWorld(Wake, TEXT("ContentSource/World/OpenSea/FirstWake_Route.json"), TEXT("Start"))) return false;
    ALaLiberacionShip* WakeShip = EnsureActor<ALaLiberacionShip>(Wake, TEXT("LaLiberacion"), FVector::ZeroVector);
    if (!WakeShip || !PlaceMesh(Wake, TEXT("/Game/Alpha/Ships/LaLiberacion/SM_LaLiberacion_Alpha.SM_LaLiberacion_Alpha"), TEXT("LaLiberacionVisual"), FVector::ZeroVector) || !SaveWorld(Wake)) return false;

    const FString RexaLayout = TEXT("ContentSource/World/Rexa/Harbor/RexaHarbor_Layout.json");
    UWorld* Rexa = BuildSimpleMap(TEXT("L_RexaHarbor"), TEXT("/Game/Alpha/World/Rexa/Harbor/SM_RexaHarborArrival_Alpha.SM_RexaHarborArrival_Alpha"), RexaLayout, TEXT("DockEntry"));
    if (!Rexa) return false;
    EnsureActor<ARexaSettlementDirector>(Rexa, TEXT("RexaSettlementDirector"), ReadAnchor(RexaLayout, TEXT("HarborMarket")));
    ARexaSettlementAnchor* Dock = EnsureActor<ARexaSettlementAnchor>(Rexa, TEXT("RexaAnchor_DockEntry"), ReadAnchor(RexaLayout, TEXT("DockEntry")));
    if (!Dock || !Dock->InitializeAuthoredAnchor(TEXT("Rexa.Harbor"), TEXT("Rexa.Anchor.DockEntry"), false, false)) return false;
    return SaveWorld(Rexa);
}

bool ValidateAsset(const FString& ObjectPath, UClass* Class)
{
    UObject* Object = LoadObject<UObject>(nullptr, *ObjectPath);
    if (!Object || !Object->IsA(Class))
    {
        UE_LOG(LogTemp, Error, TEXT("Required materialised asset missing/wrong class: %s"), *ObjectPath);
        return false;
    }
    return true;
}
}

UDarkArisenMaterializeAlphaCommandlet::UDarkArisenMaterializeAlphaCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
    ShowErrorCount = true;
    UseCommandletResultAsExitCode = true;
}

int32 UDarkArisenMaterializeAlphaCommandlet::Main(const FString& Params)
{
    using namespace DarkArisenAlphaMaterialize;
    const FEngineVersion Version = FEngineVersion::Current();
    if (Version.GetMajor() != 5 || Version.GetMinor() != 8)
    {
        UE_LOG(LogTemp, Error, TEXT("DarkArisen materialisation requires UE 5.8; got %s"), *Version.ToString());
        return 10;
    }

    IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools")).Get();
    const TArray<TPair<FString,FString>> StaticSources = {
        {TEXT("ContentSource/Ships/Harlow/SM_HarlowMerchantShip_Alpha.gltf"), TEXT("/Game/Alpha/Ships/Harlow")},
        {TEXT("ContentSource/Ships/Harlow/SM_HarlowShip_WalkableCollision_Alpha.gltf"), TEXT("/Game/Alpha/Ships/Harlow")},
        {TEXT("ContentSource/Ships/Harlow/SM_HarlowWreckage_Alpha.gltf"), TEXT("/Game/Alpha/World/Moran/DriftwoodBeach")},
        {TEXT("ContentSource/Ships/Draven/SM_DravenRaidSilhouette_Alpha.gltf"), TEXT("/Game/Alpha/Ships/Draven")},
        {TEXT("ContentSource/World/Moran/DriftwoodBeach/SM_DriftwoodTerrain_Alpha.gltf"), TEXT("/Game/Alpha/World/Moran/DriftwoodBeach")},
        {TEXT("ContentSource/World/Moran/OuterReef/SM_OuterReef_Alpha.gltf"), TEXT("/Game/Alpha/World/Moran/OuterReef")},
        {TEXT("ContentSource/World/Moran/DriftwoodCamp/SM_DriftwoodCamp_Alpha.gltf"), TEXT("/Game/Alpha/World/Moran/DriftwoodCamp")},
        {TEXT("ContentSource/World/Moran/MirasCove/SM_MirasCove_Alpha.gltf"), TEXT("/Game/Alpha/World/Moran/MirasCove")},
        {TEXT("ContentSource/World/Moran/Mangroves/SM_MangrovesRoute_Alpha.gltf"), TEXT("/Game/Alpha/World/Moran/Mangroves")},
        {TEXT("ContentSource/World/Moran/Koa/SM_KoaTradingPost_Alpha.gltf"), TEXT("/Game/Alpha/World/Moran/Koa")},
        {TEXT("ContentSource/World/Moran/GalleonCove/SM_GalleonCove_Alpha.gltf"), TEXT("/Game/Alpha/World/Moran/GalleonCove")},
        {TEXT("ContentSource/Ships/LaLiberacion/SM_LaLiberacion_Alpha.gltf"), TEXT("/Game/Alpha/Ships/LaLiberacion")},
        {TEXT("ContentSource/World/Rexa/Harbor/SM_RexaHarborArrival_Alpha.gltf"), TEXT("/Game/Alpha/World/Rexa/Harbor")}};
    for (const auto& Spec : StaticSources) if (!ImportStatic(AssetTools, Spec.Key, Spec.Value)) return 20;

    FCharacterAssets Jake;
    if (!ImportCharacter(AssetTools, TEXT("ContentSource/Characters/Jake/SK_Jake_Alpha.gltf"), TEXT("/Game/Alpha/Characters/Jake"), TEXT("SK_Jake_Alpha"),
        {TEXT("AN_Jake_Locomotion_Alpha.gltf"),TEXT("AN_Jake_CombatMotion_Alpha.gltf"),TEXT("AN_Jake_AttacksDefense_Alpha.gltf"),TEXT("AN_Jake_Reactions_Alpha.gltf"),TEXT("AN_Jake_WaterInteraction_Alpha.gltf")}, JakeRequired, Jake)) return 30;
    FCharacterAssets Boarder;
    if (!ImportCharacter(AssetTools, TEXT("ContentSource/Characters/Boarders/SK_Boarder_Alpha.gltf"), TEXT("/Game/Alpha/Characters/Boarders"), TEXT("SK_Boarder_Alpha"),
        {TEXT("AN_Boarder_Locomotion_Alpha.gltf"),TEXT("AN_Boarder_Actions_Alpha.gltf")}, BoarderRequired, Boarder)) return 31;

    if (!BuildStartup() || !BuildHarlow() || !BuildDriftwood() || !BuildMoranAndRexa()) return 40;
    if (!UEditorLoadingAndSavingUtils::SaveDirtyPackages(true, true)) return 41;

    const TArray<TPair<FString,UClass*>> RequiredAssets = {
        {TEXT("/Game/Alpha/Maps/L_AlphaStartup.L_AlphaStartup"), UWorld::StaticClass()},
        {TEXT("/Game/Alpha/Maps/L_HarlowOpening.L_HarlowOpening"), UWorld::StaticClass()},
        {TEXT("/Game/Alpha/Maps/L_DriftwoodBeach.L_DriftwoodBeach"), UWorld::StaticClass()},
        {TEXT("/Game/Alpha/Characters/Jake/SK_Jake_Alpha.SK_Jake_Alpha"), USkeletalMesh::StaticClass()},
        {TEXT("/Game/Alpha/Characters/Jake/SK_Jake_Alpha_Skeleton.SK_Jake_Alpha_Skeleton"), USkeleton::StaticClass()},
        {TEXT("/Game/Alpha/Characters/Boarders/SK_Boarder_Alpha.SK_Boarder_Alpha"), USkeletalMesh::StaticClass()},
        {TEXT("/Game/Alpha/Ships/Harlow/SM_HarlowMerchantShip_Alpha.SM_HarlowMerchantShip_Alpha"), UStaticMesh::StaticClass()},
        {TEXT("/Game/Alpha/Ships/LaLiberacion/SM_LaLiberacion_Alpha.SM_LaLiberacion_Alpha"), UStaticMesh::StaticClass()}};
    for (const auto& Required : RequiredAssets) if (!ValidateAsset(Required.Key, Required.Value)) return 50;

    UE_LOG(LogTemp, Display, TEXT("DarkArisen Alpha content materialised and validated."));
    return 0;
}
