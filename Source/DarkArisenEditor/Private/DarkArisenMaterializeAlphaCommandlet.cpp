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

bool TryReadRequiredAnchor(const FString& Relative, const FString& Key, FVector& OutLocation)
{
    TSharedPtr<FJsonObject> Root;
    if (!ReadJson(Relative, Root)) return false;
    const TSharedPtr<FJsonObject>* Anchors = nullptr;
    if (!Root->TryGetObjectField(TEXT("anchors"), Anchors) || !Anchors || !Anchors->IsValid())
    { UE_LOG(LogTemp, Error, TEXT("Materializer: required anchors missing in %s"), *Relative); return false; }
    const TArray<TSharedPtr<FJsonValue>>* Values = nullptr;
    if (!(*Anchors)->TryGetArrayField(Key, Values) || !Values || Values->Num() != 3)
    { UE_LOG(LogTemp, Error, TEXT("Materializer: required anchor %s missing/invalid in %s"), *Key, *Relative); return false; }
    for (const TSharedPtr<FJsonValue>& Value : *Values) if (!Value.IsValid() || Value->Type != EJson::Number) return false;
    OutLocation = FVector((*Values)[0]->AsNumber(), (*Values)[1]->AsNumber(), (*Values)[2]->AsNumber());
    if (OutLocation.ContainsNaN()) return false;
    return true;
}

