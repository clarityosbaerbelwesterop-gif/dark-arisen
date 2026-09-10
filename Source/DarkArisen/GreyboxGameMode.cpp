// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "GreyboxGameMode.h"

#include "Components/DirectionalLightComponent.h"
#include "Components/MainStoryDirectorComponent.h"
#include "Components/SkyLightComponent.h"
#include "DarkArisen.h"
#include "DuelingEnemyCharacter.h"
#include "Engine/DirectionalLight.h"
#include "Engine/SkyLight.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "FrameTimeTelemetryActor.h"
#include "JakeCharacter.h"
#include "Interaction/ExamineDocumentActor.h"
#include "Interaction/PhysicalJournalActor.h"
#include "Interaction/PhysicalDoorActor.h"
#include "Interaction/PhysicalPickupActor.h"
#include "Missions/MainStoryMissionCatalog.h"
#include "PostureOnlyHUD.h"

namespace
{
    AStaticMeshActor* SpawnCube(
        UWorld* World,
        UStaticMesh* CubeMesh,
        const FName Name,
        const FVector& Location,
        const FVector& Scale)
    {
        FActorSpawnParameters Parameters;
        Parameters.Name = Name;
        Parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        AStaticMeshActor* Cube = World->SpawnActor<AStaticMeshActor>(Location, FRotator::ZeroRotator, Parameters);
        if (Cube)
        {
            Cube->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
            Cube->GetStaticMeshComponent()->SetStaticMesh(CubeMesh);
            Cube->SetActorScale3D(Scale);
        }
        return Cube;
    }
}

AGreyboxGameMode::AGreyboxGameMode()
{
    DefaultPawnClass = AJakeCharacter::StaticClass();
    HUDClass = APostureOnlyHUD::StaticClass();
    MainStoryDirectorComponent =
        CreateDefaultSubobject<UMainStoryDirectorComponent>(TEXT("MainStoryDirectorComponent"));
}

void AGreyboxGameMode::InitGame(
    const FString& MapName,
    const FString& Options,
    FString& ErrorMessage)
{
    Super::InitGame(MapName, Options, ErrorMessage);
    BuildGreybox();
}

void AGreyboxGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
    if (NewPlayer && !NewPlayer->GetPawn())
    {
        RestartPlayerAtTransform(
            NewPlayer,
            FTransform(FRotator::ZeroRotator, FVector(0.0f, 0.0f, 120.0f)));
    }

    AJakeCharacter* Jake = NewPlayer ? Cast<AJakeCharacter>(NewPlayer->GetPawn()) : nullptr;
    if (!Jake || !Jake->QuestJournalComponent ||
        !UMainStoryMissionCatalog::RegisterAuthoredMissions(Jake->QuestJournalComponent))
    {
        UE_LOG(LogDarkArisen, Error, TEXT("Current main-story mission catalog failed closed."));
    }
}

void AGreyboxGameMode::StartPlay()
{
    Super::StartPlay();
    UE_LOG(LogDarkArisen, Display,
        TEXT("Native preview ready: 60 fps gameplay target with finite current story authority."));
}

void AGreyboxGameMode::BuildGreybox()
{
    UWorld* World = GetWorld();
    UStaticMesh* CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (!World || !CubeMesh)
    {
        UE_LOG(LogDarkArisen, Error, TEXT("Unable to construct the native preview room."));
        return;
    }

    SpawnCube(World, CubeMesh, TEXT("GreyboxFloor"), FVector(0.0f, 0.0f, -50.0f), FVector(20.0f, 20.0f, 1.0f));
    SpawnCube(World, CubeMesh, TEXT("GreyboxWallNorth"), FVector(0.0f, 1000.0f, 200.0f), FVector(20.0f, 1.0f, 5.0f));
    SpawnCube(World, CubeMesh, TEXT("GreyboxWallSouth"), FVector(0.0f, -1000.0f, 200.0f), FVector(20.0f, 1.0f, 5.0f));
    SpawnCube(World, CubeMesh, TEXT("GreyboxWallEast"), FVector(1000.0f, 0.0f, 200.0f), FVector(1.0f, 20.0f, 5.0f));
    SpawnCube(World, CubeMesh, TEXT("GreyboxWallWest"), FVector(-1000.0f, 0.0f, 200.0f), FVector(1.0f, 20.0f, 5.0f));
    SpawnCube(World, CubeMesh, TEXT("GreyboxStepLow"), FVector(420.0f, 300.0f, 25.0f), FVector(3.0f, 3.0f, 0.5f));
    SpawnCube(World, CubeMesh, TEXT("GreyboxStepHigh"), FVector(720.0f, 300.0f, 75.0f), FVector(3.0f, 3.0f, 1.5f));

    ADirectionalLight* Sun = World->SpawnActor<ADirectionalLight>(
        FVector::ZeroVector,
        FRotator(-45.0f, -35.0f, 0.0f));
    if (Sun)
    {
        Sun->GetLightComponent()->SetIntensity(5.0f);
    }

    ASkyLight* Sky = World->SpawnActor<ASkyLight>();
    if (Sky)
    {
        Sky->GetLightComponent()->SetIntensity(1.0f);
        Sky->GetLightComponent()->RecaptureSky();
    }

    World->SpawnActor<AFrameTimeTelemetryActor>();
    World->SpawnActor<ADuelingEnemyCharacter>(
        FVector(650.0f, 0.0f, 120.0f),
        FRotator(0.0f, 180.0f, 0.0f));
    World->SpawnActor<APhysicalDoorActor>(
        FVector(-420.0f, 120.0f, 0.0f),
        FRotator::ZeroRotator);
    World->SpawnActor<APhysicalPickupActor>(
        FVector(260.0f, -220.0f, 35.0f),
        FRotator::ZeroRotator);
    World->SpawnActor<AExamineDocumentActor>(
        FVector(100.0f, 280.0f, 22.0f),
        FRotator(0.0f, 0.0f, 8.0f));
    World->SpawnActor<APhysicalJournalActor>(
        FVector(-120.0f, 280.0f, 22.0f),
        FRotator(0.0f, 0.0f, 8.0f));
}
