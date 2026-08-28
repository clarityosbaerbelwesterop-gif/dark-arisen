// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Interaction/PhysicalMapActor.h"

APhysicalMapActor::APhysicalMapActor()
{
    DocumentTitle = FText::FromString(TEXT("Jake's map"));
}

FText APhysicalMapActor::GetInteractionLabel_Implementation() const
{
    return FText::FromString(TEXT("Unfold map"));
}

FText APhysicalMapActor::GetExamineTitle_Implementation() const
{
    return FText::FromString(TEXT("Jake's map"));
}

FText APhysicalMapActor::GetExamineBody_Implementation() const
{
    FString Body;
    AppendRecords(Body, TEXT("Coastlines sailed"), SailedCoastlines);
    AppendRecords(Body, TEXT("Settlements visited"), VisitedSettlements);
    AppendRecords(Body, TEXT("Routes given"), GivenRoutes);
    AppendRecords(Body, TEXT("In Jake's hand"), HandwrittenAnnotations);

    if (Body.IsEmpty())
    {
        Body = TEXT("Blank water and the coast he already knows.");
    }
    return FText::FromString(Body);
}

bool APhysicalMapActor::RecordSailedCoastline(const FName StableId, const FText Label)
{
    return AddUniqueRecord(SailedCoastlines, StableId, Label);
}

bool APhysicalMapActor::RecordVisitedSettlement(const FName StableId, const FText Label)
{
    return AddUniqueRecord(VisitedSettlements, StableId, Label);
}

bool APhysicalMapActor::RecordGivenRoute(const FName StableId, const FText Label)
{
    return AddUniqueRecord(GivenRoutes, StableId, Label);
}

bool APhysicalMapActor::AddHandwrittenAnnotation(const FName AnnotationId, const FText Text)
{
    return AddUniqueRecord(HandwrittenAnnotations, AnnotationId, Text);
}

bool APhysicalMapActor::AddUniqueRecord(
    TArray<FPhysicalMapRecord>& Records,
    const FName StableId,
    const FText Label)
{
    if (StableId.IsNone() || Label.IsEmpty())
    {
        return false;
    }

    if (Records.ContainsByPredicate(
        [StableId](const FPhysicalMapRecord& Record) { return Record.StableId == StableId; }))
    {
        return false;
    }

    FPhysicalMapRecord Record;
    Record.StableId = StableId;
    Record.Label = Label;
    Records.Add(MoveTemp(Record));
    return true;
}

void APhysicalMapActor::AppendRecords(
    FString& Out,
    const TCHAR* Heading,
    const TArray<FPhysicalMapRecord>& Records)
{
    if (Records.IsEmpty())
    {
        return;
    }

    if (!Out.IsEmpty())
    {
        Out += TEXT("\n\n");
    }
    Out += Heading;
    Out += TEXT("\n");

    for (const FPhysicalMapRecord& Record : Records)
    {
        Out += TEXT("— ");
        Out += Record.Label.ToString();
        Out += TEXT("\n");
    }
}
