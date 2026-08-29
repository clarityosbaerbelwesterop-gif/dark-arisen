// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Audio/VoiceProductionCatalog.h"

namespace
{
FVoiceRoleProductionDefinition Role(
    const TCHAR* RoleId,
    const TCHAR* Name,
    const EDialogueRegister Register,
    const TCHAR* Direction,
    const TCHAR* LineId,
    const TCHAR* Transcript)
{
    FVoiceRoleProductionDefinition Result;
    Result.RoleId = RoleId;
    Result.CharacterName = Name;
    Result.GoverningSource = TEXT("docs/design/npcs/named_crew_deep_dives.md; docs/design/npcs/dialogue_system.md; operator voice review 2026-08-29");
    Result.VoiceTier = EVoiceProductionTier::Tier1Full;
    Result.PrimaryRegister = Register;
    Result.VoiceDirection = Direction;
    Result.AuditionLineId = LineId;
    Result.AuditionTranscript = Transcript;
    Result.PronunciationReference = TEXT("Docs/Voice/CORE_CREW_PRONUNCIATION.md");
    Result.bDialogueLocked = true;
    Result.bSubtitleReady = true;
    Result.AuditionDisposition = EVoiceAuditionDisposition::RejectedSynthetic;
    Result.bDirectedPerformanceReviewRequired = true;
    Result.bCurrentSyntheticProviderRejected = true;
    return Result;
}
}

TArray<FVoiceRoleProductionDefinition> FVoiceProductionCatalog::BuildCoreCrewRoles()
{
    return {
        Role(
            TEXT("voice.crew.mira"), TEXT("Mira"), EDialogueRegister::Middle,
            TEXT("Rich alto; subtle Fjordlund accent; direct and quiet by default, commanding only when needed; grief is underplayed, specific and lived-in rather than sentimental. Requires breath, hesitation and subtext that survive a blind human-performance review."),
            TEXT("dialogue.mira.audition.professional-greeting"),
            TEXT("Captain. Wind's coming from the north. Two hours to harbor.")),
        Role(
            TEXT("voice.crew.big-tom"), TEXT("Big Tom"), EDialogueRegister::Low,
            TEXT("Deep baritone; loud from old forge hearing damage, warm rather than booming; Pale Isle accent worn down by years away; storyteller rhythm with irregular breath and humour that can turn serious without a voice-model gear change."),
            TEXT("dialogue.big-tom.audition.forge"),
            TEXT("Steel doesn't lie, Captain. Steel knows what it is. Better than people, that.")),
        Role(
            TEXT("voice.crew.ines"), TEXT("Ines"), EDialogueRegister::Middle,
            TEXT("Cool soprano; educated precise diction; slight Rexan accent appears under emotion rather than as a constant effect; clinical detail effortless, with restrained warmth and micro-pauses instead of synthetic evenness."),
            TEXT("dialogue.ines.audition.professional-greeting"),
            TEXT("Captain. I've finished the tonics you requested. Three healing, two stamina, one antidote. Quality grade A.")),
        Role(
            TEXT("voice.crew.father-salvio"), TEXT("Father Salvio"), EDialogueRegister::CodeSwitching,
            TEXT("Warm tenor; educated diction; calm inflection; expert register switching; philosophical without sermon cadence. Humour, doubt and fatigue must remain audible so the performance never becomes a generic wise-priest voice."),
            TEXT("dialogue.salvio.audition.day-greeting"),
            TEXT("Captain. The morning light is exceptional today. Have you had breakfast?")),
        Role(
            TEXT("voice.crew.esteban"), TEXT("Esteban"), EDialogueRegister::Middle,
            TEXT("Soft baritone; slight Caribbean/Rexan-Moran cadence; slow and deliberate without frailty. Age lives in breath and timing, not an old-man filter; wisdom is carried lightly and conversationally."),
            TEXT("dialogue.esteban.audition.standard-greeting"),
            TEXT("Mi capitán. The morning is sweet, no? I made coffee. Strong. The way Esmeralda used to make it."))
    };
}

