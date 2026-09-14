#pragma once
#include "CoreMinimal.h"
#include "Interaction/ExamineDocumentActor.h"
#include "MainStoryEvidenceActor.generated.h"
/** Physical evidence that may complete exactly one already-active canonical mission. */
UCLASS()
class DARKARISEN_API AMainStoryEvidenceActor : public AExamineDocumentActor
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FName MissionId;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FName CheckpointId;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FName SpawnId;
    virtual bool CanInteract_Implementation(AActor* Interactor) const override;
    virtual void CompleteInteraction_Implementation(AActor* Interactor) override;
};
