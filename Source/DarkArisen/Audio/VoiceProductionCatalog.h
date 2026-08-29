// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

enum class EDialogueRegister : uint8
{
    High,
    Middle,
    Low,
    CodeSwitching
};

enum class EVoiceProductionTier : uint8
{
    Tier1Full,
    Tier2Significant,
    Tier3Limited,
    Tier4Generic,
    Tier5Atmosphere
};

struct FVoiceRoleProductionDefinition
{
    FName RoleId;
    FString CharacterName;
    FString GoverningSource;
    EVoiceProductionTier VoiceTier = EVoiceProductionTier::Tier1Full;
    EDialogueRegister PrimaryRegister = EDialogueRegister::Middle;
    FString VoiceDirection;
    FString AuditionLineId;
    FString AuditionTranscript;
    FString PronunciationReference;
    bool bDialogueLocked = false;
    bool bSubtitleReady = false;
    bool bAuditionGenerated = false;
    bool bCommercialRightsApproved = false;
    FString ShippingAssetPath;
};

struct FVoiceProductionDesignGap
{
    FName StableId;
    FString Description;
    FString GoverningSource;
};

/** Voice-production authority. It stores no provider credential and never treats an audition as shipping audio. */
class DARKARISEN_API FVoiceProductionCatalog
{
public:
    static constexpr int32 CoreCrewRoleCount = 5;

    static TArray<FVoiceRoleProductionDefinition> BuildCoreCrewRoles();
    static TArray<FVoiceProductionDesignGap> BuildDesignGaps();
    static bool Validate(TArray<FString>& OutErrors);
};
