// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CrewPersonalThreadsComponent.generated.h"

class UProgressionEconomyComponent;

UENUM(BlueprintType)
enum class ECrewPersonalThread : uint8
{
    InesEsperanza,
    MirasCoast,
    BigTomsService,
    FatherSalviosParish,
    EstebansLastChart
};

UENUM(BlueprintType)
enum class ECrewThreadLifecycle : uint8
{
    Dormant,
    Active,
    AwaitingAuthoredDetail,
    Resolved,
    Expired
};

UENUM(BlueprintType)
enum class EMiraThreadStage : uint8
{
    Dormant,
    ActivatedInFjordlund,
    FirstLead,
    SearchUnderway,
    CrucialLead,
    ArmadaInvestigation,
    Truth,
    Resolved,
    Expired
};

UENUM(BlueprintType)
enum class EMiraBrotherOutcome : uint8
{
    None,
    Dead,
    AliveCaptive,
    JoinedCrimson
};

UENUM(BlueprintType)
enum class EInesThreadStage : uint8
{
    Dormant,
    ActivatedAfterThirdMeeting,
    PortraitShared,
    FirstLead,
    ImperialRecords,
    CrimsonTrail,
    HouseholdLocated,
    Truth,
    Resolved
};

UENUM(BlueprintType)
enum class EEsperanzaOutcome : uint8
{
    None,
    AliveAdoptedWell,
    AliveInServitude,
    Dead
};

UENUM(BlueprintType)
enum class EBigTomThreadStage : uint8
{
    Dormant,
    RecognisedAbsence,
    ServiceStory,
    TrackingReinhardt,
    ChoiceFormed,
    Confrontation,
    Resolved
};

UENUM(BlueprintType)
enum class EBigTomServiceOutcome : uint8
{
    None,
    Forgiveness,
    Justice,
    Vengeance,
    WalkAway
};

USTRUCT(BlueprintType)
struct FCrewThreadRuntimeState
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame)
    ECrewPersonalThread Thread = ECrewPersonalThread::InesEsperanza;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame)
    ECrewThreadLifecycle Lifecycle = ECrewThreadLifecycle::Dormant;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame)
    bool bCompletionMarksClaimed = false;
};

/**
 * Native owner for the five Phase-11 named-crew Threads.
 *
 * The newer `side quest catalog.md` owns identity, activation, duration, expiry and two-Mark reward.
 * Compatible detail from `named_crew_deep_dives.md` is used only where it does not contradict that
 * later authority. Father Salvio's newer parish Thread and Esteban's newer last-chart Thread do not
 * have enough authored stage detail to manufacture a full questline, so their source-backed
 * activations deliberately stop at AwaitingAuthoredDetail.
 */
