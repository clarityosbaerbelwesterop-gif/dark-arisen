// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Animation/AnimationProductionCatalog.h"

#include "CoreLoopTuning.h"

namespace
{
FAnimationProductionRequirement Requirement(
    const TCHAR* StableId,
    const TCHAR* DisplayName,
    const TCHAR* Source,
    const EAnimationProductionFamily Family,
    const TCHAR* Authority,
    const TCHAR* Acceptance)
{
    FAnimationProductionRequirement Result;
    Result.StableId = StableId;
    Result.DisplayName = DisplayName;
    Result.GoverningSource = Source;
    Result.Family = Family;
    Result.GameplayAuthority = Authority;
    Result.AcceptanceRead = Acceptance;
    return Result;
}

FAnimationWeightTiming WeightTiming(
    const EAnimationWeaponWeightClass WeightClass,
    const int32 StartupFrames,
    const int32 RecoveryFrames)
{
    FAnimationWeightTiming Timing;
    Timing.WeightClass = WeightClass;
    Timing.StartupFrames = StartupFrames;
    Timing.RecoveryFrames = RecoveryFrames;
    Timing.GoverningSource = TEXT("animation system.md Section 3; CoreLoopTuning.h");
    return Timing;
}

FAnimationWoundLayerRequirement WoundRequirement(
    const EAnimationWoundLayer Layer,
    const TCHAR* Trigger,
    const TCHAR* RequiredBodyRead)
{
    FAnimationWoundLayerRequirement RequirementValue;
    RequirementValue.Layer = Layer;
    RequirementValue.Trigger = Trigger;
    RequirementValue.RequiredBodyRead = RequiredBodyRead;
    RequirementValue.GoverningSource = TEXT("animation system.md Section 5.2; CoreLoopTuning.h");
    return RequirementValue;
}
}

TArray<FAnimationProductionRequirement> FAnimationProductionCatalog::BuildNamedBibleRequirements()
{
    return {
        Requirement(TEXT("anim.named.bad-catch"), TEXT("The bad catch"),
            TEXT("animation system.md Section 7; princess quest the lake.md Section 8.3"),
            EAnimationProductionFamily::PerformanceMoment,
            TEXT("PrincessQuestStateComponent / presentation ownership"),
            TEXT("Deliberately clumsy catch; both go down onto the stones; no slow motion.")),
        Requirement(TEXT("anim.named.jake-shaking-hands"), TEXT("Jake's hands, shaking"),
            TEXT("animation system.md Section 7; princess quest belos.md Section 5.3"),
            EAnimationProductionFamily::PerformanceMoment,
            TEXT("Presentation insert contract"),
            TEXT("Two-second insert; hand performance reads deterioration without adding gameplay authority.")),
        Requirement(TEXT("anim.named.guardian-settling"), TEXT("The Guardian settling"),
            TEXT("animation system.md Section 7; crystal guardian.md Section 7.1"),
            EAnimationProductionFamily::PerformanceMoment,
            TEXT("CrystalGuardianComponent"),
            TEXT("Settles into a kneel; never falls; protected playable hold remains player-controlled.")),
        Requirement(TEXT("anim.named.katana-pickup"), TEXT("Picking up the Katana"),
            TEXT("animation system.md Section 7; crystal guardian.md Section 7.3"),
            EAnimationProductionFamily::Interaction,
            TEXT("CrystalKatanaComponent"),
            TEXT("Player physically walks around the defeated Guardian and reaches for the sword; no fade pickup.")),
        Requirement(TEXT("anim.named.vega-stands-again"), TEXT("Vega standing back up"),
            TEXT("animation system.md Section 7; colonial war bosses.md Section 8.4"),
            EAnimationProductionFamily::Combat,
            TEXT("Tier1CommanderEncounterComponent"),
            TEXT("After posture break and refused mercy, Vega visibly stands back up; no clean spare animation.")),
        Requirement(TEXT("anim.named.bailiffs-leaving"), TEXT("The bailiffs leaving"),
            TEXT("animation system.md Section 7; krone_colonies.md Section 6.10"),
            EAnimationProductionFamily::PerformanceMoment,
            TEXT("Ashcroft encounter / authored NPC state"),
            TEXT("Bailiffs leave individually at different points; not a synchronized rout.")),
        Requirement(TEXT("anim.named.cassian-stop"), TEXT("Cassian's stop"),
            TEXT("animation system.md Section 7; arion falls.md Section 6.4"),
            EAnimationProductionFamily::PerformanceMoment,
            TEXT("Presentation dialogue state"),
            TEXT("Stops mid-sentence on seeing paper; facial/body performance drives the beat.")),
        Requirement(TEXT("anim.named.dance-two"), TEXT("The dance ×2"),
            TEXT("animation system.md Section 7; the ball.md Section 6; the lake.md Section 7.2"),
            EAnimationProductionFamily::PerformanceMoment,
            TEXT("Princess authored sequence state"),
            TEXT("Two distinct authored dances; neither participant is good at it.")),
        Requirement(TEXT("anim.named.big-tom-katana"), TEXT("Big Tom holding the Katana"),
            TEXT("animation system.md Section 7; crystal katana.md Section 5.1"),
            EAnimationProductionFamily::Interaction,
            TEXT("CrystalKatanaComponent / crew scene"),
            TEXT("He cannot find a comfortable purchase and the handling visibly shows it.")),
        Requirement(TEXT("anim.named.mount-three-attempts"), TEXT("Three attempts to mount"),
            TEXT("animation system.md Section 7; mounted travel.md Section 2.2"),
            EAnimationProductionFamily::Mounted,
            TEXT("HighmooreHorseComponent"),
            TEXT("The first horse takes three authored attempts; no instant mount snap.")),
        Requirement(TEXT("anim.named.clean-weapons-stones"), TEXT("Cleaning weapons on the stones"),
            TEXT("animation system.md Section 7; the lake.md Section 5.3"),
            EAnimationProductionFamily::Interaction,
            TEXT("Princess quest authored world interaction"),
            TEXT("Real-duration cleaning interaction; no fade or time-compression substitute."))
    };
}

