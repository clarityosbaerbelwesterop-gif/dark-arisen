// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Presentation/PresentationProductionCatalog.h"

namespace
{
FAuthoredCutsceneDefinition Cutscene(
    const int32 Number,
    const TCHAR* StableId,
    const TCHAR* DisplayName,
    const TCHAR* Source,
    const TCHAR* Camera,
    const TCHAR* Music,
    const float Seconds,
    const EPresentationAnimationPriority Priority,
    const EPresentationControlOwnership Control = EPresentationControlOwnership::SequencerOwned)
{
    FAuthoredCutsceneDefinition Entry;
    Entry.CatalogNumber = Number;
    Entry.StableId = StableId;
    Entry.DisplayName = DisplayName;
    Entry.GoverningSource = Source;
    Entry.CameraRule = Camera;
    Entry.MusicRule = Music;
    Entry.TargetSeconds = Seconds;
    Entry.AnimationPriority = Priority;
    Entry.ControlOwnership = Control;
    return Entry;
}

FProtectedPlayableMomentDefinition Playable(
    const int32 Number,
    const TCHAR* StableId,
    const TCHAR* DisplayName,
    const TCHAR* Source,
    const TCHAR* Reason)
{
    FProtectedPlayableMomentDefinition Entry;
    Entry.CatalogNumber = Number;
    Entry.StableId = StableId;
    Entry.DisplayName = DisplayName;
    Entry.GoverningSource = Source;
    Entry.Reason = Reason;
    return Entry;
}
}

