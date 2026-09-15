#include "DarkArisenMaterializeStoryCommandlet.h"

#include "AlphaGameplayGameMode.h"
#include "AssetImportTask.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "Bosses/MainStoryHolderBossCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Dom/JsonObject.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Factories/WorldFactory.h"
#include "FileHelpers.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/WorldSettings.h"
#include "HAL/FileManager.h"
#include "Interaction/ExamineDocumentActor.h"
#include "IAssetTools.h"
#include "Misc/EngineVersion.h"
#include "Misc/FileHelper.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Story/DarkArisenWorldDirector.h"
#include "Story/MainStoryContactActor.h"
#include "Story/MainStoryEvidenceActor.h"
#include "Story/MainStoryLocationTriggerActor.h"
#include "Story/MainStoryRouteResolutionActor.h"
#include "Story/MainStoryWarActionActor.h"
#include "UObject/Package.h"

namespace DarkArisenStoryMaterialize
{
static bool ReadJson(const FString& Path, TSharedPtr<FJsonObject>& Out)
{
    FString Text;
    if (!FFileHelper::LoadFileToString(Text, *Path))
    {
        UE_LOG(LogTemp, Error, TEXT("Story materialiser cannot read %s"), *Path);
        return false;
    }
    return FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Text), Out) && Out.IsValid();
}

static FString OptionalString(const TSharedPtr<FJsonObject>& Object, const TCHAR* Field)
{
    FString Value;
    if (Object.IsValid()) Object->TryGetStringField(Field, Value);
    return Value;
}

static FName OptionalName(const TSharedPtr<FJsonObject>& Object, const TCHAR* Field)
{
    const FString Value = OptionalString(Object, Field);
    return Value.IsEmpty() ? NAME_None : FName(*Value);
}

static bool ReadVector(const TArray<TSharedPtr<FJsonValue>>* Values, FVector& Out)
{
    if (!Values || Values->Num() != 3) return false;
    for (const TSharedPtr<FJsonValue>& Value : *Values)
        if (!Value.IsValid() || Value->Type != EJson::Number) return false;
    Out = FVector((*Values)[0]->AsNumber(), (*Values)[1]->AsNumber(), (*Values)[2]->AsNumber());
    return !Out.ContainsNaN();
}

static bool ReadAnchor(const TSharedPtr<FJsonObject>& Root, const FString& Anchor, FVector& Out)
{
    const TSharedPtr<FJsonObject>* Anchors = nullptr;
    if (!Root.IsValid() || !Root->TryGetObjectField(TEXT("anchors"), Anchors) || !Anchors || !Anchors->IsValid()) return false;
    const TArray<TSharedPtr<FJsonValue>>* Values = nullptr;
    return (*Anchors)->TryGetArrayField(Anchor, Values) && ReadVector(Values, Out);
}

static bool ReadSize(const TSharedPtr<FJsonObject>& Spec, FVector& Out)
{
    const TArray<TSharedPtr<FJsonValue>>* Values = nullptr;
    return Spec.IsValid() && Spec->TryGetArrayField(TEXT("size"), Values) && ReadVector(Values, Out)
        && Out.X > 0.0 && Out.Y > 0.0 && Out.Z > 0.0;
}

