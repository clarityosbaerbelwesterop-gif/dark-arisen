// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interaction/ExamineDocumentActor.h"
#include "PhysicalMapActor.generated.h"

USTRUCT(BlueprintType)
struct FPhysicalMapRecord
{
    GENERATED_BODY()

    UPROPERTY(SaveGame, BlueprintReadOnly)
    FName StableId;

    UPROPERTY(SaveGame, BlueprintReadOnly)
    FText Label;
};

/**
 * Phase-12 physical map. Jake unfolds and holds this actor through the existing anchored
 * Examine path, so the world remains visible around the paper.
 *
 * It records only sailed coastlines, visited settlements, given routes and Jake's annotations.
 * There is intentionally no player position, quest-marker, compass, minimap or map-click API.
 */
UCLASS()
class DARKARISEN_API APhysicalMapActor : public AExamineDocumentActor
{
    GENERATED_BODY()

public:
    APhysicalMapActor();

    virtual FText GetInteractionLabel_Implementation() const override;
    virtual FText GetExamineTitle_Implementation() const override;
    virtual FText GetExamineBody_Implementation() const override;

    UFUNCTION(BlueprintCallable, Category="Map|Physical")
    bool RecordSailedCoastline(FName StableId, FText Label);

    UFUNCTION(BlueprintCallable, Category="Map|Physical")
    bool RecordVisitedSettlement(FName StableId, FText Label);

    UFUNCTION(BlueprintCallable, Category="Map|Physical")
    bool RecordGivenRoute(FName StableId, FText Label);

    UFUNCTION(BlueprintCallable, Category="Map|Physical")
    bool AddHandwrittenAnnotation(FName AnnotationId, FText Text);

private:
    static bool AddUniqueRecord(TArray<FPhysicalMapRecord>& Records, FName StableId, FText Label);
    static void AppendRecords(FString& Out, const TCHAR* Heading, const TArray<FPhysicalMapRecord>& Records);

    UPROPERTY(SaveGame)
    TArray<FPhysicalMapRecord> SailedCoastlines;

    UPROPERTY(SaveGame)
    TArray<FPhysicalMapRecord> VisitedSettlements;

    UPROPERTY(SaveGame)
    TArray<FPhysicalMapRecord> GivenRoutes;

    UPROPERTY(SaveGame)
    TArray<FPhysicalMapRecord> HandwrittenAnnotations;
};