FName StableName(const FString& Prefix, const FString& Suffix)
{ const FString NameString = Prefix + Suffix; return FName(*NameString); }

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
            if (UAnimSequence* Sequence = Cast<UAnimSequence>(Object))
            { USkeleton* Imported=Sequence->GetSkeleton(); if(!Imported||Imported->GetReferenceSkeleton().GetNum()<=0||Out.Skeleton->GetReferenceSkeleton().GetNum()<=0||Imported->GetReferenceSkeleton().GetBoneName(0)!=Out.Skeleton->GetReferenceSkeleton().GetBoneName(0)||Sequence->GetPlayLength()<=KINDA_SMALL_NUMBER) return false; Sequence->SetSkeleton(Out.Skeleton); Sequence->MarkPackageDirty(); Out.Animations.AddUnique(Sequence); }
        }
    }
    if (Out.Skeleton->GetReferenceSkeleton().FindBoneIndex(TEXT("root")) == INDEX_NONE) return false;
    TSet<FString> ImportedNames; for(const UAnimationAsset* Animation:Out.Animations){const FString N=Animation->GetName().Replace(TEXT("_"),TEXT("")).ToLower();if(ImportedNames.Contains(N))return false;ImportedNames.Add(N);}
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
    if (!World) return false; FVector StartLocation;
    if (!TryReadRequiredAnchor(Layout, StartAnchor, StartLocation)) return false;
    World->GetWorldSettings()->DefaultGameMode = AAlphaGameplayGameMode::StaticClass();
    return EnsureActor<ADarkArisenWorldDirector>(World, TEXT("DarkArisenWorldDirector"), FVector::ZeroVector) && EnsureActor<APlayerStart>(World, TEXT("PlayerStart"), StartLocation);
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
 const FString Layout=TEXT("ContentSource/Ships/Harlow/HarlowShip_GameplayLayout.json"); UWorld* World=OpenOrCreateWorld(TEXT("/Game/Alpha/Maps/L_HarlowOpening")); if(!PrepareGameplayWorld(World,Layout,TEXT("Spawn.Jake.HarlowOpening"))) return false;
 FVector Draven,FamilyGathering,PortA,PortB,StarboardA,Cargo,Helm,Overboard;
 if(!TryReadRequiredAnchor(Layout,TEXT("Draven.Arrival"),Draven)||!TryReadRequiredAnchor(Layout,TEXT("Family.Gathering"),FamilyGathering)||!TryReadRequiredAnchor(Layout,TEXT("Boarding.Port.A"),PortA)||!TryReadRequiredAnchor(Layout,TEXT("Boarding.Port.B"),PortB)||!TryReadRequiredAnchor(Layout,TEXT("Boarding.Starboard.A"),StarboardA)||!TryReadRequiredAnchor(Layout,TEXT("Interaction.Cargo.Manifest"),Cargo)||!TryReadRequiredAnchor(Layout,TEXT("Helm"),Helm)||!TryReadRequiredAnchor(Layout,TEXT("Overboard"),Overboard)) return false;
 if(!PlaceMesh(World,TEXT("/Game/Alpha/Ships/Harlow/SM_HarlowMerchantShip_Alpha.SM_HarlowMerchantShip_Alpha"),TEXT("HarlowMerchantShip"),FVector::ZeroVector)||!PlaceMesh(World,TEXT("/Game/Alpha/Ships/Harlow/SM_HarlowShip_WalkableCollision_Alpha.SM_HarlowShip_WalkableCollision_Alpha"),TEXT("HarlowWalkableCollision"),FVector::ZeroVector,false)||!PlaceMesh(World,TEXT("/Game/Alpha/Ships/Draven/SM_DravenRaidSilhouette_Alpha.SM_DravenRaidSilhouette_Alpha"),TEXT("DravenRaidSilhouette"),Draven+FVector(9000,0,0))) return false;
 struct FFamilySpec{const TCHAR* Id;const TCHAR* Anchor;const TCHAR* Label;}; const FFamilySpec Family[]={{TEXT("character.marc"),TEXT("Character.Marc"),TEXT("Marc")},{TEXT("character.denise"),TEXT("Character.Denise"),TEXT("Denise")},{TEXT("character.ethan"),TEXT("Character.Ethan"),TEXT("Ethan")}};
 for(const FFamilySpec& Spec:Family){FVector Loc;if(!TryReadRequiredAnchor(Layout,Spec.Anchor,Loc)) return false;const FString NameString=FString::Printf(TEXT("Family_%s"),Spec.Label);const FName Name(*NameString);auto* Actor=EnsureActor<AOpeningFamilyInteractableActor>(World,Name,Loc);if(!Actor)return false;Actor->CharacterId=FName(Spec.Id);Actor->CharacterName=FText::FromString(Spec.Label);const FString Dialogue=FString::Printf(TEXT("Opening.Harlow.%s"),Spec.Label);Actor->DialogueId=FName(*Dialogue);}
 auto* Encounter=EnsureActor<AOpeningBoardingEncounterActor>(World,TEXT("OpeningBoardingEncounter"),FamilyGathering);if(!Encounter)return false;Encounter->EncounterId=TEXT("Encounter.HarlowRaid.MainDeck");Encounter->RequiredBoarders=3;Encounter->SpawnTransforms={FTransform(PortA),FTransform(PortB),FTransform(StarboardA)};
 if(!EnsureActor<AActor>(World,TEXT("OpeningCargoAnchor"),Cargo)||!EnsureActor<AActor>(World,TEXT("OpeningHelmAnchor"),Helm)||!EnsureActor<AActor>(World,TEXT("OpeningOverboardAnchor"),Overboard)) return false; return SaveWorld(World);
}