template<typename T>
static T* EnsureActor(UWorld* World, const FName Name, const FVector& Location)
{
    if (!World || Name.IsNone()) return nullptr;
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

static UWorld* OpenOrCreateWorld(const FString& MapId)
{
    const FString PackageName = TEXT("/Game/Alpha/Maps/") + MapId;
    const FString ObjectPath = PackageName + TEXT(".") + MapId;
    if (UWorld* Existing = LoadObject<UWorld>(nullptr, *ObjectPath)) return Existing;

    UPackage* Package = CreatePackage(*PackageName);
    UWorldFactory* Factory = NewObject<UWorldFactory>();
    Factory->WorldType = EWorldType::Game;
    Factory->bCreateWorldPartition = false;
    Factory->bEnableWorldPartitionStreaming = false;
    Factory->bInformEngineOfWorld = false;
    UWorld* World = Cast<UWorld>(Factory->FactoryCreateNew(UWorld::StaticClass(), Package, *MapId,
        RF_Public | RF_Standalone, nullptr, GWarn));
    if (World) FAssetRegistryModule::AssetCreated(World);
    return World;
}

static bool SaveWorld(UWorld* World)
{
    if (!World) return false;
    World->MarkPackageDirty();
    TArray<UPackage*> Packages{World->GetOutermost()};
    return UEditorLoadingAndSavingUtils::SavePackages(Packages, false);
}

static UStaticMesh* ImportReferencedGeometry(IAssetTools& AssetTools, const FString& ObjectPath)
{
    if (ObjectPath.IsEmpty()) return nullptr;
    if (UStaticMesh* Existing = LoadObject<UStaticMesh>(nullptr, *ObjectPath)) return Existing;

    FString PackagePath;
    FString AssetName;
    if (!ObjectPath.Split(TEXT("."), &PackagePath, &AssetName) || AssetName.IsEmpty()) return nullptr;
    const FString Destination = FPaths::GetPath(PackagePath);

    TArray<FString> Matches;
    const FString SourceRoot = FPaths::Combine(FPaths::ProjectDir(), TEXT("ContentSource"));
    IFileManager::Get().FindFilesRecursive(Matches, *SourceRoot, *(AssetName + TEXT(".gltf")), true, false, false);
    if (Matches.Num() != 1)
    {
        UE_LOG(LogTemp, Error, TEXT("Story geometry %s requires exactly one ContentSource glTF; found %d"), *AssetName, Matches.Num());
        return nullptr;
    }

    UAssetImportTask* Task = NewObject<UAssetImportTask>();
    Task->Filename = Matches[0];
    Task->DestinationPath = Destination;
    Task->bAutomated = true;
    Task->bReplaceExisting = true;
    Task->bReplaceExistingSettings = false;
    Task->bSave = true;
    Task->bAsync = false;
    TArray<UAssetImportTask*> Tasks{Task};
    AssetTools.ImportAssetTasks(Tasks);

    UStaticMesh* Mesh = LoadObject<UStaticMesh>(nullptr, *ObjectPath);
    if (Mesh) return Mesh;
    for (UObject* Imported : Task->GetObjects())
    {
        if (UStaticMesh* Candidate = Cast<UStaticMesh>(Imported))
        {
            TArray<FAssetRenameData> Rename;
            Rename.Emplace(TWeakObjectPtr<UObject>(Candidate), Destination, AssetName);
            if (!AssetTools.RenameAssets(Rename)) return nullptr;
            return LoadObject<UStaticMesh>(nullptr, *ObjectPath);
        }
    }
    return nullptr;
}

static bool PlaceReferencedGeometry(IAssetTools& AssetTools, UWorld* World, const TSharedPtr<FJsonObject>& Root)
{
    const TArray<TSharedPtr<FJsonValue>>* Geometry = nullptr;
    if (!Root->TryGetArrayField(TEXT("geometry"), Geometry)) return true;
    for (const TSharedPtr<FJsonValue>& Value : *Geometry)
    {
        const TSharedPtr<FJsonObject> Spec = Value.IsValid() ? Value->AsObject() : nullptr;
        if (!Spec.IsValid()) return false;
        const FString Name = OptionalString(Spec, TEXT("name"));
        const FString ObjectPath = OptionalString(Spec, TEXT("objectPath"));
        const FString AnchorName = OptionalString(Spec, TEXT("anchor"));
        FVector Location;
        if (Name.IsEmpty() || ObjectPath.IsEmpty() || !ReadAnchor(Root, AnchorName, Location)) return false;
        UStaticMesh* Mesh = ImportReferencedGeometry(AssetTools, ObjectPath);
        AStaticMeshActor* Actor = Mesh ? EnsureActor<AStaticMeshActor>(World, FName(*Name), Location) : nullptr;
        if (!Actor) return false;
        Actor->GetStaticMeshComponent()->SetStaticMesh(Mesh);
    }
    return true;
}

static bool PlacePhysicalGeometry(UWorld* World, const TSharedPtr<FJsonObject>& Root)
{
    const TArray<TSharedPtr<FJsonValue>>* Geometry = nullptr;
    if (!Root->TryGetArrayField(TEXT("physicalGeometry"), Geometry)) return true;
    UStaticMesh* Cube = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (!Cube) return false;
    for (const TSharedPtr<FJsonValue>& Value : *Geometry)
    {
        const TSharedPtr<FJsonObject> Spec = Value.IsValid() ? Value->AsObject() : nullptr;
        FVector Location, Size;
        const FString Name = OptionalString(Spec, TEXT("name"));
        const FString AnchorName = OptionalString(Spec, TEXT("anchor"));
        if (Name.IsEmpty() || !ReadAnchor(Root, AnchorName, Location) || !ReadSize(Spec, Size)) return false;
        AStaticMeshActor* Actor = EnsureActor<AStaticMeshActor>(World, FName(*Name), Location);
        if (!Actor) return false;
        Actor->GetStaticMeshComponent()->SetStaticMesh(Cube);
        Actor->SetActorScale3D(Size / 100.0);
    }
    return true;
}

static EColonialFaction ParseFaction(const FString& Value)
{
    if (Value.Equals(TEXT("Albion"), ESearchCase::IgnoreCase)) return EColonialFaction::Albion;
    if (Value.Equals(TEXT("Liberation"), ESearchCase::IgnoreCase)) return EColonialFaction::Liberation;
    if (Value.Equals(TEXT("Crimson"), ESearchCase::IgnoreCase)) return EColonialFaction::Crimson;
    return EColonialFaction::Imperial;
}

static EWarActionVerb ParseWarVerb(const FString& Value)
{
    if (Value.Equals(TEXT("Support"), ESearchCase::IgnoreCase)) return EWarActionVerb::Support;
    if (Value.Equals(TEXT("Liberate"), ESearchCase::IgnoreCase)) return EWarActionVerb::Liberate;
    return EWarActionVerb::Break;
}

static int32 OptionalInt(const TSharedPtr<FJsonObject>& Object, const TCHAR* Field)
{
    double Number = 0.0;
    return Object.IsValid() && Object->TryGetNumberField(Field, Number) ? FMath::Max(0, FMath::RoundToInt(Number)) : 0;
}

static bool SpawnStoryActors(UWorld* World, const TSharedPtr<FJsonObject>& Root)
{
    const FString MissionString = OptionalString(Root, TEXT("missionId"));
    const FName MissionId(*MissionString);
    const FName RootCheckpoint = OptionalName(Root, TEXT("checkpointId"));
    const FName RootSpawn = OptionalName(Root, TEXT("spawnId"));
    const FString RootRegion = OptionalString(Root, TEXT("regionId"));
    const TArray<TSharedPtr<FJsonValue>>* Actors = nullptr;
    if (MissionString.IsEmpty() || !Root->TryGetArrayField(TEXT("actors"), Actors)) return false;

    for (const TSharedPtr<FJsonValue>& Value : *Actors)
    {
        const TSharedPtr<FJsonObject> Spec = Value.IsValid() ? Value->AsObject() : nullptr;
        const FString Type = OptionalString(Spec, TEXT("type"));
        const FString ActorName = OptionalString(Spec, TEXT("name"));
        const FString AnchorName = OptionalString(Spec, TEXT("anchor"));
        FVector Location;
        if (!Spec.IsValid() || Type.IsEmpty() || ActorName.IsEmpty() || !ReadAnchor(Root, AnchorName, Location)) return false;
        const FName StableName(*ActorName);

        if (Type == TEXT("LocationTrigger"))
        {
            AMainStoryLocationTriggerActor* Actor = EnsureActor<AMainStoryLocationTriggerActor>(World, StableName, Location);
            if (!Actor) return false;
            Actor->MissionId = MissionId;
            Actor->Action = OptionalString(Spec, TEXT("action")) == TEXT("CompleteMission")
                ? EMainStoryLocationAction::CompleteMission : EMainStoryLocationAction::ActivateMission;
            Actor->CheckpointId = OptionalName(Spec, TEXT("checkpointId"));
            Actor->SpawnId = OptionalName(Spec, TEXT("spawnId"));
            if (Actor->CheckpointId.IsNone()) Actor->CheckpointId = RootCheckpoint;
            if (Actor->SpawnId.IsNone()) Actor->SpawnId = RootSpawn;
        }
        else if (Type == TEXT("Contact"))
        {
            AMainStoryContactActor* Actor = EnsureActor<AMainStoryContactActor>(World, StableName, Location);
            if (!Actor) return false;
            Actor->MissionId = MissionId;
            Actor->ContactId = OptionalName(Spec, TEXT("contactId"));
            Actor->DialogueId = OptionalName(Spec, TEXT("dialogueId"));
            Actor->DisplayName = FText::FromString(OptionalString(Spec, TEXT("displayName")));
            Actor->DialogueText = FText::FromString(OptionalString(Spec, TEXT("text")));
            bool bActivate = false;
            Spec->TryGetBoolField(TEXT("activatesMission"), bActivate);
            Actor->bActivateMissionOnInteraction = bActivate;
        }
        else if (Type == TEXT("Evidence"))
        {
            bool bCompletes = false;
            Spec->TryGetBoolField(TEXT("completesMission"), bCompletes);
            if (bCompletes)
            {
                AMainStoryEvidenceActor* Actor = EnsureActor<AMainStoryEvidenceActor>(World, StableName, Location);
                if (!Actor) return false;
                Actor->MissionId = MissionId;
                Actor->DocumentTitle = FText::FromString(OptionalString(Spec, TEXT("title")));
                Actor->DocumentBody = FText::FromString(OptionalString(Spec, TEXT("body")));
                Actor->CheckpointId = OptionalName(Spec, TEXT("checkpointId"));
                Actor->SpawnId = OptionalName(Spec, TEXT("spawnId"));
                Actor->RequiredBossId = OptionalName(Spec, TEXT("requiredBossId"));
            }
            else
            {
                AExamineDocumentActor* Actor = EnsureActor<AExamineDocumentActor>(World, StableName, Location);
                if (!Actor) return false;
                Actor->DocumentTitle = FText::FromString(OptionalString(Spec, TEXT("title")));
                Actor->DocumentBody = FText::FromString(OptionalString(Spec, TEXT("body")));
            }
        }
        else if (Type == TEXT("RouteResolution"))
        {
            AMainStoryRouteResolutionActor* Actor = EnsureActor<AMainStoryRouteResolutionActor>(World, StableName, Location);
            if (!Actor) return false;
            Actor->MissionId = MissionId;
            Actor->OutcomeKey = OptionalName(Spec, TEXT("outcomeKey"));
            Actor->OutcomeValue = OptionalName(Spec, TEXT("outcomeValue"));
            Actor->InteractionLabel = FText::FromString(OptionalString(Spec, TEXT("label")));
        }
        else if (Type == TEXT("WarAction"))
        {
            AMainStoryWarActionActor* Actor = EnsureActor<AMainStoryWarActionActor>(World, StableName, Location);
            if (!Actor) return false;
            Actor->MissionId = MissionId;
            const FString Region = OptionalString(Spec, TEXT("regionId"));
            Actor->RegionId = FName(*(Region.IsEmpty() ? RootRegion : Region));
            Actor->InitialController = ParseFaction(OptionalString(Spec, TEXT("initialController")));
            Actor->Verb = ParseWarVerb(OptionalString(Spec, TEXT("verb")));
            Actor->TargetFaction = ParseFaction(OptionalString(Spec, TEXT("targetFaction")));
            Actor->ControlDelta = OptionalInt(Spec, TEXT("controlDelta"));
            Actor->LiberationDelta = OptionalInt(Spec, TEXT("liberationDelta"));
            Actor->CrimsonDelta = OptionalInt(Spec, TEXT("crimsonDelta"));
            Actor->OutcomeKey = OptionalName(Spec, TEXT("outcomeKey"));
            Actor->OutcomeValue = OptionalName(Spec, TEXT("outcomeValue"));
            Actor->InteractionLabel = FText::FromString(OptionalString(Spec, TEXT("label")));
        }
        else if (Type == TEXT("HolderBoss"))
        {
            AMainStoryHolderBossCharacter* Actor = EnsureActor<AMainStoryHolderBossCharacter>(World, StableName, Location);
            if (!Actor) return false;
            Actor->MissionId = MissionId;
            Actor->BossId = OptionalName(Spec, TEXT("bossId"));
            Actor->OutcomeKey = OptionalName(Spec, TEXT("outcomeKey"));
            const FName OutcomeValue = OptionalName(Spec, TEXT("outcomeValue"));
            if (!OutcomeValue.IsNone()) Actor->OutcomeValue = OutcomeValue;
            bool bComplete = true;
            Spec->TryGetBoolField(TEXT("completesMissionOnDefeat"), bComplete);
            Actor->bCompleteMissionOnDefeat = bComplete;
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Unsupported story actor type '%s' in %s"), *Type, *MissionString);
            return false;
        }
    }
    return true;
}

static bool MaterializeMission(IAssetTools& AssetTools, const FString& SourcePath)
{
    TSharedPtr<FJsonObject> Root;
    if (!ReadJson(SourcePath, Root)) return false;
    const FString MapId = OptionalString(Root, TEXT("mapId"));
    const FString EntryAnchor = OptionalString(Root, TEXT("entryAnchor"));
    if (MapId.IsEmpty() || EntryAnchor.IsEmpty()) return false;

    FVector StartLocation;
    if (!ReadAnchor(Root, EntryAnchor, StartLocation)) return false;
    UWorld* World = OpenOrCreateWorld(MapId);
    if (!World) return false;
    World->GetWorldSettings()->DefaultGameMode = AAlphaGameplayGameMode::StaticClass();
    if (!EnsureActor<ADarkArisenWorldDirector>(World, TEXT("DarkArisenWorldDirector"), FVector::ZeroVector)
        || !EnsureActor<APlayerStart>(World, TEXT("PlayerStart"), StartLocation)) return false;
    if (!PlaceReferencedGeometry(AssetTools, World, Root)
        || !PlacePhysicalGeometry(World, Root)
        || !SpawnStoryActors(World, Root)
        || !SaveWorld(World)) return false;

    UE_LOG(LogTemp, Display, TEXT("Materialised story mission %s -> %s"),
        *OptionalString(Root, TEXT("missionId")), *MapId);
    return true;
}
}

