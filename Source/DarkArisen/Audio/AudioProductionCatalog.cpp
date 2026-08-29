// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Audio/AudioProductionCatalog.h"

namespace
{
FAudioProductionRequirement Audio(
    const TCHAR* StableId,
    const TCHAR* Name,
    const TCHAR* Source,
    const EAudioProductionKind Kind,
    const TCHAR* Rule)
{
    FAudioProductionRequirement Result;
    Result.StableId = StableId;
    Result.DisplayName = Name;
    Result.GoverningSource = Source;
    Result.Kind = Kind;
    Result.ProductionRule = Rule;
    return Result;
}
}

TArray<FAudioProductionRequirement> FAudioProductionCatalog::BuildCoreRequirements()
{
    return {
        Audio(TEXT("audio.global.non-reactive-score"), TEXT("Non-reactive score law"),
            TEXT("cutscene catalog.md Sections 7.2 and 9; Docs/DesignAuthority.md"),
            EAudioProductionKind::SilenceContract,
            TEXT("Music starts in authored contexts, runs its authored length and stops; gameplay events never add layers, alter intensity or fire stingers.")),
        Audio(TEXT("audio.dungeons.no-ambient-score"), TEXT("Dungeon silence"),
            TEXT("dungeon design philosophy.md Section 8.1; cutscene catalog.md Section 7.1"),
            EAudioProductionKind::SilenceContract,
            TEXT("Ordinary dungeons use environmental sound only; no ambient dungeon score.")),
        Audio(TEXT("audio.highmoore.emergence"), TEXT("The Emergence"),
            TEXT("cutscene catalog.md #7; highmoore music.md Section 3.1"),
            EAudioProductionKind::NonDiegeticCue,
            TEXT("Warm minor chamber cue; no triumphant swell; ends before the player finishes looking.")),
        Audio(TEXT("audio.highmoore.ball-diegetic"), TEXT("Arion ball musicians"),
            TEXT("highmoore music.md Sections 2.2 and 3.2; cutscene catalog.md #8"),
            EAudioProductionKind::DiegeticMusic,
            TEXT("Music comes from visible gallery musicians in-world; no non-diegetic ball score.")),
        Audio(TEXT("audio.highmoore.terrace-silence"), TEXT("Terrace silence"),
            TEXT("highmoore music.md Section 3.3"),
            EAudioProductionKind::Environmental,
            TEXT("Wind and muffled hall sound through the door; no score.")),
        Audio(TEXT("audio.highmoore.lake-ride"), TEXT("Lake ride silence"),
            TEXT("highmoore music.md Sections 4.1-4.2"),
            EAudioProductionKind::Environmental,
            TEXT("Hooves, weather, voices and lake environment carry the sequence before the authored dance cue.")),
        Audio(TEXT("audio.highmoore.rain-dance"), TEXT("Rain dance cue"),
            TEXT("cutscene catalog.md #11 and Section 7.2; highmoore music.md Section 4.3 by non-conflicting detail only"),
            EAudioProductionKind::NonDiegeticCue,
            TEXT("Quiet chamber cue under rain. Under current authority the cue continues its authored playback unchanged through the arrow; no event-triggered musical mutation.")),
        Audio(TEXT("audio.highmoore.arrow-aftermath"), TEXT("Arrow aftermath environmental bed"),
            TEXT("cutscene catalog.md #11 and Section 6 #1"),
            EAudioProductionKind::Environmental,
            TEXT("Rain/world sound continues; no new cue, stinger, swell or reactive layer is triggered by the arrow.")),
        Audio(TEXT("audio.highmoore.belos-silence"), TEXT("House Belos silence"),
            TEXT("highmoore music.md Section 8; cutscene catalog.md Section 6"),
            EAudioProductionKind::SilenceContract,
            TEXT("No score during the nine-minute playable Belos sequence or its aftermath.")),
        Audio(TEXT("audio.katana.resonance"), TEXT("Crystal Katana resonance"),
            TEXT("crystal katana.md; highmoore music.md Section 8.2"),
            EAudioProductionKind::WeaponResonance,
            TEXT("Weapon impact resonance is physical/diegetic sound, not adaptive music.")),
        Audio(TEXT("audio.highmoore.return"), TEXT("Return cue"),
            TEXT("highmoore music.md Section 9.3; princess quest the return.md"),
            EAudioProductionKind::NonDiegeticCue,
            TEXT("Solo-cello final Highmoore cue, forty seconds, stopping before the scene ends; not event-reactive.")),
        Audio(TEXT("audio.battles.open-and-after"), TEXT("Battle score boundary"),
            TEXT("cutscene catalog.md Section 7.1; battle system.md Section 8.1"),
            EAudioProductionKind::NonDiegeticCue,
            TEXT("Authored opening and silence afterward only; no adaptive score running through the battle simulation."))
    };
}

