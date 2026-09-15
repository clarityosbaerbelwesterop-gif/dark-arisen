#include "DarkArisenMaterializeNavalCommandlet.h"
#include "Dom/JsonObject.h"
#include "EngineUtils.h"
#include "FileHelpers.h"
#include "HAL/FileManager.h"
#include "Misc/EngineVersion.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Ship/HostileNavalShip.h"
#include "Ship/LaLiberacionShip.h"
#include "Ship/NavalMissionGateActor.h"

template<typename T> static T* NavalEnsure(UWorld* W,FName N,const FVector& L){for(TActorIterator<T> It(W);It;++It)if(It->GetFName()==N){It->SetActorLocation(L);return *It;}FActorSpawnParameters P;P.Name=N;P.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;return W->SpawnActor<T>(T::StaticClass(),L,FRotator::ZeroRotator,P);}
static bool V3(const TSharedPtr<FJsonObject>& O,const TCHAR* K,FVector& Out){const TArray<TSharedPtr<FJsonValue>>* A=nullptr;if(!O->TryGetArrayField(K,A)||!A||A->Num()!=3)return false;Out=FVector((*A)[0]->AsNumber(),(*A)[1]->AsNumber(),(*A)[2]->AsNumber());return !Out.ContainsNaN();}
static FString S(const TSharedPtr<FJsonObject>& O,const TCHAR* K){FString V;if(O)O->TryGetStringField(K,V);return V;}
UDarkArisenMaterializeNavalCommandlet::UDarkArisenMaterializeNavalCommandlet(){IsClient=false;IsServer=false;LogToConsole=true;}
int32 UDarkArisenMaterializeNavalCommandlet::Main(const FString&)
{
    if(FEngineVersion::Current().GetMajor()!=5||FEngineVersion::Current().GetMinor()!=8)return 2;
    TArray<FString> Files;IFileManager::Get().FindFilesRecursive(Files,*FPaths::Combine(FPaths::ProjectDir(),TEXT("ContentSource/Naval")),TEXT("*.naval.json"),true,false,false);if(Files.IsEmpty())return 3;
    int32 Failures=0;
    for(const FString& Path:Files)
    {
        FString Text;TSharedPtr<FJsonObject> Root;if(!FFileHelper::LoadFileToString(Text,*Path)||!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Text),Root)||!Root){++Failures;continue;}
        const FString MapId=S(Root,TEXT("mapId"));const FString Mission=S(Root,TEXT("missionId"));UWorld* World=LoadObject<UWorld>(nullptr,*FString::Printf(TEXT("/Game/Alpha/Maps/%s.%s"),*MapId,*MapId));if(!World||Mission.IsEmpty()){++Failures;continue;}
        const TSharedPtr<FJsonObject>* Player=nullptr;if(!Root->TryGetObjectField(TEXT("playerShip"),Player)||!Player||!Player->IsValid()){++Failures;continue;}FVector L;if(!V3(*Player,TEXT("location"),L)||!NavalEnsure<ALaLiberacionShip>(World,FName(*S(*Player,TEXT("name"))),L)){++Failures;continue;}
        const TArray<TSharedPtr<FJsonValue>>* Hostiles=nullptr;if(!Root->TryGetArrayField(TEXT("hostiles"),Hostiles)||!Hostiles){++Failures;continue;}bool Bad=false;for(const auto& V:*Hostiles){auto O=V->AsObject();if(!O||!V3(O,TEXT("location"),L)||!NavalEnsure<AHostileNavalShip>(World,FName(*S(O,TEXT("name"))),L)){Bad=true;break;}}if(Bad){++Failures;continue;}
        const TSharedPtr<FJsonObject>* GateSpec=nullptr;if(!Root->TryGetObjectField(TEXT("completionGate"),GateSpec)||!GateSpec||!GateSpec->IsValid()||!V3(*GateSpec,TEXT("location"),L)){++Failures;continue;}ANavalMissionGateActor* Gate=NavalEnsure<ANavalMissionGateActor>(World,FName(*S(*GateSpec,TEXT("name"))),L);if(!Gate){++Failures;continue;}Gate->MissionId=FName(*Mission);Gate->CheckpointId=FName(*S(*GateSpec,TEXT("checkpointId")));Gate->SpawnId=FName(*S(*GateSpec,TEXT("spawnId")));double Required=1;(*GateSpec)->TryGetNumberField(TEXT("requiredSunkShips"),Required);Gate->RequiredSunkShips=FMath::Max(1,FMath::RoundToInt(Required));
        World->MarkPackageDirty();TArray<UPackage*> Packages{World->GetOutermost()};if(!UEditorLoadingAndSavingUtils::SavePackages(Packages,false))++Failures;
    }
    return Failures==0?0:4;
}
