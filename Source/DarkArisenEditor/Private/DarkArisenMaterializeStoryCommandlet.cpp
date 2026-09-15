#include "DarkArisenMaterializeStoryCommandlet.h"

#include "AlphaGameplayGameMode.h"
#include "AssetImportTask.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "Bosses/DravenVossBossCharacter.h"
#include "Bosses/DreamEthanBossCharacter.h"
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
#include "Misc/Paths.h"
#include "Modules/ModuleManager.h"
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
static FString Str(const TSharedPtr<FJsonObject>& O,const TCHAR* K){FString V;if(O.IsValid())O->TryGetStringField(K,V);return V;}
static FName Name(const TSharedPtr<FJsonObject>& O,const TCHAR* K){const FString V=Str(O,K);return V.IsEmpty()?NAME_None:FName(*V);}
static bool Bool(const TSharedPtr<FJsonObject>& O,const TCHAR* K,bool D=false){bool V=D;if(O.IsValid())O->TryGetBoolField(K,V);return V;}
static int32 Int(const TSharedPtr<FJsonObject>& O,const TCHAR* K){double V=0;return O.IsValid()&&O->TryGetNumberField(K,V)?FMath::Max(0,FMath::RoundToInt(V)):0;}

static bool ReadJson(const FString& Path,TSharedPtr<FJsonObject>& Out){FString T;return FFileHelper::LoadFileToString(T,*Path)&&FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(T),Out)&&Out.IsValid();}
static bool Vec(const TArray<TSharedPtr<FJsonValue>>* A,FVector& Out){if(!A||A->Num()!=3)return false;for(const auto& V:*A)if(!V.IsValid()||V->Type!=EJson::Number)return false;Out=FVector((*A)[0]->AsNumber(),(*A)[1]->AsNumber(),(*A)[2]->AsNumber());return !Out.ContainsNaN();}
static bool Anchor(const TSharedPtr<FJsonObject>& R,const FString& K,FVector& Out){const TSharedPtr<FJsonObject>* O=nullptr;if(!R.IsValid()||!R->TryGetObjectField(TEXT("anchors"),O)||!O||!O->IsValid())return false;const TArray<TSharedPtr<FJsonValue>>* A=nullptr;return (*O)->TryGetArrayField(K,A)&&Vec(A,Out);}

template<typename T> static T* Ensure(UWorld* W,FName N,const FVector& L){if(!W||N.IsNone())return nullptr;for(TActorIterator<T> It(W);It;++It){if(It->GetFName()==N){It->SetActorLocation(L);return *It;}}FActorSpawnParameters P;P.Name=N;P.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;return W->SpawnActor<T>(T::StaticClass(),L,FRotator::ZeroRotator,P);}

static UWorld* OpenOrCreate(const FString& MapId){const FString PkgName=TEXT("/Game/Alpha/Maps/")+MapId;const FString Obj=PkgName+TEXT(".")+MapId;if(UWorld* W=LoadObject<UWorld>(nullptr,*Obj))return W;UPackage* Pkg=CreatePackage(*PkgName);UWorldFactory* F=NewObject<UWorldFactory>();F->WorldType=EWorldType::Game;F->bCreateWorldPartition=false;F->bEnableWorldPartitionStreaming=false;F->bInformEngineOfWorld=false;UWorld* W=Cast<UWorld>(F->FactoryCreateNew(UWorld::StaticClass(),Pkg,*MapId,RF_Public|RF_Standalone,nullptr,GWarn));if(W)FAssetRegistryModule::AssetCreated(W);return W;}
static bool Save(UWorld* W){if(!W)return false;W->MarkPackageDirty();TArray<UPackage*> P{W->GetOutermost()};return UEditorLoadingAndSavingUtils::SavePackages(P,false);}