TArray<FAuthoredCutsceneDefinition> FPresentationProductionCatalog::BuildCutscenes()
{
    TArray<FAuthoredCutsceneDefinition> Result;
    Result.Reserve(RequiredCutsceneCount);

    Result.Add(Cutscene(1, TEXT("cutscene.opening.the-brother"), TEXT("The Brother"),
        TEXT("cutscene catalog.md Section 3.1 #1; characters/ethan_harlow.md"),
        TEXT("Static hold, single position, no coverage."),
        TEXT("None until final eight seconds."), 130.0f, EPresentationAnimationPriority::P1));
    Result.Add(Cutscene(2, TEXT("cutscene.opening.la-liberacion"), TEXT("The Liberación"),
        TEXT("cutscene catalog.md Section 3.1 #2; ship management.md"),
        TEXT("Slow push at maximum 0.15 m/s onto ship name, then release."),
        TEXT("Main theme, first statement."), 80.0f, EPresentationAnimationPriority::P2));
    Result.Add(Cutscene(3, TEXT("cutscene.archipelago.first-letter"), TEXT("The First Letter"),
        TEXT("cutscene catalog.md Section 3.2 #3; retaliation system.md Section 3.2"),
        TEXT("Slow push onto letter on great-cabin table."),
        TEXT("None."), 35.0f, EPresentationAnimationPriority::P3));
    Result.Add(Cutscene(4, TEXT("cutscene.archipelago.the-grove"), TEXT("The Grove"),
        TEXT("cutscene catalog.md Section 3.2 #4"),
        TEXT("Anchored wide for six seconds on entry, then release."),
        TEXT("Ethan motif, unresolved."), 50.0f, EPresentationAnimationPriority::P2,
        EPresentationControlOwnership::AnchoredPlayerMovement));
    Result.Add(Cutscene(5, TEXT("cutscene.archipelago.dream-fight-entry"), TEXT("The Dream Fight Entry"),
        TEXT("cutscene catalog.md Section 3.2 #5; cannabis system.md Section 6"),
        TEXT("Authored non-realist visual grammar; sole permitted exception."),
        TEXT("Mama Jacinta singing, diegetic becoming non-diegetic."), 100.0f, EPresentationAnimationPriority::P1));
    Result.Add(Cutscene(6, TEXT("cutscene.archipelago.alejandro-strain"), TEXT("Don Alejandro and the Strain"),
        TEXT("cutscene catalog.md Section 3.2 #6"),
        TEXT("Slow push, then static hold on hands."),
        TEXT("None."), 65.0f, EPresentationAnimationPriority::P1));
    Result.Add(Cutscene(7, TEXT("cutscene.highmoore.emergence"), TEXT("The Emergence"),
        TEXT("cutscene catalog.md Section 3.3 #7; highmoore region.md Section 1.2; camera system.md Section 6.1"),
        TEXT("Anchored wide six seconds facing basin, then release; never a flythrough; movement remains available."),
        TEXT("Warm minor Highmoore cue, unresolved."), 6.0f, EPresentationAnimationPriority::NoneRequired,
        EPresentationControlOwnership::AnchoredPlayerMovement));
    Result.Add(Cutscene(8, TEXT("cutscene.highmoore.voice-from-behind"), TEXT("The Voice From Behind"),
        TEXT("cutscene catalog.md Section 3.3 #8; princess quest the ball.md Section 4.4"),
        TEXT("Authored over-shoulder for the turn only, then free."),
        TEXT("Diegetic Arion musicians."), 18.0f, EPresentationAnimationPriority::P1,
        EPresentationControlOwnership::BriefAuthoredTakeover));
    Result.Add(Cutscene(9, TEXT("cutscene.highmoore.ejection"), TEXT("The Ejection"),
        TEXT("cutscene catalog.md Section 3.3 #9"),
        TEXT("Static two-shot, no coverage cuts."),
        TEXT("None."), 75.0f, EPresentationAnimationPriority::P1));
    Result.Add(Cutscene(10, TEXT("cutscene.highmoore.false-letter"), TEXT("The Letter — False"),
        TEXT("cutscene catalog.md Section 3.3 #10; princess quest the lake.md Section 2.1"),
        TEXT("Static two-shot, wide, wind."),
        TEXT("None."), 40.0f, EPresentationAnimationPriority::P2));
    {
        FAuthoredCutsceneDefinition Entry = Cutscene(11, TEXT("cutscene.highmoore.arrow"), TEXT("The Arrow"),
            TEXT("cutscene catalog.md Section 3.3 #11; princess quest the lake.md Section 8"),
            TEXT("Camera does not move; control taken for catch only, exactly 1.5 seconds, then returned."),
            TEXT("Rain-dance cue continues unchanged for ninety more seconds and finishes."),
            1.5f, EPresentationAnimationPriority::P1,
            EPresentationControlOwnership::BriefAuthoredTakeover);
        Entry.bCameraMustRemainStatic = true;
        Entry.bMusicMustContinueUnchanged = true;
        Result.Add(Entry);
    }
    Result.Add(Cutscene(12, TEXT("cutscene.highmoore.real-letter"), TEXT("The Letter — Real"),
        TEXT("cutscene catalog.md Section 3.3 #12; princess quest arion falls.md Section 4.4"),
        TEXT("Eight-second slow push onto pillow only; reading remains player-paced/free."),
        TEXT("None; rain on glass."), 8.0f, EPresentationAnimationPriority::P3,
        EPresentationControlOwnership::SlowPushOnly));
    Result.Add(Cutscene(13, TEXT("cutscene.highmoore.she-wrote-two"), TEXT("She Wrote Two"),
        TEXT("cutscene catalog.md Section 3.3 #13"),
        TEXT("Static two-shot in rain with Maerwyn permanently in frame and never cut to."),
        TEXT("None."), 150.0f, EPresentationAnimationPriority::P1));
    Result.Add(Cutscene(14, TEXT("cutscene.highmoore.wizards-question"), TEXT("The Wizard's Question"),
        TEXT("cutscene catalog.md Section 3.3 #14"),
        TEXT("Ellis simply exists in an already-running frame; no entrance/reveal."),
        TEXT("None."), 110.0f, EPresentationAnimationPriority::P1));

    for (int32 Number = 15; Number <= 19; ++Number)
    {
        FAuthoredCutsceneDefinition Entry = Cutscene(
            Number,
            *FString::Printf(TEXT("cutscene.endgame.slot-%d"), Number),
            *FString::Printf(TEXT("Endgame Cutscene Slot %d"), Number),
            TEXT("cutscene catalog.md Section 3.4; main story structure.md; missing crimson_armada source"),
            TEXT("UNRESOLVED — final-act source identity/camera specification not present in repository."),
            TEXT("UNRESOLVED — no Highmoore reference permitted."),
            0.0f,
            EPresentationAnimationPriority::Unresolved);
        Entry.bIdentityResolved = false;
        Result.Add(Entry);
    }

    return Result;
}

