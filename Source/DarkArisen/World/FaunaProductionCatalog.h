// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

enum class EFaunaCorpusFamily : uint8
{
    LandAnimals,
    Birds,
    SeaAnimals,
    SmallCreatures,
    PlantsAndHerbs
};

struct FFaunaCorpusContract
{
    EFaunaCorpusFamily Family = EFaunaCorpusFamily::LandAnimals;
    FString DisplayName;
    int32 RequiredBaseIdentityCount = 0;
    int32 RequiredLegendaryCount = 0;
    bool bCountIsMinimum = false;
    bool bLegendaryCountIncludedInBase = false;
    FString GoverningSource;
};

struct FRegionalFaunaIdentity
{
    FName StableId;
    FString DisplayName;
    FString GoverningSource;
    FString AudibleIdentity;
    FString EcologicalProductionRule;
    bool bSilenceIsIntentional = false;
};

struct FNamedFaunaProductionAnchor
{
    FName StableId;
    FString DisplayName;
    FString GoverningSource;
    FString ProductionLaw;
    bool bRuntimeCreatureAssetAuthored = false;
    FString RuntimeCreatureAssetPath;
};

struct FFaunaProductionDesignGap
{
    FName StableId;
    FString Description;
    FString GoverningSource;
};

/**
 * Production-scale authority for fauna/flora corpus counts, regional ecological identity and named
 * cross-system creature anchors. It never spawns wildlife and never licenses procedural encounter
 * generation. Real placement, animation, AI, audio and meshes remain authored Unreal asset work.
 */
class DARKARISEN_API FFaunaProductionCatalog
{
public:
    static constexpr int32 LandAnimalSpeciesCount = 42;
    static constexpr int32 LandLegendaryVariantCount = 6;
    static constexpr int32 BirdSpeciesCount = 52;
    static constexpr int32 BirdLegendaryCount = 4;
    static constexpr int32 SeaSpeciesCount = 65;
    static constexpr int32 SeaLegendaryWithinSpeciesCount = 2;
    static constexpr int32 SmallCreatureTypeCount = 45;
    static constexpr int32 SmallCreatureLegendaryCount = 3;
    static constexpr int32 PlantSpeciesMinimum = 60;
    static constexpr int32 PlantLegendaryCount = 4;
    static constexpr int32 HighmooreMvpCoreSpeciesCount = 5;
    static constexpr int32 RequiredRegionalEcologyProfiles = 8;

    static TArray<FFaunaCorpusContract> BuildCorpusContracts();
    static TArray<FRegionalFaunaIdentity> BuildRegionalIdentities();
    static TArray<FNamedFaunaProductionAnchor> BuildNamedAnchors();
    static TArray<FFaunaProductionDesignGap> BuildDesignGaps();
    static bool Validate(TArray<FString>& OutErrors);

    static bool AllowsRandomEncounterGeneration() { return false; }
    static bool AllowsGenericSpawnPointSubstitution() { return false; }
    static bool RequiresCulturalConsequenceWhereAuthored() { return true; }
    static bool RequiresEcologicalPersistenceWhereAuthored() { return true; }
    static bool AllowsHorseIntoStoppedLightShaft() { return false; }
};