static UStaticMesh* ImportGeometry(IAssetTools& Tools,const FString& Obj){if(Obj.IsEmpty())return nullptr;if(UStaticMesh* M=LoadObject<UStaticMesh>(nullptr,*Obj))return M;FString Pkg,NameStr;if(!Obj.Split(TEXT("."),&Pkg,&NameStr)||NameStr.IsEmpty())return nullptr;const FString Dest=FPaths::GetPath(Pkg);TArray<FString> Matches;IFileManager::Get().FindFilesRecursive(Matches,*FPaths::Combine(FPaths::ProjectDir(),TEXT("ContentSource")),*(NameStr+TEXT(".gltf")),true,false,false);if(Matches.Num()!=1){UE_LOG(LogTemp,Error,TEXT("%s requires exactly one source glTF; found %d"),*NameStr,Matches.Num());return nullptr;}UAssetImportTask* T=NewObject<UAssetImportTask>();T->Filename=Matches[0];T->DestinationPath=Dest;T->bAutomated=true;T->bReplaceExisting=true;T->bSave=true;T->bAsync=false;TArray<UAssetImportTask*> Tasks{T};Tools.ImportAssetTasks(Tasks);if(UStaticMesh* M=LoadObject<UStaticMesh>(nullptr,*Obj))return M;for(UObject* I:T->GetObjects())if(UStaticMesh* C=Cast<UStaticMesh>(I)){TArray<FAssetRenameData> R;R.Emplace(TWeakObjectPtr<UObject>(C),Dest,NameStr);if(!Tools.RenameAssets(R))return nullptr;return LoadObject<UStaticMesh>(nullptr,*Obj);}return nullptr;}