TArray<FProtectedPlayableMomentDefinition> FPresentationProductionCatalog::BuildProtectedPlayableMoments()
{
    return {
        Playable(1, TEXT("playable.arrow.after-thirty-seconds"), TEXT("The thirty seconds after the arrow"), TEXT("cutscene catalog.md Section 6 #1"), TEXT("Full control with nothing useful to do; helplessness is the mechanic.")),
        Playable(2, TEXT("playable.lake.standing"), TEXT("The standing at the lake"), TEXT("cutscene catalog.md Section 6 #2"), TEXT("No prompt/timer; player chooses when to leave.")),
        Playable(3, TEXT("playable.archer.kill"), TEXT("Killing the archer"), TEXT("cutscene catalog.md Section 6 #3"), TEXT("Gameplay, not a cinematic.")),
        Playable(4, TEXT("playable.real-letter.reading"), TEXT("Reading the real letter"), TEXT("cutscene catalog.md Section 6 #4"), TEXT("Player-paced reading, rain on glass.")),
        Playable(5, TEXT("playable.cassian.kill"), TEXT("Killing Cassian"), TEXT("cutscene catalog.md Section 6 #5"), TEXT("One input; not a cinematic.")),
        Playable(6, TEXT("playable.belos.nine-minutes"), TEXT("The nine minutes at Belos"), TEXT("cutscene catalog.md Section 6 #6"), TEXT("Entirely playable.")),
        Playable(7, TEXT("playable.belos.walk-out"), TEXT("The walk out of Belos"), TEXT("cutscene catalog.md Section 6 #7"), TEXT("Player-controlled through consequences.")),
        Playable(8, TEXT("playable.stair.approach"), TEXT("Going to the stair"), TEXT("cutscene catalog.md Section 6 #8"), TEXT("Input exists; rubble clears; only then does it cut.")),
        Playable(9, TEXT("playable.mountain.return"), TEXT("The return through the mountain"), TEXT("cutscene catalog.md Section 6 #9"), TEXT("Forty minutes at a walk.")),
        Playable(10, TEXT("playable.churchyard"), TEXT("The churchyard"), TEXT("cutscene catalog.md Section 6 #10"), TEXT("Enterable; no prompt; nothing happens.")),
        Playable(11, TEXT("playable.boss-death.hold"), TEXT("Every boss death"), TEXT("cutscene catalog.md Section 6 #11; colonial war bosses.md"), TEXT("The Hold, never a death cinematic.")),
        Playable(12, TEXT("playable.ibarra.garden"), TEXT("Ibarra in the garden"), TEXT("cutscene catalog.md Section 6 #12"), TEXT("No arena, phases or camera seizure.")),
        Playable(13, TEXT("playable.guardian.settling"), TEXT("The Guardian settling"), TEXT("cutscene catalog.md Section 6 #13"), TEXT("Static hold while player retains control.")),
        Playable(14, TEXT("playable.light-elf.ending"), TEXT("The Light Elf ending"), TEXT("cutscene catalog.md Section 6 #14"), TEXT("Empty hilltop and wind returning.")),
        Playable(15, TEXT("playable.vidal.confession"), TEXT("Vidal's confession"), TEXT("cutscene catalog.md Section 6 #15"), TEXT("Player-paced dialogue with real options.")),
        Playable(16, TEXT("playable.kettle.office"), TEXT("Kettle's office"), TEXT("cutscene catalog.md Section 6 #16"), TEXT("A man behind a desk talking.")),
        Playable(17, TEXT("playable.aldric.ninety-seconds"), TEXT("Aldric's ninety seconds"), TEXT("cutscene catalog.md Section 6 #17"), TEXT("Playable while standing over him.")),
        Playable(18, TEXT("playable.rowland.offer"), TEXT("Rowland's offer"), TEXT("cutscene catalog.md Section 6 #18"), TEXT("No accept option; still player-paced.")),
        Playable(19, TEXT("playable.liberation.aftermath"), TEXT("Every Liberation Event aftermath"), TEXT("cutscene catalog.md Section 6 #19"), TEXT("Free camera and it does not find Jake.")),
        Playable(20, TEXT("playable.great-cabin.crew-scene"), TEXT("The crew scene in the great cabin"), TEXT("cutscene catalog.md Section 6 #20"), TEXT("Dialogue with three options; player-paced.")),
        Playable(21, TEXT("playable.maerwyn.breakfast-line"), TEXT("Maerwyn's line at breakfast"), TEXT("cutscene catalog.md Section 6 #21"), TEXT("No reaction shot; scene moves on.")),
        Playable(22, TEXT("playable.stopped-shaft.first-sight"), TEXT("The first sight of a stopped shaft"), TEXT("cutscene catalog.md Section 6 #22"), TEXT("Player is standing there; no cue."))
    };
}

