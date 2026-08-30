// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

enum class EShipVisualProductionKind : uint8
{
    ExteriorHull,
    WeatherDeck,
    MainDeckInterior,
    LowerDeck,
    CargoWorkshop,
    CaptainsCabin
};

struct FShipVisualProductionBrief
{
    FName StableId;
    FString DisplayName;
    EShipVisualProductionKind Kind = EShipVisualProductionKind::ExteriorHull;
    FString GoverningSource;
    FString AuthoredVisualFacts;
    FString FunctionalObjects;
    FString AntiInventionBoundary;
    bool bProviderReferenceReady = false;
};

struct FShipVisualProductionDesignGap
{
    FName StableId;
    FString Description;
    FString GoverningSource;
};

/** Source-backed visual production authority for La Liberacion before UE asset integration. */
class DARKARISEN_API FShipVisualProductionCatalog
{
public:
    static constexpr int32 RequiredBriefCount = 6;
    static constexpr int32 ProviderReadyBriefCount = 5;
    static constexpr int32 ProviderBlockedBriefCount = 1;

    static TArray<FShipVisualProductionBrief> BuildLaLiberacionBriefs();
    static TArray<FShipVisualProductionDesignGap> BuildDesignGaps();
    static bool Validate(TArray<FString>& OutErrors);
};