TArray<FAudioProductionDesignGap> FAudioProductionCatalog::BuildDesignGaps()
{
    return {
        {
            TEXT("design-gap.highmoore-arrow-music-conflict"),
            TEXT("Older `highmoore music.md` makes clarinet/piano removal, an unresolved violin and a cello semitone drop happen at the arrow event. Newer Phase-10 `cutscene catalog.md` and current locked law say the music does not react and continues unchanged. Runtime follows the newer non-reactive law; any composition edit that preserves the older musical idea must be pre-authored into a fixed cue rather than triggered by gameplay."),
            TEXT("highmoore music.md Section 5 versus cutscene catalog.md #11, Sections 7.2 and 9")
        },
        {
            TEXT("design-gap.audio-assets"),
            TEXT("The repository does not currently evidence reviewed final SoundWave/MetaSound/music masters for the production catalog. Source requirements are not imported audio assets."),
            TEXT("Docs/CONTENT_ALPHA_PRODUCTION_PLAN.md P8; repository content audit")
        },
        {
            TEXT("design-gap.music-rights-credits"),
            TEXT("Final music composition/recording ownership, licence references, attribution text and cost approval remain unresolved, so credits/music release gates stay closed."),
            TEXT("AlphaReleaseContracts; ContentScale FCreditsReadinessManifest")
        }
    };
}

bool FAudioProductionCatalog::Validate(TArray<FString>& OutErrors)
{
    OutErrors.Reset();
    const TArray<FAudioProductionRequirement> Requirements = BuildCoreRequirements();
    if (Requirements.Num() != CoreRequirementCount)
    {
        OutErrors.Add(FString::Printf(TEXT("Core audio catalog requires exactly %d requirements; found %d."), CoreRequirementCount, Requirements.Num()));
    }

    TSet<FName> Seen;
    for (const FAudioProductionRequirement& Requirement : Requirements)
    {
        if (Requirement.StableId.IsNone()
            || Requirement.DisplayName.IsEmpty()
            || Requirement.GoverningSource.IsEmpty()
            || Requirement.ProductionRule.IsEmpty())
        {
            OutErrors.Add(TEXT("Every audio production requirement needs identity, source and production rule."));
        }
        if (Seen.Contains(Requirement.StableId))
        {
            OutErrors.Add(FString::Printf(TEXT("Duplicate audio production id: %s"), *Requirement.StableId.ToString()));
        }
        Seen.Add(Requirement.StableId);

        if (Requirement.bReactiveToGameplayEvent)
        {
            OutErrors.Add(FString::Printf(TEXT("Reactive music/audio authority is forbidden: %s"), *Requirement.StableId.ToString()));
        }
        if (Requirement.bAssetAuthored || !Requirement.AssetPath.IsEmpty())
        {
            OutErrors.Add(FString::Printf(TEXT("Audio requirement %s may not claim an imported asset before evidence exists."), *Requirement.StableId.ToString()));
        }
    }

    if (BuildDesignGaps().Num() != 3)
    {
        OutErrors.Add(TEXT("Audio production must preserve the arrow-conflict, asset and rights gaps."));
    }
    return OutErrors.IsEmpty();
}
