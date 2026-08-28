// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interaction/ExamineDocumentActor.h"
#include "PhysicalJournalActor.generated.h"

/** Jake physically holds this notebook while its chronological handwriting is presented. */
UCLASS()
class DARKARISEN_API APhysicalJournalActor : public AExamineDocumentActor
{
    GENERATED_BODY()

public:
    APhysicalJournalActor();

    virtual bool CanInteract_Implementation(AActor* Interactor) const override;
    virtual FText GetInteractionLabel_Implementation() const override;
    virtual FText GetExamineBody_Implementation() const override;
    virtual void BeginInteraction_Implementation(AActor* Interactor) override;
    virtual void CancelInteraction_Implementation(AActor* Interactor) override;

    /** Plain text is the notebook's only tracking concession. */
    UFUNCTION(BlueprintCallable, Category = "Journal|Physical")
    void SetSearchQuery(const FString& Query);

private:
    TWeakObjectPtr<AActor> JournalOwner;
    FString SearchQuery;

    FText BuildChronologicalPages() const;
};
