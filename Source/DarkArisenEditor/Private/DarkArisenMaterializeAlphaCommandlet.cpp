#include "DarkArisenMaterializeAlphaCommandlet.h"

#include "AlphaGameplayGameMode.h"
#include "AlphaStartupGameMode.h"
#include "AssetImportTask.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "Animation/AnimationAsset.h"
#include "Animation/AnimSequence.h"
#include "Animation/Skeleton.h"
#include "Components/BoxComponent.h"
#include "Dom/JsonObject.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/SkeletalMesh.h"
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
struct FImportedCharacter
{
    USkeletalMesh* Mesh = nullptr;
    USkeleton* Skeleton = nullptr;
    TArray<UAnimationAsset*> Animations;
};

static const TArray<FString> JakeClips = {
    TEXT("Idle"), TEXT("Walk"), TEXT("Run"), TEXT("Sprint"), TEXT("TurnLeft"), TEXT("TurnRight"), TEXT("Stop"),
    TEXT("CombatIdle"), TEXT("CombatForward"), TEXT("CombatBack"), TEXT("CombatStrafeLeft"), TEXT("CombatStrafeRight"),
    TEXT("LightAttack1"), TEXT("LightAttack2"), TEXT("HeavyAttack"), TEXT("Deflect"), TEXT("DodgeForward"),
    TEXT("DodgeLeft"), TEXT("DodgeRight"), TEXT("Backstep"), TEXT("HitReactionFront"), TEXT("Stagger"), TEXT("Death"),
    TEXT("Swim"), TEXT("StrugglingSwim"), TEXT("WaterExit"), TEXT("BeachCrawl"), TEXT("BeachRecover"),
    TEXT("InteractReach"), TEXT("Pickup"), TEXT("HelmIdle")};

static const TArray<FString> BoarderClips = {
    TEXT("Idle"), TEXT("Walk"), TEXT("Run"), TEXT("CombatIdle"), TEXT("LightAttack1"), TEXT("LightAttack2"),
    TEXT("HeavyAttack"), TEXT("HitReactionFront"), TEXT("Stagger"), TEXT("Death")};

bool ReadJson(const FString& Relative, TSharedPtr<FJsonObject>& Out)
{
    FString Text;
    const FString Path = FPaths::Combine(FPaths::ProjectDir(), Relative);
    if (!FFileHelper::LoadFileToString(Text, *Path))
    {
        UE_LOG(LogTemp, Error, TEXT("Materializer: cannot read %s"), *Path);
        return false;
    }
    return FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Text), Out) && Out.IsValid();
}

FVector Anchor(const FString& Relative, const FString& Name, const FVector& Fallback = FVector::ZeroVector)
{
    TSharedPtr<FJsonObject> Root;
    if (!ReadJson(Relative, Root)) return Fallback;
    const TSharedPtr<FJsonObject>* Anchors = nullptr;
    if (!Root->TryGetObjectField(TEXT("anchors"), Anchors) || !Anchors || !Anchors->IsValid()) return Fallback;
    const TArray<TSharedPtr<FJsonValue>>* Values = nullptr;
    if (!(*Anchors)->TryGetArrayField(Name, Values) || !Values || Values->Num() < 3) return Fallback;
    return FVector((*Values)[0]->AsNumber(), (*Values)[1]->AsNumber(), (*Values)[2]->AsNumber());
}

bool RenameAsset(UObject* Asset, const FString& PackagePath, const FString& Name, IAssetTools& AssetTools)
{
    if (!Asset) return false;
    if (Asset->GetName() == Name && Asset->GetOutermost()->GetName().StartsWith(PackagePath)) return true;
    TArray<FAssetRenameData> Renames;
    Renames.Emplace(TWeakObjectPtr<UObject>(Asset), PackagePath, Name);
    return AssetTools.RenameAssets(Renames);
}