TArray<FName> FPresentationProductionCatalog::BuildInsertIds()
{
    return {
        TEXT("insert.belos.shaking-hands"),
        TEXT("insert.nine-state-treasures"),
        TEXT("insert.black-candle"),
        TEXT("insert.notice-of-engagement"),
        TEXT("insert.elowen-signatures")
    };
}

TArray<FName> FPresentationProductionCatalog::BuildSlowPushIds()
{
    return {
        TEXT("slowpush.ship-name"),
        TEXT("slowpush.first-imperial-letter"),
        TEXT("slowpush.ethans-strain"),
        TEXT("slowpush.real-letter-pillow"),
        TEXT("slowpush.lake-boundary-stone-ngplus"),
        TEXT("slowpush.empty-rabbit-hutch")
    };
}

TArray<FPresentationDesignGap> FPresentationProductionCatalog::BuildDesignGaps()
{
    return {
        {
            TEXT("design-gap.cutscene-15-19-identities"),
            TEXT("Cutscene catalog locks five final-act slots but delegates identity/camera/music/animation details to a Crimson Armada source that is not present under that document identity. Slots stay unresolved rather than invented."),
            TEXT("cutscene catalog.md Section 3.4; main story structure.md")
        },
        {
            TEXT("design-gap.p1-summary-count"),
            TEXT("Cutscene #9 individually specifies P1 animation, while Section 8.1's P1 summary omits #9 yet states nine total and also reserves two endgame P1 sequences. The narrower #9 entry is preserved and the aggregate inconsistency remains unresolved."),
            TEXT("cutscene catalog.md Sections 3.3 #9 and 8.1")
        },
        {
            TEXT("design-gap.presentation-assets"),
            TEXT("No reviewed Sequencer/performance-capture/facial-animation asset set exists in repository evidence; this catalog is production authority only, not proof of authored Unreal assets."),
            TEXT("Docs/CONTENT_ALPHA_PRODUCTION_PLAN.md P7; repository content audit")
        }
    };
}