bool BuildDriftwood()
{
 const FString Layout=TEXT("ContentSource/World/Moran/DriftwoodBeach/DriftwoodBeach_GameplayLayout.json"); UWorld* World=OpenOrCreateWorld(TEXT("/Game/Alpha/Maps/L_DriftwoodBeach")); if(!PrepareGameplayWorld(World,Layout,TEXT("Spawn.Moran.DriftwoodBeach.Recovery")))return false;
 FVector Wreckage,Water,ReefGap,Shallows;if(!TryReadRequiredAnchor(Layout,TEXT("Harlow.Wreckage"),Wreckage)||!TryReadRequiredAnchor(Layout,TEXT("WaterEntry"),Water)||!TryReadRequiredAnchor(Layout,TEXT("OuterReef.SafeGap"),ReefGap)||!TryReadRequiredAnchor(Layout,TEXT("Shallows"),Shallows))return false;
 if(!PlaceMesh(World,TEXT("/Game/Alpha/World/Moran/DriftwoodBeach/SM_DriftwoodTerrain_Alpha.SM_DriftwoodTerrain_Alpha"),TEXT("DriftwoodTerrain"),FVector::ZeroVector)||!PlaceMesh(World,TEXT("/Game/Alpha/World/Moran/DriftwoodBeach/SM_HarlowWreckage_Alpha.SM_HarlowWreckage_Alpha"),TEXT("HarlowWreckage"),Wreckage)||!PlaceMesh(World,TEXT("/Game/Alpha/World/Moran/OuterReef/SM_OuterReef_Alpha.SM_OuterReef_Alpha"),TEXT("OuterReef"),FVector::ZeroVector))return false;
 auto* Entry=EnsureActor<AOpeningWaterCurrentVolume>(World,TEXT("WaterEntryCurrent"),Water);auto* Reef=EnsureActor<AOpeningWaterCurrentVolume>(World,TEXT("OuterReefCurrent"),ReefGap);auto* Shallow=EnsureActor<AOpeningWaterCurrentVolume>(World,TEXT("ShallowsExit"),Shallows);if(!Entry||!Reef||!Shallow)return false;
 Entry->bSignalsWaterEntry=true;Entry->CurrentAcceleration=FVector(130,20,0);Entry->Volume->SetBoxExtent(FVector(1800,1400,500));Reef->bSignalsOuterReef=true;Reef->CurrentAcceleration=FVector(80,-35,0);Reef->Volume->SetBoxExtent(FVector(1400,1000,450));Shallow->bShallowExit=true;Shallow->CurrentAcceleration=FVector(45,0,0);Shallow->Volume->SetBoxExtent(FVector(900,800,250));return SaveWorld(World);
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
 UWorld* Camp=BuildSimpleMap(TEXT("L_DriftwoodCamp"),TEXT("/Game/Alpha/World/Moran/DriftwoodCamp/SM_DriftwoodCamp_Alpha.SM_DriftwoodCamp_Alpha"),TEXT("ContentSource/World/Moran/DriftwoodCamp/DriftwoodCamp_Layout.json"),TEXT("Arrival"));if(!Camp||!SaveWorld(Camp))return false;
 const FString ML=TEXT("ContentSource/World/Moran/MirasCove/MirasCove_Layout.json");UWorld* Mira=BuildSimpleMap(TEXT("L_MirasCove"),TEXT("/Game/Alpha/World/Moran/MirasCove/SM_MirasCove_Alpha.SM_MirasCove_Alpha"),ML,TEXT("Arrival"));FVector MLoc;if(!Mira||!TryReadRequiredAnchor(ML,TEXT("Mira"),MLoc))return false;auto* MA=EnsureActor<AOpeningCrewRecruitmentActor>(Mira,TEXT("Crew_Mira"),MLoc);if(!MA)return false;MA->CrewId=TEXT("crew.mira");MA->DisplayName=FText::FromString(TEXT("Mira"));if(!SaveWorld(Mira))return false;
 const FString TL=TEXT("ContentSource/World/Moran/Mangroves/Mangroves_Layout.json");UWorld* Mang=BuildSimpleMap(TEXT("L_MangroveShallows"),TEXT("/Game/Alpha/World/Moran/Mangroves/SM_MangrovesRoute_Alpha.SM_MangrovesRoute_Alpha"),TL,TEXT("Entry"));FVector TLoc;if(!Mang||!TryReadRequiredAnchor(TL,TEXT("BigTom"),TLoc))return false;auto* Tom=EnsureActor<AOpeningCrewRecruitmentActor>(Mang,TEXT("Crew_BigTom"),TLoc);if(!Tom)return false;Tom->CrewId=TEXT("crew.big_tom");Tom->DisplayName=FText::FromString(TEXT("Big Tom"));if(!SaveWorld(Mang))return false;
 UWorld* Koa=BuildSimpleMap(TEXT("L_KoaTradingPost"),TEXT("/Game/Alpha/World/Moran/Koa/SM_KoaTradingPost_Alpha.SM_KoaTradingPost_Alpha"),TEXT("ContentSource/World/Moran/Koa/KoaTradingPost_Layout.json"),TEXT("Arrival"));if(!Koa||!SaveWorld(Koa))return false;
 const FString CL=TEXT("ContentSource/World/Moran/GalleonCove/GalleonCove_Layout.json");UWorld* Cove=BuildSimpleMap(TEXT("L_GalleonCove"),TEXT("/Game/Alpha/World/Moran/GalleonCove/SM_GalleonCove_Alpha.SM_GalleonCove_Alpha"),CL,TEXT("Approach"));FVector ELoc,Berth,Helm,Exit;if(!Cove||!TryReadRequiredAnchor(CL,TEXT("Esteban"),ELoc)||!TryReadRequiredAnchor(CL,TEXT("ImpoundBerth"),Berth)||!TryReadRequiredAnchor(CL,TEXT("Helm"),Helm)||!TryReadRequiredAnchor(CL,TEXT("HarborExit"),Exit))return false;auto* Est=EnsureActor<AOpeningCrewRecruitmentActor>(Cove,TEXT("Crew_Esteban"),ELoc);if(!Est)return false;Est->CrewId=TEXT("crew.esteban");Est->DisplayName=FText::FromString(TEXT("Esteban"));auto* Ship=EnsureActor<ALaLiberacionShip>(Cove,TEXT("LaLiberacion"),Berth);if(!Ship||!PlaceMesh(Cove,TEXT("/Game/Alpha/Ships/LaLiberacion/SM_LaLiberacion_Alpha.SM_LaLiberacion_Alpha"),TEXT("LaLiberacionVisual"),Ship->GetActorLocation())||!EnsureActor<ALaLiberacionHelmInteractableActor>(Cove,TEXT("LaLiberacionHelm"),Helm)||!EnsureActor<AActor>(Cove,TEXT("GalleonHarborExit"),Exit)||!SaveWorld(Cove))return false;
 const FString WL=TEXT("ContentSource/World/OpenSea/FirstWake_Route.json");UWorld* Wake=OpenOrCreateWorld(TEXT("/Game/Alpha/Maps/L_OpenSea_FirstWake"));if(!PrepareGameplayWorld(Wake,WL,TEXT("GalleonCove.HarborExit")))return false;FVector RexaApproach;if(!TryReadRequiredAnchor(WL,TEXT("RexaHarbor.Approach"),RexaApproach))return false;auto* WS=EnsureActor<ALaLiberacionShip>(Wake,TEXT("LaLiberacion"),FVector::ZeroVector);if(!WS||!PlaceMesh(Wake,TEXT("/Game/Alpha/Ships/LaLiberacion/SM_LaLiberacion_Alpha.SM_LaLiberacion_Alpha"),TEXT("LaLiberacionVisual"),FVector::ZeroVector)||!EnsureActor<AActor>(Wake,TEXT("RexaApproach"),RexaApproach)||!SaveWorld(Wake))return false;
 const FString RL=TEXT("ContentSource/World/Rexa/Harbor/RexaHarbor_Layout.json");UWorld* Rexa=BuildSimpleMap(TEXT("L_RexaHarbor"),TEXT("/Game/Alpha/World/Rexa/Harbor/SM_RexaHarborArrival_Alpha.SM_RexaHarborArrival_Alpha"),RL,TEXT("DockEntry"));FVector Market,DockLoc,Arrival;if(!Rexa||!TryReadRequiredAnchor(RL,TEXT("HarborMarket"),Market)||!TryReadRequiredAnchor(RL,TEXT("DockEntry"),DockLoc)||!TryReadRequiredAnchor(RL,TEXT("ArrivalTrigger"),Arrival)||!EnsureActor<ARexaSettlementDirector>(Rexa,TEXT("RexaSettlementDirector"),Market))return false;auto* Dock=EnsureActor<ARexaSettlementAnchor>(Rexa,TEXT("RexaAnchor_DockEntry"),DockLoc);if(!Dock||!Dock->InitializeAuthoredAnchor(TEXT("Rexa.Harbor"),TEXT("Rexa.Anchor.DockEntry"),false,false)||!EnsureActor<AActor>(Rexa,TEXT("RexaArrivalTrigger"),Arrival))return false;return SaveWorld(Rexa);
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
        {TEXT("ContentSource/World/Moran/DriftwoodBeach/SM_HarlowWreckage_Alpha.gltf"), TEXT("/Game/Alpha/World/Moran/DriftwoodBeach")},
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