UCLASS(ClassGroup=(DarkArisen), meta=(BlueprintSpawnableComponent))
class DARKARISEN_API UCrewPersonalThreadsComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCrewPersonalThreadsComponent();

    static constexpr int32 CompletionMarksPerCrewThread = 2;

    // Ines — Chapter 5+, third meeting, then the source-backed Esperanza investigation.
    UFUNCTION(BlueprintCallable, Category="Quest|Crew|Ines")
    bool ActivateInesThread(int32 CurrentChapter, int32 MeetingCount);

    UFUNCTION(BlueprintCallable, Category="Quest|Crew|Ines")
    bool RecordInesPortraitShared();

    UFUNCTION(BlueprintCallable, Category="Quest|Crew|Ines")
    bool RecordInesFirstLead();

    UFUNCTION(BlueprintCallable, Category="Quest|Crew|Ines")
    bool RecordInesImperialRecordsFound();

    UFUNCTION(BlueprintCallable, Category="Quest|Crew|Ines")
    bool RecordInesCrimsonTrailFound();

    UFUNCTION(BlueprintCallable, Category="Quest|Crew|Ines")
    bool RecordInesHouseholdLocated();

    UFUNCTION(BlueprintCallable, Category="Quest|Crew|Ines")
    bool ResolveInesTruth(EEsperanzaOutcome Outcome);

    // Mira — activation is the Phase-11 overheard event in Fjordlund. No timer/warning UI exists.
    UFUNCTION(BlueprintCallable, Category="Quest|Crew|Mira")
    bool ActivateMiraThread(bool bOverheardInFjordlund, bool bFjordlundControlAlreadyResolved);

    UFUNCTION(BlueprintCallable, Category="Quest|Crew|Mira")
    bool RecordMiraFirstLead();

    UFUNCTION(BlueprintCallable, Category="Quest|Crew|Mira")
    bool RecordMiraSearchUnderway();

    UFUNCTION(BlueprintCallable, Category="Quest|Crew|Mira")
    bool RecordMiraCrucialLead();

    UFUNCTION(BlueprintCallable, Category="Quest|Crew|Mira")
    bool RecordMiraArmadaInvestigation();

    UFUNCTION(BlueprintCallable, Category="Quest|Crew|Mira")
    bool ResolveMiraTruth(EMiraBrotherOutcome Outcome);

    /** World-state expiry from side quest catalog Section 3. Resolved Threads stay resolved. */
    UFUNCTION(BlueprintCallable, Category="Quest|Crew|Mira")
    bool RecordFjordlundControlResolved();

    // Big Tom — activation by absence: a recogniser sees him and leaves.
    UFUNCTION(BlueprintCallable, Category="Quest|Crew|Big Tom")
    bool ActivateBigTomThread(bool bRecogniserSawTomAndLeft);

    UFUNCTION(BlueprintCallable, Category="Quest|Crew|Big Tom")
    bool RecordBigTomServiceStory();

    UFUNCTION(BlueprintCallable, Category="Quest|Crew|Big Tom")
    bool RecordBigTomTrackingReinhardt();

    UFUNCTION(BlueprintCallable, Category="Quest|Crew|Big Tom")
    bool RecordBigTomChoiceFormed();

    UFUNCTION(BlueprintCallable, Category="Quest|Crew|Big Tom")
    bool RecordBigTomConfrontationReached();

    UFUNCTION(BlueprintCallable, Category="Quest|Crew|Big Tom")
    bool ResolveBigTomService(EBigTomServiceOutcome Outcome);

    // Salvio — exact later activation is known; later parish stages are not sufficiently authored.
    UFUNCTION(BlueprintCallable, Category="Quest|Crew|Salvio")
    bool ActivateSalvioParishThread(bool bJakeHasSparedSomeone, bool bConversationCompleted);

    // Esteban — exact later activation is the physical chart in his quarters; destination/content is withheld.
    UFUNCTION(BlueprintCallable, Category="Quest|Crew|Esteban")
    bool ActivateEstebanLastChartThread(bool bPhysicalChartFoundInQuarters);

    /** Awards the exact two Marks only after a Thread has genuinely resolved. */
    UFUNCTION(BlueprintCallable, Category="Quest|Crew|Reward")
    bool ClaimCompletionMarks(ECrewPersonalThread Thread, UProgressionEconomyComponent* Progression);

    UFUNCTION(BlueprintPure, Category="Quest|Crew")
    ECrewThreadLifecycle GetLifecycle(ECrewPersonalThread Thread) const;

    UFUNCTION(BlueprintPure, Category="Quest|Crew|Mira")
    EMiraThreadStage GetMiraStage() const { return MiraStage; }

    UFUNCTION(BlueprintPure, Category="Quest|Crew|Mira")
    EMiraBrotherOutcome GetMiraOutcome() const { return MiraOutcome; }

    UFUNCTION(BlueprintPure, Category="Quest|Crew|Ines")
    EInesThreadStage GetInesStage() const { return InesStage; }

    UFUNCTION(BlueprintPure, Category="Quest|Crew|Ines")
    EEsperanzaOutcome GetEsperanzaOutcome() const { return EsperanzaOutcome; }

    UFUNCTION(BlueprintPure, Category="Quest|Crew|Big Tom")
    EBigTomThreadStage GetBigTomStage() const { return BigTomStage; }

    UFUNCTION(BlueprintPure, Category="Quest|Crew|Big Tom")
    EBigTomServiceOutcome GetBigTomOutcome() const { return BigTomOutcome; }

    UFUNCTION(BlueprintPure, Category="Quest|Crew|Design Law")
    bool AllowsGeneratedCrewQuestStages() const { return false; }

    UFUNCTION(BlueprintPure, Category="Quest|Crew|Design Law")
    bool AllowsSystemicRomance() const { return false; }

private:
    FCrewThreadRuntimeState* FindStateMutable(ECrewPersonalThread Thread);
    const FCrewThreadRuntimeState* FindState(ECrewPersonalThread Thread) const;
    bool SetResolved(ECrewPersonalThread Thread);
    static FName MarkSourceId(ECrewPersonalThread Thread);

    UPROPERTY(SaveGame)
    TArray<FCrewThreadRuntimeState> ThreadStates;

    UPROPERTY(SaveGame)
    EMiraThreadStage MiraStage = EMiraThreadStage::Dormant;

    UPROPERTY(SaveGame)
    EMiraBrotherOutcome MiraOutcome = EMiraBrotherOutcome::None;

    UPROPERTY(SaveGame)
    EInesThreadStage InesStage = EInesThreadStage::Dormant;

    UPROPERTY(SaveGame)
    EEsperanzaOutcome EsperanzaOutcome = EEsperanzaOutcome::None;

    UPROPERTY(SaveGame)
    EBigTomThreadStage BigTomStage = EBigTomThreadStage::Dormant;

    UPROPERTY(SaveGame)
    EBigTomServiceOutcome BigTomOutcome = EBigTomServiceOutcome::None;
};