bool FPresentationProductionCatalog::Validate(TArray<FString>& OutErrors)
{
    OutErrors.Reset();

    const TArray<FAuthoredCutsceneDefinition> Cutscenes = BuildCutscenes();
    if (Cutscenes.Num() != RequiredCutsceneCount)
    {
        OutErrors.Add(FString::Printf(TEXT("Presentation catalog requires exactly %d cutscene slots; found %d."), RequiredCutsceneCount, Cutscenes.Num()));
    }

    int32 ResolvedCount = 0;
    int32 UnresolvedCount = 0;
    TSet<FName> CutsceneIds;
    for (const FAuthoredCutsceneDefinition& Entry : Cutscenes)
    {
        if (Entry.CatalogNumber < 1 || Entry.CatalogNumber > 19 || Entry.StableId.IsNone() || Entry.GoverningSource.IsEmpty())
        {
            OutErrors.Add(TEXT("Every cutscene slot requires valid number, stable id and source."));
        }
        if (CutsceneIds.Contains(Entry.StableId))
        {
            OutErrors.Add(FString::Printf(TEXT("Duplicate cutscene stable id: %s"), *Entry.StableId.ToString()));
        }
        CutsceneIds.Add(Entry.StableId);
        ResolvedCount += Entry.bIdentityResolved ? 1 : 0;
        UnresolvedCount += Entry.bIdentityResolved ? 0 : 1;

        if (Entry.bIdentityResolved && (Entry.DisplayName.IsEmpty() || Entry.CameraRule.IsEmpty() || Entry.MusicRule.IsEmpty() || Entry.TargetSeconds <= 0.0f))
        {
            OutErrors.Add(FString::Printf(TEXT("Resolved cutscene %s lacks authored production detail."), *Entry.StableId.ToString()));
        }
    }
    if (ResolvedCount != ResolvedCutsceneIdentityCount || UnresolvedCount != UnresolvedEndgameCutsceneCount)
    {
        OutErrors.Add(TEXT("Cutscene identity state must remain fourteen resolved plus five final-act gaps."));
    }

    const TArray<FProtectedPlayableMomentDefinition> Protected = BuildProtectedPlayableMoments();
    if (Protected.Num() != RequiredProtectedPlayableCount)
    {
        OutErrors.Add(TEXT("Protected playable catalog must contain exactly twenty-two moments."));
    }
    TSet<FName> ProtectedIds;
    for (const FProtectedPlayableMomentDefinition& Entry : Protected)
    {
        if (Entry.StableId.IsNone() || Entry.GoverningSource.IsEmpty() || Entry.Reason.IsEmpty())
        {
            OutErrors.Add(TEXT("Every protected playable moment requires identity, source and reason."));
        }
        if (CutsceneIds.Contains(Entry.StableId) || ProtectedIds.Contains(Entry.StableId))
        {
            OutErrors.Add(FString::Printf(TEXT("Presentation identity collision: %s"), *Entry.StableId.ToString()));
        }
        ProtectedIds.Add(Entry.StableId);
    }

    if (BuildInsertIds().Num() != RequiredInsertCount || BuildSlowPushIds().Num() != RequiredSlowPushCount)
    {
        OutErrors.Add(TEXT("Presentation inserts/slow pushes must remain exactly five/six."));
    }

    const auto FindCutscene = [&Cutscenes](const TCHAR* StableId) -> const FAuthoredCutsceneDefinition*
    {
        return Cutscenes.FindByPredicate([StableId](const FAuthoredCutsceneDefinition& Entry)
        {
            return Entry.StableId == FName(StableId);
        });
    };

    const FAuthoredCutsceneDefinition* Grove = FindCutscene(TEXT("cutscene.archipelago.the-grove"));
    const FAuthoredCutsceneDefinition* Emergence = FindCutscene(TEXT("cutscene.highmoore.emergence"));
    if (!Grove || !Emergence
        || Grove->ControlOwnership != EPresentationControlOwnership::AnchoredPlayerMovement
        || Emergence->ControlOwnership != EPresentationControlOwnership::AnchoredPlayerMovement)
    {
        OutErrors.Add(TEXT("Grove and Emergence must use anchored camera ownership while preserving player movement."));
    }

    const FAuthoredCutsceneDefinition* VoiceFromBehind = FindCutscene(TEXT("cutscene.highmoore.voice-from-behind"));
    if (!VoiceFromBehind || VoiceFromBehind->ControlOwnership != EPresentationControlOwnership::BriefAuthoredTakeover)
    {
        OutErrors.Add(TEXT("Voice From Behind may take control only for its authored turn before returning to free camera."));
    }

    const FAuthoredCutsceneDefinition* Arrow = FindCutscene(TEXT("cutscene.highmoore.arrow"));
    if (!Arrow || !Arrow->bCameraMustRemainStatic || !Arrow->bMusicMustContinueUnchanged
        || !FMath::IsNearlyEqual(Arrow->TargetSeconds, 1.5f)
        || Arrow->ControlOwnership != EPresentationControlOwnership::BriefAuthoredTakeover)
    {
        OutErrors.Add(TEXT("Arrow must remain a 1.5-second brief takeover with static camera and non-reactive music."));
    }

    const FAuthoredCutsceneDefinition* RealLetter = FindCutscene(TEXT("cutscene.highmoore.real-letter"));
    if (!RealLetter || RealLetter->ControlOwnership != EPresentationControlOwnership::SlowPushOnly)
    {
        OutErrors.Add(TEXT("Real Letter cutscene ownership ends with the eight-second pillow push; reading remains player-paced."));
    }

    if (BuildDesignGaps().Num() != 3)
    {
        OutErrors.Add(TEXT("Presentation production must preserve the three current source/asset gaps."));
    }

    return OutErrors.IsEmpty();
}