TArray<FVoiceProductionDesignGap> FVoiceProductionCatalog::BuildDesignGaps()
{
    return {
        {
            TEXT("design-gap.tier1-performance-casting"),
            TEXT("The first generic AI auditions were explicitly rejected for sounding synthetic and emotionally flat. Tier-1 core cast therefore needs directed performance casting/recording, or a later voice solution that passes the same blind human-performance bar; the current connected generic provider is not approved for shipping core-cast dialogue."),
            TEXT("operator voice review 2026-08-29; named_crew_deep_dives.md; dialogue_system.md")
        },
        {
            TEXT("design-gap.full-dialogue-lock"),
            TEXT("The five crew voice bibles contain many authored samples and future branches, but the complete final dialogue script with stable line IDs is not yet locked. Only the five audition lines in this catalog are text/subtitle locked today."),
            TEXT("named_crew_deep_dives.md; dialogue_system.md")
        },
        {
            TEXT("design-gap.major-cast-voice-matrix"),
            TEXT("Bosses, sages, Highmoore cast and other Tier-1/Tier-2 NPCs still require the same role/direction/pronunciation/subtitle matrix and directed-performance acceptance before bulk recording."),
            TEXT("dialogue_system.md Section 3; colonial war bosses.md; Highmoore character sources")
        }
    };
}

bool FVoiceProductionCatalog::Validate(TArray<FString>& OutErrors)
{
    OutErrors.Reset();
    const TArray<FVoiceRoleProductionDefinition> Roles = BuildCoreCrewRoles();
    if (Roles.Num() != CoreCrewRoleCount)
    {
        OutErrors.Add(FString::Printf(TEXT("Core crew voice catalog requires exactly %d roles; found %d."), CoreCrewRoleCount, Roles.Num()));
    }

    TSet<FName> SeenRoles;
    TSet<FString> SeenLineIds;
    for (const FVoiceRoleProductionDefinition& Role : Roles)
    {
        if (Role.RoleId.IsNone()
            || Role.CharacterName.IsEmpty()
            || Role.GoverningSource.IsEmpty()
            || Role.VoiceDirection.IsEmpty()
            || Role.AuditionLineId.IsEmpty()
            || Role.AuditionTranscript.IsEmpty()
            || Role.PronunciationReference.IsEmpty())
        {
            OutErrors.Add(TEXT("Every core voice role requires identity, source, direction, locked audition line and pronunciation reference."));
        }
        if (SeenRoles.Contains(Role.RoleId))
        {
            OutErrors.Add(FString::Printf(TEXT("Duplicate core voice role id: %s"), *Role.RoleId.ToString()));
        }
        if (SeenLineIds.Contains(Role.AuditionLineId))
        {
            OutErrors.Add(FString::Printf(TEXT("Duplicate core audition line id: %s"), *Role.AuditionLineId));
        }
        SeenRoles.Add(Role.RoleId);
        SeenLineIds.Add(Role.AuditionLineId);

        if (!Role.bDialogueLocked || !Role.bSubtitleReady)
        {
            OutErrors.Add(FString::Printf(TEXT("Core audition line must be text/subtitle locked before performance review: %s"), *Role.AuditionLineId));
        }
        if (Role.AuditionDisposition != EVoiceAuditionDisposition::RejectedSynthetic
            || !Role.bDirectedPerformanceReviewRequired
            || !Role.bCurrentSyntheticProviderRejected)
        {
            OutErrors.Add(FString::Printf(TEXT("Rejected synthetic audition state must remain fail-closed for core role: %s"), *Role.RoleId.ToString()));
        }
        if (Role.bCommercialRightsApproved || !Role.ShippingAssetPath.IsEmpty())
        {
            OutErrors.Add(FString::Printf(TEXT("Rejected audition may not become approved/shipping audio without new evidence: %s"), *Role.RoleId.ToString()));
        }
    }

    if (BuildDesignGaps().Num() != 3)
    {
        OutErrors.Add(TEXT("Voice production must preserve the current casting/script/cast gaps."));
    }

    return OutErrors.IsEmpty();
}