TArray<FAnimationProductionRequirement> FAnimationProductionCatalog::BuildSystemRequirements()
{
    return {
        Requirement(TEXT("anim.system.posture-five-states"), TEXT("Five posture visual states"),
            TEXT("animation system.md Section 2"),
            EAnimationProductionFamily::Combat,
            TEXT("CombatComponent / DarkArisenAnimInstance"),
            TEXT("Set, Pressed, Failing, Broken-imminent, Broken must remain readable without relying on HUD.")),
        Requirement(TEXT("anim.system.wounded-four-layers"), TEXT("Four wounded layers"),
            TEXT("animation system.md Section 5"),
            EAnimationProductionFamily::WoundDeterioration,
            TEXT("WoundStateComponent / DarkArisenAnimInstance"),
            TEXT("Winded, Hurt, Bad, Failing remain readable from behind/in rain/firelight; Katana suppresses wounded weapon-set swap.")),
        Requirement(TEXT("anim.system.attack-tell"), TEXT("Minimum attack tell"),
            TEXT("animation system.md Section 4.1"),
            EAnimationProductionFamily::Combat,
            TEXT("CombatComponent / animation notifies"),
            TEXT("Every attack silhouette changes at least eight frames before contact; six-frame deflection window remains unchanged.")),
        Requirement(TEXT("anim.system.non-cancellable-recovery"), TEXT("Committed attack recovery"),
            TEXT("animation system.md Section 1.1 and 3"),
            EAnimationProductionFamily::Combat,
            TEXT("CombatComponent / action montage ownership"),
            TEXT("Every attack completes its authored recovery; no cancel into dodge or block exists to speed combat up.")),
        Requirement(TEXT("anim.system.horse-gaits"), TEXT("Horse gait transitions"),
            TEXT("animation system.md Section 9.2; mounted travel.md Section 5.1"),
            EAnimationProductionFamily::Mounted,
            TEXT("HighmooreHorseComponent"),
            TEXT("Walk↔Trot, Trot↔Canter, Canter↔Gallop require real transition animations both directions.")),
        Requirement(TEXT("anim.system.horse-mood-body-read"), TEXT("Horse mood body read"),
            TEXT("animation system.md Section 9.1"),
            EAnimationProductionFamily::Mounted,
            TEXT("HighmooreHorseComponent / mounted animation graph"),
            TEXT("Mood reads through ears, head carriage, gait quality, mane and tail; no stamina/mood HUD number substitutes for it.")),
        Requirement(TEXT("anim.system.bad-rider-layer"), TEXT("Jake permanently worse rider layer"),
            TEXT("animation system.md Section 9.3"),
            EAnimationProductionFamily::Mounted,
            TEXT("HighmooreHorseComponent / mounted animation graph"),
            TEXT("Jake remains visibly worse on horseback next to skilled Highmoore riders.")),
        Requirement(TEXT("anim.system.finished-horse"), TEXT("Horse finished state at Belos"),
            TEXT("animation system.md Section 9.4; mounted travel.md Section 9.3"),
            EAnimationProductionFamily::Mounted,
            TEXT("HighmooreHorseComponent"),
            TEXT("Blown, head down, sides heaving; state holds through the nine-minute gateway wait.")),
        Requirement(TEXT("anim.system.world-interaction-reach"), TEXT("Physical world interaction reach"),
            TEXT("animation system.md Section 8"),
            EAnimationProductionFamily::Interaction,
            TEXT("InteractionComponent"),
            TEXT("Pickups, doors, ladders and object handling use visible interaction rather than fade-to-black.")),
        Requirement(TEXT("anim.system.people-carried"), TEXT("Full-body carried-person set"),
            TEXT("animation system.md Section 8; colony system core.md Section 3; castle ownership.md Section 4.2"),
            EAnimationProductionFamily::Interaction,
            TEXT("Population/voyage/holding interaction owners"),
            TEXT("People are physically carried with visible full-body weight; no teleport/fade substitutes for relocation interaction.")),
        Requirement(TEXT("anim.system.traversal-no-magnet"), TEXT("Traversal effort/no ledge magnetism"),
            TEXT("animation system.md Section 6.3; movement physics.md"),
            EAnimationProductionFamily::Traversal,
            TEXT("JakeCharacter traversal owner"),
            TEXT("Climb/vine/obstacle transitions show weight and may fail; no hidden ledge magnetism.")),
        Requirement(TEXT("anim.system.katana-clean-under-wounds"), TEXT("Katana clean-set exception"),
            TEXT("animation system.md Sections 3.1 and 5.3; crystal katana.md"),
            EAnimationProductionFamily::WoundDeterioration,
            TEXT("WoundStateComponent / CrystalKatanaComponent / DarkArisenAnimInstance"),
            TEXT("At every wound layer the Crystal Katana remains on its clean weapon set at the same authored speed; Jake's body still deteriorates."))
    };
}