UDarkArisenMaterializeStoryCommandlet::UDarkArisenMaterializeStoryCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
    ShowErrorCount = true;
    UseCommandletResultAsExitCode = true;
}

int32 UDarkArisenMaterializeStoryCommandlet::Main(const FString& Params)
{
    using namespace DarkArisenStoryMaterialize;
    const FEngineVersion Version = FEngineVersion::Current();
    if (Version.GetMajor() != 5 || Version.GetMinor() != 8)
    {
        UE_LOG(LogTemp, Error, TEXT("Story materialisation requires UE 5.8; got %s"), *Version.ToString());
        return 10;
    }

    TArray<FString> MissionFiles;
    const FString StoryRoot = FPaths::Combine(FPaths::ProjectDir(), TEXT("ContentSource/Story"));
    IFileManager::Get().FindFilesRecursive(MissionFiles, *StoryRoot, TEXT("C*.json"), true, false, false);
    MissionFiles.Sort();
    if (MissionFiles.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("No authored physical story mission contracts found under %s"), *StoryRoot);
        return 20;
    }

    IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools")).Get();
    for (const FString& MissionFile : MissionFiles)
        if (!MaterializeMission(AssetTools, MissionFile)) return 30;

    if (!UEditorLoadingAndSavingUtils::SaveDirtyPackages(true, true)) return 40;
    UE_LOG(LogTemp, Display, TEXT("Materialised %d authored physical story mission contracts."), MissionFiles.Num());
    return 0;
}
