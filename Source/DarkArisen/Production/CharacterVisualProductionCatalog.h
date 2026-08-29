// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

enum class ECharacterVisualProductionRole : uint8
{
    Protagonist,
    CoreCrew,
    MajorNarrative,
    MajorAntagonist
};

struct FCharacterVisualProductionBrief
{
    FName StableId;
    FString DisplayName;
    ECharacterVisualProductionRole Role = ECharacterVisualProductionRole::MajorNarrative;
    FString GoverningSource;
    FString PhysicalFacts;
    FString WardrobeAndObjects;
    FString PerformanceRead;
    FString ExplicitUnknowns;

    /** True only when current design authority has enough non-conflicting visual facts for a bounded provider reference prompt. */
    bool bProviderReferenceReady = false;

    /** External output has not been generated/reviewed merely because this source brief exists. */
    bool bApprovedReferenceExists = false;
    FString ApprovedReferencePath;
};

struct FCharacterVisualProductionDesignGap
{
    FName StableId;
    FString Description;
    FString GoverningSource;
};

/** Source-only visual authority for the first major-character production batch. */
class DARKARISEN_API FCharacterVisualProductionCatalog
{
public:
    static constexpr int32 MajorCharacterBriefCount = 9;
    static constexpr int32 ProviderReadyCharacterCount = 6;
    static constexpr int32 ExplicitlyBlockedCharacterCount = 3;

    static TArray<FCharacterVisualProductionBrief> BuildMajorCharacterBriefs();
    static TArray<FCharacterVisualProductionDesignGap> BuildDesignGaps();
    static bool Validate(TArray<FString>& OutErrors);
};