TArray<FAnimationWeightTiming> FAnimationProductionCatalog::BuildWeightTimings()
{
    using namespace DarkArisen::CoreLoopTuning;
    return {
        WeightTiming(EAnimationWeaponWeightClass::Light, LightStartupFrames, LightRecoveryFrames),
        WeightTiming(EAnimationWeaponWeightClass::Medium, MediumStartupFrames, MediumRecoveryFrames),
        WeightTiming(EAnimationWeaponWeightClass::Heavy, HeavyStartupFrames, HeavyRecoveryFrames),
        WeightTiming(EAnimationWeaponWeightClass::Great, GreatStartupFrames, GreatRecoveryFrames),
        WeightTiming(EAnimationWeaponWeightClass::Polearm, PolearmStartupFrames, PolearmRecoveryFrames)
    };
}

TArray<FAnimationWoundLayerRequirement> FAnimationProductionCatalog::BuildWoundLayerRequirements()
{
    return {
        WoundRequirement(
            EAnimationWoundLayer::Winded,
            TEXT("Stamina < 30%"),
            TEXT("Breathing audible; guard slower to return.")),
        WoundRequirement(
            EAnimationWoundLayer::Hurt,
            TEXT("HP < 60%"),
            TEXT("Favoured side; off-hand guards a wound.")),
        WoundRequirement(
            EAnimationWoundLayer::Bad,
            TEXT("HP < 30%"),
            TEXT("Stance collapsed; walk becomes limp; run becomes stagger-run.")),
        WoundRequirement(
            EAnimationWoundLayer::Failing,
            TEXT("HP < 12%"),
            TEXT("Cannot sprint; weapon drags between swings."))
    };
}

TArray<FAnimationProductionDesignGap> FAnimationProductionCatalog::BuildDesignGaps()
{
    return {
        {
            TEXT("design-gap.animation-assets"),
            TEXT("Repository source defines animation contracts but does not contain a reviewed production animation asset set covering the named/system requirements."),
            TEXT("animation system.md; repository content audit")
        },
        {
            TEXT("design-gap.capture-source"),
            TEXT("Performance-capture versus authored-keyframe production source, performer/capture session and imported takes are not yet evidenced for P1/P2 sequences."),
            TEXT("cutscene catalog.md Sections 2 and 8; facial animation.md")
        },
        {
            TEXT("design-gap.exact-transition-timing"),
            TEXT("Many locomotion/mounted/world-interaction transitions are required but exact clip durations/blends are not locked canon values; they remain production tuning subject to weight/readability laws."),
            TEXT("animation system.md Sections 1, 6, 8, 9")
        }
    };
}