static bool Geometry(IAssetTools& Tools,UWorld* W,const TSharedPtr<FJsonObject>& R){const TArray<TSharedPtr<FJsonValue>>* Specs=nullptr;if(R->TryGetArrayField(TEXT("geometry"),Specs)){for(const auto& V:*Specs){const auto S=V.IsValid()?V->AsObject():nullptr;FVector L;const FString N=Str(S,TEXT("name"));if(N.IsEmpty()||!Anchor(R,Str(S,TEXT("anchor")),L))return false;UStaticMesh* M=ImportGeometry(Tools,Str(S,TEXT("objectPath")));AStaticMeshActor* A=M?Ensure<AStaticMeshActor>(W,FName(*N),L):nullptr;if(!A)return false;A->GetStaticMeshComponent()->SetStaticMesh(M);}}if(R->TryGetArrayField(TEXT("physicalGeometry"),Specs)){UStaticMesh* Cube=LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Cube.Cube"));if(!Cube)return false;for(const auto& V:*Specs){const auto S=V.IsValid()?V->AsObject():nullptr;FVector L,Size;const TArray<TSharedPtr<FJsonValue>>* SV=nullptr;const FString N=Str(S,TEXT("name"));if(N.IsEmpty()||!Anchor(R,Str(S,TEXT("anchor")),L)||!S->TryGetArrayField(TEXT("size"),SV)||!Vec(SV,Size))return false;AStaticMeshActor* A=Ensure<AStaticMeshActor>(W,FName(*N),L);if(!A)return false;A->GetStaticMeshComponent()->SetStaticMesh(Cube);A->SetActorScale3D(Size/100.0);}}return true;}

static EColonialFaction Faction(const FString& V){if(V.Equals(TEXT("Albion"),ESearchCase::IgnoreCase))return EColonialFaction::Albion;if(V.Equals(TEXT("Liberation"),ESearchCase::IgnoreCase))return EColonialFaction::Liberation;if(V.Equals(TEXT("Crimson"),ESearchCase::IgnoreCase)||V.Equals(TEXT("CrimsonArmada"),ESearchCase::IgnoreCase))return EColonialFaction::Crimson;return EColonialFaction::Imperial;}
static EWarActionVerb Verb(const FString& V){if(V.Equals(TEXT("Support"),ESearchCase::IgnoreCase))return EWarActionVerb::Support;if(V.Equals(TEXT("Liberate"),ESearchCase::IgnoreCase))return EWarActionVerb::Liberate;return EWarActionVerb::Break;}
static AMainStoryHolderBossCharacter* Boss(UWorld* W,FName N,const FVector& L,FName Id){if(Id==TEXT("boss.dream_ethan"))return Ensure<ADreamEthanBossCharacter>(W,N,L);if(Id==TEXT("boss.draven_voss"))return Ensure<ADravenVossBossCharacter>(W,N,L);return Ensure<AMainStoryHolderBossCharacter>(W,N,L);}

static bool Actors(UWorld* W,const TSharedPtr<FJsonObject>& R){const FString MissionStr=Str(R,TEXT("missionId"));const FName Mission(*MissionStr);const FName RootCP=Name(R,TEXT("checkpointId"));const FName RootSpawn=Name(R,TEXT("spawnId"));const FString RootRegion=Str(R,TEXT("regionId"));const TArray<TSharedPtr<FJsonValue>>* Specs=nullptr;if(MissionStr.IsEmpty()||!R->TryGetArrayField(TEXT("actors"),Specs))return false;for(const auto& V:*Specs){const auto S=V.IsValid()?V->AsObject():nullptr;const FString Type=Str(S,TEXT("type"));const FString AN=Str(S,TEXT("name"));FVector L;if(!S.IsValid()||Type.IsEmpty()||AN.IsEmpty()||!Anchor(R,Str(S,TEXT("anchor")),L))return false;const FName Stable(*AN);
if(Type==TEXT("LocationTrigger")){auto* A=Ensure<AMainStoryLocationTriggerActor>(W,Stable,L);if(!A)return false;A->MissionId=Mission;A->Action=Str(S,TEXT("action"))==TEXT("CompleteMission")?EMainStoryLocationAction::CompleteMission:EMainStoryLocationAction::ActivateMission;A->CheckpointId=Name(S,TEXT("checkpointId"));if(A->CheckpointId.IsNone())A->CheckpointId=RootCP;A->SpawnId=Name(S,TEXT("spawnId"));if(A->SpawnId.IsNone())A->SpawnId=RootSpawn;}
else if(Type==TEXT("Contact")){auto* A=Ensure<AMainStoryContactActor>(W,Stable,L);if(!A)return false;A->MissionId=Mission;A->ContactId=Name(S,TEXT("contactId"));A->DialogueId=Name(S,TEXT("dialogueId"));A->DisplayName=FText::FromString(Str(S,TEXT("displayName")));A->DialogueText=FText::FromString(Str(S,TEXT("text")));A->bActivateMissionOnInteraction=Bool(S,TEXT("activatesMission"));A->bCompleteMissionOnInteraction=Bool(S,TEXT("completesMission"));A->CheckpointId=Name(S,TEXT("checkpointId"));if(A->CheckpointId.IsNone())A->CheckpointId=RootCP;A->SpawnId=Name(S,TEXT("spawnId"));if(A->SpawnId.IsNone())A->SpawnId=RootSpawn;}
else if(Type==TEXT("Evidence")){if(Bool(S,TEXT("completesMission"))){auto* A=Ensure<AMainStoryEvidenceActor>(W,Stable,L);if(!A)return false;A->MissionId=Mission;A->DocumentTitle=FText::FromString(Str(S,TEXT("title")));A->DocumentBody=FText::FromString(Str(S,TEXT("body")));A->CheckpointId=Name(S,TEXT("checkpointId"));if(A->CheckpointId.IsNone())A->CheckpointId=RootCP;A->SpawnId=Name(S,TEXT("spawnId"));if(A->SpawnId.IsNone())A->SpawnId=RootSpawn;A->RequiredBossId=Name(S,TEXT("requiredBossId"));}else{auto* A=Ensure<AExamineDocumentActor>(W,Stable,L);if(!A)return false;A->DocumentTitle=FText::FromString(Str(S,TEXT("title")));A->DocumentBody=FText::FromString(Str(S,TEXT("body")));}}
else if(Type==TEXT("RouteResolution")){auto* A=Ensure<AMainStoryRouteResolutionActor>(W,Stable,L);if(!A)return false;A->MissionId=Mission;A->OutcomeKey=Name(S,TEXT("outcomeKey"));A->OutcomeValue=Name(S,TEXT("outcomeValue"));A->InteractionLabel=FText::FromString(Str(S,TEXT("label")));}
else if(Type==TEXT("WarAction")){auto* A=Ensure<AMainStoryWarActionActor>(W,Stable,L);if(!A)return false;A->MissionId=Mission;const FString Reg=Str(S,TEXT("regionId"));A->RegionId=FName(*(Reg.IsEmpty()?RootRegion:Reg));A->InitialController=Faction(Str(S,TEXT("initialController")));A->Verb=Verb(Str(S,TEXT("verb")));A->TargetFaction=Faction(Str(S,TEXT("targetFaction")));A->ControlDelta=Int(S,TEXT("controlDelta"));A->LiberationDelta=Int(S,TEXT("liberationDelta"));A->CrimsonDelta=Int(S,TEXT("crimsonDelta"));A->OutcomeKey=Name(S,TEXT("outcomeKey"));A->OutcomeValue=Name(S,TEXT("outcomeValue"));A->InteractionLabel=FText::FromString(Str(S,TEXT("label")));}
else if(Type==TEXT("HolderBoss")){const FName Id=Name(S,TEXT("bossId"));auto* A=Boss(W,Stable,L,Id);if(!A)return false;A->MissionId=Mission;A->BossId=Id;A->OutcomeKey=Name(S,TEXT("outcomeKey"));A->OutcomeValue=Name(S,TEXT("outcomeValue"));A->bCompleteMissionOnDefeat=Bool(S,TEXT("completesMissionOnDefeat"),true);}
else{UE_LOG(LogTemp,Error,TEXT("Unknown story actor type %s in %s"),*Type,*MissionStr);return false;}}
return true;}

static bool Materialize(IAssetTools& Tools,const FString& Path){TSharedPtr<FJsonObject> R;if(!ReadJson(Path,R))return false;const FString Map=Str(R,TEXT("mapId"));if(Map.IsEmpty())return false;UWorld* W=OpenOrCreate(Map);if(!W)return false;if(AWorldSettings* S=W->GetWorldSettings())S->DefaultGameMode=AAlphaGameplayGameMode::StaticClass();Ensure<ADarkArisenWorldDirector>(W,TEXT("WorldDirector"),FVector::ZeroVector);FVector Entry=FVector::ZeroVector;Anchor(R,Str(R,TEXT("entryAnchor")),Entry);Ensure<APlayerStart>(W,TEXT("PlayerStart"),Entry+FVector(0,0,120));return Geometry(Tools,W,R)&&Actors(W,R)&&Save(W);}
}

UDarkArisenMaterializeStoryCommandlet::UDarkArisenMaterializeStoryCommandlet(){IsClient=false;IsServer=false;LogToConsole=true;}
int32 UDarkArisenMaterializeStoryCommandlet::Main(const FString& Params){if(FEngineVersion::Current().GetMajor()!=5||FEngineVersion::Current().GetMinor()!=8){UE_LOG(LogTemp,Error,TEXT("DarkArisenMaterializeStory requires Unreal Engine 5.8; current=%s"),*FEngineVersion::Current().ToString());return 2;}IAssetTools& Tools=FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools")).Get();TArray<FString> Contracts;IFileManager::Get().FindFilesRecursive(Contracts,*FPaths::Combine(FPaths::ProjectDir(),TEXT("ContentSource/Story")),TEXT("C*.json"),true,false,false);Contracts.Sort();if(Contracts.IsEmpty())return 3;int32 Failures=0;for(const FString& C:Contracts)if(!DarkArisenStoryMaterialize::Materialize(Tools,C)){++Failures;UE_LOG(LogTemp,Error,TEXT("Story materialisation failed: %s"),*C);}UE_LOG(LogTemp,Display,TEXT("Story materialisation complete: %d contracts, %d failures"),Contracts.Num(),Failures);return Failures==0?0:4;}
