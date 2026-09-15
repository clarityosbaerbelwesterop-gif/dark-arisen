#include "Story/CreditsPresentationActor.h"

#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "Components/TextRenderComponent.h"
#include "GameFramework/PlayerController.h"
#include "InputCoreTypes.h"
#include "Kismet/GameplayStatics.h"

ACreditsPresentationActor::ACreditsPresentationActor()
{
    PrimaryActorTick.bCanEverTick = true;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(Root);
    Camera->SetRelativeLocation(FVector::ZeroVector);
    Camera->SetRelativeRotation(FRotator::ZeroRotator);

    CreditsText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("CreditsText"));
    CreditsText->SetupAttachment(Root);
    CreditsText->SetRelativeLocation(FVector(900.f, 0.f, -450.f));
    CreditsText->SetRelativeRotation(FRotator(0.f, 180.f, 0.f));
    CreditsText->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
    CreditsText->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextTop);
    CreditsText->SetWorldSize(34.f);
}

void ACreditsPresentationActor::BeginPlay()
{
    Super::BeginPlay();
    CreditsText->SetText(RollText);
    InitialTextZ = CreditsText->GetRelativeLocation().Z;

    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        PC->SetViewTarget(this);
        PC->bShowMouseCursor = false;
    }
}

void ACreditsPresentationActor::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (bFinished) return;

    ElapsedSeconds += FMath::Max(0.f, DeltaSeconds);
    const float Alpha = FMath::Clamp(ElapsedSeconds / FMath::Max(1.f, RollDurationSeconds), 0.f, 1.f);
    FVector Location = CreditsText->GetRelativeLocation();
    Location.Z = FMath::Lerp(InitialTextZ, InitialTextZ + 1850.f, Alpha);
    CreditsText->SetRelativeLocation(Location);

    bool bSkip = false;
    if (ElapsedSeconds >= AllowSkipAfterSeconds)
    {
        if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
        {
            bSkip = PC->WasInputKeyJustPressed(EKeys::Escape)
                || PC->WasInputKeyJustPressed(EKeys::Enter)
                || PC->WasInputKeyJustPressed(EKeys::SpaceBar)
                || PC->WasInputKeyJustPressed(EKeys::Gamepad_FaceButton_Bottom);
        }
    }

    if (Alpha >= 1.f || bSkip) FinishCredits();
}

void ACreditsPresentationActor::FinishCredits()
{
    if (bFinished) return;
    bFinished = true;
    if (!ReturnMap.IsNone()) UGameplayStatics::OpenLevel(this, ReturnMap);
}