bool FAnimationProductionCatalog::Validate(TArray<FString>& OutErrors)
{
    OutErrors.Reset();

    const TArray<FAnimationProductionRequirement> Named = BuildNamedBibleRequirements();
    if (Named.Num() != NamedBibleRequirementCount)
    {
        OutErrors.Add(FString::Printf(TEXT("Named animation production list requires exactly %d entries; found %d."), NamedBibleRequirementCount, Named.Num()));
    }

    TSet<FName> Seen;
    const auto ValidateRequirement = [&Seen, &OutErrors](const FAnimationProductionRequirement& RequirementValue)
    {
        if (RequirementValue.StableId.IsNone()
            || RequirementValue.DisplayName.IsEmpty()
            || RequirementValue.GoverningSource.IsEmpty()
            || RequirementValue.GameplayAuthority.IsEmpty()
            || RequirementValue.AcceptanceRead.IsEmpty())
        {
            OutErrors.Add(TEXT("Every animation production requirement needs id, name, source, authority and acceptance read."));
        }
        if (Seen.Contains(RequirementValue.StableId))
        {
            OutErrors.Add(FString::Printf(TEXT("Duplicate animation production id: %s"), *RequirementValue.StableId.ToString()));
        }
        Seen.Add(RequirementValue.StableId);
        if (RequirementValue.bAssetAuthored || !RequirementValue.AssetPath.IsEmpty())
        {
            OutErrors.Add(FString::Printf(TEXT("Animation requirement %s may not claim an asset until an imported/reviewed asset exists."), *RequirementValue.StableId.ToString()));
        }
    };

    for (const FAnimationProductionRequirement& RequirementValue : Named)
    {
        ValidateRequirement(RequirementValue);
    }
    for (const FAnimationProductionRequirement& RequirementValue : BuildSystemRequirements())
    {
        ValidateRequirement(RequirementValue);
    }

    const TArray<FAnimationWeightTiming> WeightTimings = BuildWeightTimings();
    if (WeightTimings.Num() != RequiredWeightClassCount)
    {
        OutErrors.Add(FString::Printf(TEXT("Animation timing contract requires exactly %d weight classes; found %d."), RequiredWeightClassCount, WeightTimings.Num()));
    }
    const int32 ExpectedStartup[] = {9, 13, 19, 26, 15};
    const int32 ExpectedRecovery[] = {14, 20, 31, 44, 24};
    for (int32 Index = 0; Index < WeightTimings.Num() && Index < RequiredWeightClassCount; ++Index)
    {
        if (WeightTimings[Index].StartupFrames != ExpectedStartup[Index]
            || WeightTimings[Index].RecoveryFrames != ExpectedRecovery[Index]
            || WeightTimings[Index].GoverningSource.IsEmpty())
        {
            OutErrors.Add(FString::Printf(TEXT("Animation weight timing %d no longer matches the authored frame table."), Index));
        }
    }

    const TArray<FAnimationWoundLayerRequirement> WoundLayers = BuildWoundLayerRequirements();
    if (WoundLayers.Num() != RequiredWoundLayerCount)
    {
        OutErrors.Add(FString::Printf(TEXT("Animation wound contract requires exactly %d layers; found %d."), RequiredWoundLayerCount, WoundLayers.Num()));
    }
    for (const FAnimationWoundLayerRequirement& WoundLayer : WoundLayers)
    {
        if (WoundLayer.Trigger.IsEmpty() || WoundLayer.RequiredBodyRead.IsEmpty() || WoundLayer.GoverningSource.IsEmpty())
        {
            OutErrors.Add(TEXT("Every wounded animation layer requires a source trigger and visible body read."));
        }
    }

    if (MinimumAttackTellFrames != 8 || LockedDeflectionWindowFrames != 6)
    {
        OutErrors.Add(TEXT("Animation readability contract must remain eight-frame minimum tell and six-frame deflection."));
    }
    if (RequiredHorseGaitTransitionCount != 6)
    {
        OutErrors.Add(TEXT("Four horse gaits require six adjacent directional transitions."));
    }
    if (BuildDesignGaps().Num() != 3)
    {
        OutErrors.Add(TEXT("Animation production must preserve the three current asset/capture/tuning gaps."));
    }

    return OutErrors.IsEmpty();
}