TArray<UObject*> ImportFile(const FString& Relative, const FString& Destination, IAssetTools& AssetTools)
{
    const FString Source = FPaths::Combine(FPaths::ProjectDir(), Relative);
    if (!IFileManager::Get().FileExists(*Source))
    {
        UE_LOG(LogTemp, Error, TEXT("Materializer: source missing: %s"), *Source);
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
    TArray<UObject*> Result;
    for (UObject* Object : Task->GetObjects()) if (Object) Result.Add(Object);
    if (Result.IsEmpty()) UE_LOG(LogTemp, Error, TEXT("Materializer: import produced no objects: %s"), *Source);
    return Result;
}

template<typename T> T* FirstOf(const TArray<UObject*>& Objects)
{
    for (UObject* Object : Objects) if (T* Typed = Cast<T>(Object)) return Typed;
    return nullptr;
}

bool ImportStatic(const FString& Relative, const FString& Destination, IAssetTools& AssetTools)
{
    TArray<UObject*> Objects = ImportFile(Relative, Destination, AssetTools);
    UStaticMesh* Mesh = FirstOf<UStaticMesh>(Objects);
    if (!Mesh) return false;
    return RenameAsset(Mesh, Destination, FPaths::GetBaseFilename(Relative), AssetTools);
}

bool HasClip(const TArray<UAnimationAsset*>& Animations, const FString& Required)
{
    for (const UAnimationAsset* Animation : Animations)
    {
        const FString Name = Animation->GetName().Replace(TEXT("_"), TEXT(""));
        const FString Need = Required.Replace(TEXT("_"), TEXT(""));
        if (Name.Equals(Need, ESearchCase::IgnoreCase) || Name.EndsWith(Need, ESearchCase::IgnoreCase)) return true;
    }
    return false;
}

bool ImportCharacter(const FString& MeshSource, const TArray<FString>& Packs, const FString& Destination,
    const FString& MeshName, const TArray<FString>& RequiredClips, IAssetTools& AssetTools, FImportedCharacter& Out)
{
    TArray<UObject*> MeshObjects = ImportFile(MeshSource, Destination, AssetTools);
    Out.Mesh = FirstOf<USkeletalMesh>(MeshObjects);
    if (!Out.Mesh || !RenameAsset(Out.Mesh, Destination, MeshName, AssetTools)) return false;
    Out.Skeleton = Out.Mesh->GetSkeleton();
    if (!Out.Skeleton || !RenameAsset(Out.Skeleton, Destination, MeshName + TEXT("_Skeleton"), AssetTools)) return false;

    const FString AnimDestination = Destination + TEXT("/Animations");
    for (const FString& Pack : Packs)
    {
        TArray<UObject*> PackObjects = ImportFile(TEXT("ContentSource/Animations/") + Pack, AnimDestination, AssetTools);
        for (UObject* Object : PackObjects)
        {
            if (UAnimationAsset* Animation = Cast<UAnimationAsset>(Object))
            {
                Animation->SetSkeleton(Out.Skeleton);
                Animation->MarkPackageDirty();
                Out.Animations.AddUnique(Animation);
            }
        }
    }
    if (Out.Animations.Num() != RequiredClips.Num())
    {
        UE_LOG(LogTemp, Error, TEXT("Materializer: %s expected %d clips, imported %d"), *MeshName, RequiredClips.Num(), Out.Animations.Num());
        return false;
    }
    for (const FString& Required : RequiredClips)
    {
        if (!HasClip(Out.Animations, Required))
        {
            UE_LOG(LogTemp, Error, TEXT("Materializer: %s missing clip %s"), *MeshName, *Required);
            return false;
        }
    }
    return true;
}

UWorld* CreateOrLoadWorld(const FString& PackageName)
{
    const FString AssetName = FPackageName::GetShortName(PackageName);
    const FString ObjectPath = PackageName + TEXT(".") + AssetName;
    if (UWorld* Existing = LoadObject<UWorld>(nullptr, *ObjectPath)) return Existing;
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

template<typename T> T* EnsureActor(UWorld* World, const FName Name, const FVector& Location)
{
    for (TActorIterator<T> It(World); It; ++It)
    {
        if (It->GetFName() == Name)
        {
            It->SetActorLocation(Location);
            return *It;
        }
    }
    FActorSpawnParameters Params;
    Params.Name = Name;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    return World->SpawnActor<T>(T::StaticClass(), Location, FRotator::ZeroRotator, Params);
}

bool PlaceMesh(UWorld* World, const FString& ObjectPath, const FName ActorName, const FVector& Location, bool bVisible = true)
{
    UStaticMesh* Mesh = LoadObject<UStaticMesh>(nullptr, *ObjectPath);
    if (!Mesh) return false;
    AStaticMeshActor* Actor = EnsureActor<AStaticMeshActor>(World, ActorName, Location);
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

bool BuildStartup()
{
    UWorld* World = CreateOrLoadWorld(TEXT("/Game/Alpha/Maps/L_AlphaStartup"));
    if (!World) return false;
    World->GetWorldSettings()->DefaultGameMode = AAlphaStartupGameMode::StaticClass();
    return SaveWorld(World);
}

bool BuildHarlow()
{
    const FString Layout = TEXT("ContentSource/Ships/Harlow/HarlowShip_GameplayLayout.json");
    UWorld* World = CreateOrLoadWorld(TEXT("/Game/Alpha/Maps/L_HarlowOpening"));
    if (!World) return false;
    World->GetWorldSettings()->DefaultGameMode = AAlphaGameplayGameMode::StaticClass();
    if (!PlaceMesh(World, TEXT("/Game/Alpha/Ships/Harlow/SM_HarlowMerchantShip_Alpha.SM_HarlowMerchantShip_Alpha"), TEXT("HarlowMerchantShip"), FVector::ZeroVector)) return false;
    if (!PlaceMesh(World, TEXT("/Game/Alpha/Ships/Harlow/SM_HarlowShip_WalkableCollision_Alpha.SM_HarlowShip_WalkableCollision_Alpha"), TEXT("HarlowWalkableCollision"), FVector::ZeroVector, false)) return false;
    PlaceMesh(World, TEXT("/Game/Alpha/Ships/Draven/SM_DravenRaidSilhouette_Alpha.SM_DravenRaidSilhouette_Alpha"), TEXT("DravenRaidSilhouette"), Anchor(Layout, TEXT("Draven.Arrival")) + FVector(9000,0,0));
    EnsureActor<ADarkArisenWorldDirector>(World, TEXT("DarkArisenWorldDirector"), FVector::ZeroVector);
    EnsureActor<APlayerStart>(World, TEXT("PlayerStart_Harlow"), Anchor(Layout, TEXT("Spawn.Jake.HarlowOpening")));

    struct FFamily { const TCHAR* Name; const TCHAR* AnchorId; const TCHAR* CharacterId; const TCHAR* Display; };
    const FFamily Family[] = {
        {TEXT("Marc"), TEXT("Character.Marc"), TEXT("character.marc"), TEXT("Marc")},
        {TEXT("Denise"), TEXT("Character.Denise"), TEXT("character.denise"), TEXT("Denise")},
        {TEXT("Ethan"), TEXT("Character.Ethan"), TEXT("character.ethan"), TEXT("Ethan")}};
    for (const FFamily& Entry : Family)
    {
        AOpeningFamilyInteractableActor* Actor = EnsureActor<AOpeningFamilyInteractableActor>(World, *FString(TEXT("Family_")) + Entry.Name, Anchor(Layout, Entry.AnchorId));
        if (!Actor) return false;
        Actor->CharacterId = Entry.CharacterId;
        Actor->CharacterName = FText::FromString(Entry.Display);
        Actor->DialogueId = *FString::Printf(TEXT("Opening.Harlow.%s"), Entry.Name);
    }

    AOpeningBoardingEncounterActor* Encounter = EnsureActor<AOpeningBoardingEncounterActor>(World, TEXT("OpeningBoardingEncounter"), Anchor(Layout, TEXT("Family.Gathering")));
    if (!Encounter) return false;
    Encounter->EncounterId = TEXT("Encounter.HarlowRaid.MainDeck");
    Encounter->RequiredBoarders = 3;
    Encounter->SpawnTransforms = {
        FTransform(Anchor(Layout, TEXT("Boarding.Port.A"))),
        FTransform(Anchor(Layout, TEXT("Boarding.Port.B"))),
        FTransform(Anchor(Layout, TEXT("Boarding.Starboard.A")))};
    return SaveWorld(World);
}

bool BuildDriftwood()
{
    const FString Layout = TEXT("ContentSource/World/Moran/DriftwoodBeach/DriftwoodBeach_GameplayLayout.json");
    UWorld* World = CreateOrLoadWorld(TEXT("/Game/Alpha/Maps/L_DriftwoodBeach"));
    if (!World) return false;
    World->GetWorldSettings()->DefaultGameMode = AAlphaGameplayGameMode::StaticClass();
    if (!PlaceMesh(World, TEXT("/Game/Alpha/World/Moran/DriftwoodBeach/SM_DriftwoodTerrain_Alpha.SM_DriftwoodTerrain_Alpha"), TEXT("DriftwoodTerrain"), FVector::ZeroVector)) return false;
    PlaceMesh(World, TEXT("/Game/Alpha/World/Moran/DriftwoodBeach/SM_HarlowWreckage_Alpha.SM_HarlowWreckage_Alpha"), TEXT("HarlowWreckage"), Anchor(Layout, TEXT("Harlow.Wreckage")));
    PlaceMesh(World, TEXT("/Game/Alpha/World/Moran/OuterReef/SM_OuterReef_Alpha.SM_OuterReef_Alpha"), TEXT("OuterReef"), FVector::ZeroVector);
    EnsureActor<ADarkArisenWorldDirector>(World, TEXT("DarkArisenWorldDirector"), FVector::ZeroVector);
    EnsureActor<APlayerStart>(World, TEXT("PlayerStart_Driftwood"), Anchor(Layout, TEXT("Spawn.Moran.DriftwoodBeach.Recovery")));

    AOpeningWaterCurrentVolume* Water = EnsureActor<AOpeningWaterCurrentVolume>(World, TEXT("WaterEntryCurrent"), Anchor(Layout, TEXT("WaterEntry")));
    AOpeningWaterCurrentVolume* Reef = EnsureActor<AOpeningWaterCurrentVolume>(World, TEXT("OuterReefCurrent"), Anchor(Layout, TEXT("OuterReef.SafeGap")));
    AOpeningWaterCurrentVolume* Shallows = EnsureActor<AOpeningWaterCurrentVolume>(World, TEXT("ShallowsExit"), Anchor(Layout, TEXT("Shallows")));
    if (!Water || !Reef || !Shallows) return false;
    Water->bSignalsWaterEntry = true; Water->CurrentAcceleration = FVector(130.f, 20.f, 0.f); Water->Volume->SetBoxExtent(FVector(1800,1400,500));
    Reef->bSignalsOuterReef = true; Reef->CurrentAcceleration = FVector(80.f,-35.f,0.f); Reef->Volume->SetBoxExtent(FVector(1400,1000,450));
    Shallows->bShallowExit = true; Shallows->CurrentAcceleration = FVector(45.f,0.f,0.f); Shallows->Volume->SetBoxExtent(FVector(900,800,250));
    return SaveWorld(World);
}

bool BuildLocationMap(const FString& MapName, const FString& MeshPath, const FString& Layout, const FString& StartAnchor)
{
    UWorld* World = CreateOrLoadWorld(TEXT("/Game/Alpha/Maps/") + MapName);
    if (!World) return false;
    World->GetWorldSettings()->DefaultGameMode = AAlphaGameplayGameMode::StaticClass();
    if (!MeshPath.IsEmpty() && !PlaceMesh(World, MeshPath, *FString(TEXT("Geo_")) + MapName, FVector::ZeroVector)) return false;
    EnsureActor<ADarkArisenWorldDirector>(World, TEXT("DarkArisenWorldDirector"), FVector::ZeroVector);
    EnsureActor<APlayerStart>(World, *FString(TEXT("PlayerStart_")) + MapName, Anchor(Layout, StartAnchor));
    return true;
}

bool BuildMoranAndRexa()
{
    UWorld* Camp = CreateOrLoadWorld(TEXT("/Game/Alpha/Maps/L_DriftwoodCamp"));
    if (!Camp || !BuildLocationMap(TEXT("L_DriftwoodCamp"), TEXT("/Game/Alpha/World/Moran/DriftwoodCamp/SM_DriftwoodCamp_Alpha.SM_DriftwoodCamp_Alpha"), TEXT("ContentSource/World/Moran/DriftwoodCamp/DriftwoodCamp_Layout.json"), TEXT("Arrival")) || !SaveWorld(Camp)) return false;

    UWorld* Mira = CreateOrLoadWorld(TEXT("/Game/Alpha/Maps/L_MirasCove"));
    if (!Mira || !BuildLocationMap(TEXT("L_MirasCove"), TEXT("/Game/Alpha/World/Moran/MirasCove/SM_MirasCove_Alpha.SM_MirasCove_Alpha"), TEXT("ContentSource/World/Moran/MirasCove/MirasCove_Layout.json"), TEXT("Arrival"))) return false;
    AOpeningCrewRecruitmentActor* MiraActor = EnsureActor<AOpeningCrewRecruitmentActor>(Mira, TEXT("Crew_Mira"), Anchor(TEXT("ContentSource/World/Moran/MirasCove/MirasCove_Layout.json"), TEXT("Mira")));
    if (!MiraActor) return false; MiraActor->CrewId = TEXT("crew.mira"); MiraActor->DisplayName = FText::FromString(TEXT("Mira")); if (!SaveWorld(Mira)) return false;

    UWorld* Mangrove = CreateOrLoadWorld(TEXT("/Game/Alpha/Maps/L_MangroveShallows"));
    if (!Mangrove || !BuildLocationMap(TEXT("L_MangroveShallows"), TEXT("/Game/Alpha/World/Moran/Mangroves/SM_MangrovesRoute_Alpha.SM_MangrovesRoute_Alpha"), TEXT("ContentSource/World/Moran/Mangroves/Mangroves_Layout.json"), TEXT("Entry"))) return false;
    AOpeningCrewRecruitmentActor* Tom = EnsureActor<AOpeningCrewRecruitmentActor>(Mangrove, TEXT("Crew_BigTom"), Anchor(TEXT("ContentSource/World/Moran/Mangroves/Mangroves_Layout.json"), TEXT("BigTom")));
    if (!Tom) return false; Tom->CrewId = TEXT("crew.big_tom"); Tom->DisplayName = FText::FromString(TEXT("Big Tom")); if (!SaveWorld(Mangrove)) return false;

    UWorld* Koa = CreateOrLoadWorld(TEXT("/Game/Alpha/Maps/L_KoaTradingPost"));
    if (!Koa || !BuildLocationMap(TEXT("L_KoaTradingPost"), TEXT("/Game/Alpha/World/Moran/Koa/SM_KoaTradingPost_Alpha.SM_KoaTradingPost_Alpha"), TEXT("ContentSource/World/Moran/Koa/KoaTradingPost_Layout.json"), TEXT("Arrival")) || !SaveWorld(Koa)) return false;

    UWorld* Cove = CreateOrLoadWorld(TEXT("/Game/Alpha/Maps/L_GalleonCove"));
    if (!Cove || !BuildLocationMap(TEXT("L_GalleonCove"), TEXT("/Game/Alpha/World/Moran/GalleonCove/SM_GalleonCove_Alpha.SM_GalleonCove_Alpha"), TEXT("ContentSource/World/Moran/GalleonCove/GalleonCove_Layout.json"), TEXT("Approach"))) return false;
    AOpeningCrewRecruitmentActor* Esteban = EnsureActor<AOpeningCrewRecruitmentActor>(Cove, TEXT("Crew_Esteban"), Anchor(TEXT("ContentSource/World/Moran/GalleonCove/GalleonCove_Layout.json"), TEXT("Esteban")));
    if (!Esteban) return false; Esteban->CrewId = TEXT("crew.esteban"); Esteban->DisplayName = FText::FromString(TEXT("Esteban"));
    ALaLiberacionShip* Ship = EnsureActor<ALaLiberacionShip>(Cove, TEXT("LaLiberacion"), Anchor(TEXT("ContentSource/World/Moran/GalleonCove/GalleonCove_Layout.json"), TEXT("ImpoundBerth")));
    if (!Ship || !PlaceMesh(Cove, TEXT("/Game/Alpha/Ships/LaLiberacion/SM_LaLiberacion_Alpha.SM_LaLiberacion_Alpha"), TEXT("LaLiberacionVisual"), Ship->GetActorLocation())) return false;
    EnsureActor<ALaLiberacionHelmInteractableActor>(Cove, TEXT("LaLiberacionHelm"), Anchor(TEXT("ContentSource/World/Moran/GalleonCove/GalleonCove_Layout.json"), TEXT("Helm")));
    if (!SaveWorld(Cove)) return false;

    UWorld* Wake = CreateOrLoadWorld(TEXT("/Game/Alpha/Maps/L_OpenSea_FirstWake"));
    if (!Wake) return false; Wake->GetWorldSettings()->DefaultGameMode = AAlphaGameplayGameMode::StaticClass();
    EnsureActor<ADarkArisenWorldDirector>(Wake, TEXT("DarkArisenWorldDirector"), FVector::ZeroVector);
    ALaLiberacionShip* WakeShip = EnsureActor<ALaLiberacionShip>(Wake, TEXT("LaLiberacion"), FVector::ZeroVector);
    if (!WakeShip || !PlaceMesh(Wake, TEXT("/Game/Alpha/Ships/LaLiberacion/SM_LaLiberacion_Alpha.SM_LaLiberacion_Alpha"), TEXT("LaLiberacionVisual"), FVector::ZeroVector) || !SaveWorld(Wake)) return false;

    UWorld* Rexa = CreateOrLoadWorld(TEXT("/Game/Alpha/Maps/L_RexaHarbor"));
    if (!Rexa || !BuildLocationMap(TEXT("L_RexaHarbor"), TEXT("/Game/Alpha/World/Rexa/Harbor/SM_RexaHarborArrival_Alpha.SM_RexaHarborArrival_Alpha"), TEXT("ContentSource/World/Rexa/Harbor/RexaHarbor_Layout.json"), TEXT("DockEntry"))) return false;
    EnsureActor<ARexaSettlementDirector>(Rexa, TEXT("RexaSettlementDirector"), Anchor(TEXT("ContentSource/World/Rexa/Harbor/RexaHarbor_Layout.json"), TEXT("HarborMarket")));
    ARexaSettlementAnchor* Dock = EnsureActor<ARexaSettlementAnchor>(Rexa, TEXT("RexaAnchor_DockEntry"), Anchor(TEXT("ContentSource/World/Rexa/Harbor/RexaHarbor_Layout.json"), TEXT("DockEntry")));
    if (Dock) Dock->InitializeAuthoredAnchor(TEXT("Rexa.Harbor"), TEXT("Rexa.Anchor.DockEntry"), false, false);
    return SaveWorld(Rexa);
}

bool ValidateObject(const FString& ObjectPath, UClass* RequiredClass)
{
    UObject* Object = LoadObject<UObject>(nullptr, *ObjectPath);
    if (!Object || !Object->IsA(RequiredClass))
    {
        UE_LOG(LogTemp, Error, TEXT("Materializer: required asset missing or wrong class: %s"), *ObjectPath);
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
        UE_LOG(LogTemp, Error, TEXT("DarkArisen materialisation requires Unreal Engine 5.8; got %s"), *Version.ToString());
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
    for (const TPair<FString,FString>& Spec : StaticSources) if (!ImportStatic(Spec.Key, Spec.Value, AssetTools)) return 20;

    FImportedCharacter Jake;
    if (!ImportCharacter(TEXT("ContentSource/Characters/Jake/SK_Jake_Alpha.gltf"),
        {TEXT("AN_Jake_Locomotion_Alpha.gltf"),TEXT("AN_Jake_CombatMotion_Alpha.gltf"),TEXT("AN_Jake_AttacksDefense_Alpha.gltf"),TEXT("AN_Jake_Reactions_Alpha.gltf"),TEXT("AN_Jake_WaterInteraction_Alpha.gltf")},
        TEXT("/Game/Alpha/Characters/Jake"), TEXT("SK_Jake_Alpha"), JakeClips, AssetTools, Jake)) return 30;
    FImportedCharacter Boarder;
    if (!ImportCharacter(TEXT("ContentSource/Characters/Boarders/SK_Boarder_Alpha.gltf"),
        {TEXT("AN_Boarder_Locomotion_Alpha.gltf"),TEXT("AN_Boarder_Actions_Alpha.gltf")},
        TEXT("/Game/Alpha/Characters/Boarders"), TEXT("SK_Boarder_Alpha"), BoarderClips, AssetTools, Boarder)) return 31;

    if (!BuildStartup() || !BuildHarlow() || !BuildDriftwood() || !BuildMoranAndRexa()) return 40;
    UEditorLoadingAndSavingUtils::SaveDirtyPackages(true, true);

    const TArray<TPair<FString,UClass*>> Required = {
        {TEXT("/Game/Alpha/Maps/L_AlphaStartup.L_AlphaStartup"), UWorld::StaticClass()},
        {TEXT("/Game/Alpha/Maps/L_HarlowOpening.L_HarlowOpening"), UWorld::StaticClass()},
        {TEXT("/Game/Alpha/Maps/L_DriftwoodBeach.L_DriftwoodBeach"), UWorld::StaticClass()},
        {TEXT("/Game/Alpha/Characters/Jake/SK_Jake_Alpha.SK_Jake_Alpha"), USkeletalMesh::StaticClass()},
        {TEXT("/Game/Alpha/Characters/Jake/SK_Jake_Alpha_Skeleton.SK_Jake_Alpha_Skeleton"), USkeleton::StaticClass()},
        {TEXT("/Game/Alpha/Characters/Boarders/SK_Boarder_Alpha.SK_Boarder_Alpha"), USkeletalMesh::StaticClass()},
        {TEXT("/Game/Alpha/Ships/Harlow/SM_HarlowMerchantShip_Alpha.SM_HarlowMerchantShip_Alpha"), UStaticMesh::StaticClass()},
        {TEXT("/Game/Alpha/Ships/LaLiberacion/SM_LaLiberacion_Alpha.SM_LaLiberacion_Alpha"), UStaticMesh::StaticClass()}};
    for (const auto& Entry : Required) if (!ValidateObject(Entry.Key, Entry.Value)) return 50;

    UE_LOG(LogTemp, Display, TEXT("DarkArisen Alpha content materialised and validated for UE 5.8."));
    return 0;
}
