#include "Story/MainStoryWarActionActor.h"

#include "Components/StaticMeshComponent.h"
#include "Story/MainStorySubsystem.h"
#include "UObject/ConstructorHelpers.h"

AMainStoryWarActionActor::AMainStoryWarActionActor()
{
    PrimaryActorTick.bCanEverTick = false;
    Visual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Visual"));
    SetRootComponent(Visual);
    Visual->SetRelativeScale3D(FVector(0.6f, 0.6f, 0.9f));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Mesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (Mesh.Succeeded()) Visual->SetStaticMesh(Mesh.Object);
    InteractionLabel = NSLOCTEXT("DarkArisenStory", "ResolveWarAction", "Commit to the action");
}

bool AMainStoryWarActionActor::CanInteract_Implementation(AActor* Interactor) const
{
    if (!IsValid(Interactor) || MissionId.IsNone() || RegionId.IsNone() || OutcomeKey.IsNone() ||
        OutcomeValue.IsNone() || !GetGameInstance()) return false;
    const UMainStorySubsystem* Story = GetGameInstance()->GetSubsystem<UMainStorySubsystem>();
    return Story && Story->GetMissionState(MissionId) == EMainMissionState::Active;
}

FText AMainStoryWarActionActor::GetInteractionLabel_Implementation() const
{
    return InteractionLabel;
}

void AMainStoryWarActionActor::CompleteInteraction_Implementation(AActor* Interactor)
{
    if (!CanInteract_Implementation(Interactor) || !GetWorld()) return;

    UMainStorySubsystem* Story = GetGameInstance()->GetSubsystem<UMainStorySubsystem>();
    UColonialWarStateSubsystem* War = GetWorld()->GetSubsystem<UColonialWarStateSubsystem>();
    if (!Story || !War) return;

    const FRegionalWarSnapshot Existing = War->GetRegionSnapshot(RegionId);
    if (Existing.RegionId.IsNone() && !War->RegisterRegion(RegionId, InitialController)) return;
    if (!War->RecordResolvedWarAction(RegionId, Verb, TargetFaction, ControlDelta, LiberationDelta, CrimsonDelta)) return;
    if (!Story->SetQuestOutcome(OutcomeKey, OutcomeValue)) return;
    Story->CompleteAuthoredMission(MissionId);
}
