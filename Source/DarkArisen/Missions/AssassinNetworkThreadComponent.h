// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ColonialWar/RetaliationSubsystem.h"
#include "AssassinNetworkThreadComponent.generated.h"

UENUM(BlueprintType)
enum class EAssassinHunterOrganisation : uint8
{
    Cofradia,
    Regulators
};

UENUM(BlueprintType)
enum class EAssassinHunterOutcome : uint8
{
    Killed,
    Spared,
    BoughtOff
};

UENUM(BlueprintType)
enum class EAssassinThreadState : uint8
{
    Dormant,
    CrewScenePending,
    Investigation,
    Resolved
};

UENUM(BlueprintType)
enum class ECofradiaInvestigationState : uint8
{
    NotStarted,
    ChapterHousesIdentified,
    ThreeHousesInfiltrated,
    WritLedgerRecovered,
    PrimeraRevealed,
    ResolvedDestroyed,
    ResolvedWithdrawn
};

UENUM(BlueprintType)
enum class ECofradiaChapterHouse : uint8
{
    SanBenito,
    ValdenaEnMar,
    PaleIsle
};

UENUM(BlueprintType)
enum class ERegulatorInvestigationState : uint8
{
    NotStarted,
    FileReferencesDecoded,
    CoventryHouseInfiltrated,
    EngagementRegisterRecovered,
    KettleMeeting,
    ResolvedDestroyed,
    ResolvedTurned
};

USTRUCT(BlueprintType)
struct FAssassinHunterRecord
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame)
    FName HunterId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame)
    EAssassinHunterOrganisation Organisation = EAssassinHunterOrganisation::Cofradia;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame)
    EAssassinHunterOutcome Outcome = EAssassinHunterOutcome::Killed;
};

USTRUCT(BlueprintType)
struct FCofradiaMarkEvidence
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame)
    FName EvidenceId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame)
    FName HunterId;

    /** Authored chapter-house mark/cipher identity. The component invents no mark names. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame)
    FName ChapterHouseMarkId;
};

/**
 * Phase-11 Thread state for `thread.archipelago.assassin-network`.
 *
 * This component owns only state that the Assassin Network sources explicitly define. It does not
 * author journal prose, dialogue, money figures, infiltration geometry, hunter AI or cinematic
 * presentation. Those remain authored content gates in their existing owners.
 */
UCLASS(ClassGroup=(DarkArisen), meta=(BlueprintSpawnableComponent))
class DARKARISEN_API UAssassinNetworkThreadComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAssassinNetworkThreadComponent();

    static const FName ThreadId;

    /** Records one named hunter the player survived by a source-approved outcome. */
    UFUNCTION(BlueprintCallable, Category="Quest|Assassin Network")
    bool RecordNamedHunterOutcome(FName HunterId, EAssassinHunterOrganisation Organisation,
        EAssassinHunterOutcome Outcome);

    /** Spared/captured hunters may substitute for an empty collected-evidence trail. */
    UFUNCTION(BlueprintCallable, Category="Quest|Assassin Network")
    bool RecordCapturedHunterSource(FName HunterId, EAssassinHunterOrganisation Organisation);

    /** Entry requires historical Retaliation Stage 3+, three named hunters and Momentum Beta+. */
    UFUNCTION(BlueprintCallable, Category="Quest|Assassin Network")
    bool RefreshEntryEligibility();

    /** Completes the authored great-cabin instigating scene and opens the investigation layer. */
    UFUNCTION(BlueprintCallable, Category="Quest|Assassin Network")
    bool CompleteCrewInstigatingScene();

    UFUNCTION(BlueprintCallable, Category="Quest|Assassin Network|Cofradia")
    bool RecordCofradiaChapterMark(FName EvidenceId, FName HunterId, FName ChapterHouseMarkId);

    /** Three distinct hunter/mark pairs, or a captured Cofradia source, identify the houses. */
    UFUNCTION(BlueprintCallable, Category="Quest|Assassin Network|Cofradia")
    bool IdentifyCofradiaChapterHouses();

    UFUNCTION(BlueprintCallable, Category="Quest|Assassin Network|Cofradia")
    bool RecordCofradiaHouseInfiltrated(ECofradiaChapterHouse House);

    UFUNCTION(BlueprintCallable, Category="Quest|Assassin Network|Cofradia")
    bool RecordWritLedgerRecovered();

    UFUNCTION(BlueprintCallable, Category="Quest|Assassin Network|Cofradia")
    bool RecordPrimeraRevealedAsVidal();

    /** May occur during Vidal's earlier hunter encounter, before this Thread opens. */
    UFUNCTION(BlueprintCallable, Category="Quest|Assassin Network|Cofradia")
    bool RecordVidalConfessionTaken();

    /** Destroy route: ledger must be published and Vidal's final authored encounter resolved. */
    UFUNCTION(BlueprintCallable, Category="Quest|Assassin Network|Cofradia")
    bool ResolveCofradiaByPublication(bool bLedgerPublishedThroughReformNetwork,
        bool bVidalFinalEncounterResolved);

    /** Turn route: only the genuine earlier confession can open Vidal's withdrawal. */
    UFUNCTION(BlueprintCallable, Category="Quest|Assassin Network|Cofradia")
    bool ResolveCofradiaByWithdrawal(bool bVidalWithdrewWritAsUnrighteous);

    UFUNCTION(BlueprintCallable, Category="Quest|Assassin Network|Regulators")
    bool RecordRegulatorFileReferencesDecoded();

    UFUNCTION(BlueprintCallable, Category="Quest|Assassin Network|Regulators")
    bool RecordCoventryHouseStationInfiltrated();

    UFUNCTION(BlueprintCallable, Category="Quest|Assassin Network|Regulators")
    bool RecordEngagementRegisterRecovered(bool bUnactionedContractRecovered);

    /** Kettle's office remains player-paced dialogue, not a cutscene. */
    UFUNCTION(BlueprintCallable, Category="Quest|Assassin Network|Regulators")
    bool RecordKettleMeetingCompleted();

    /** Destroy route also destroys the settlement receipts by canon. */
    UFUNCTION(BlueprintCallable, Category="Quest|Assassin Network|Regulators")
    bool ResolveRegulatorsByDestruction(bool bStationDestroyed, bool bKettleKilledOrRouted,
        bool bSettlementReceiptsDestroyed);

    /**
     * Turn route requires proof that all contracts, operating costs and retainer were actually paid.
     * The design does not supply a numeric figure, so this component deliberately contains none.
     */
    UFUNCTION(BlueprintCallable, Category="Quest|Assassin Network|Regulators")
    bool ResolveRegulatorsBySettlement(bool bAllContractsPaid, bool bOperatingCostsPaid,
        bool bRetainerPaid);

    UFUNCTION(BlueprintPure, Category="Quest|Assassin Network")
    bool AreCofradiaHuntersActive() const;

    UFUNCTION(BlueprintPure, Category="Quest|Assassin Network")
    bool AreRegulatorHuntersActive() const;

    UFUNCTION(BlueprintPure, Category="Quest|Assassin Network")
    bool IsRegulatorWarningNetworkAvailable() const;

    UFUNCTION(BlueprintPure, Category="Quest|Assassin Network")
    int32 GetNamedHunterSurvivalCount() const { return NamedHunters.Num(); }

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame, Category="Quest|Assassin Network")
    EAssassinThreadState ThreadState = EAssassinThreadState::Dormant;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame, Category="Quest|Assassin Network")
    ECofradiaInvestigationState CofradiaState = ECofradiaInvestigationState::NotStarted;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame, Category="Quest|Assassin Network")
    ERegulatorInvestigationState RegulatorState = ERegulatorInvestigationState::NotStarted;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame, Category="Quest|Assassin Network")
    TArray<FAssassinHunterRecord> NamedHunters;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame, Category="Quest|Assassin Network")
    TArray<FCofradiaMarkEvidence> CofradiaMarks;

private:
    UPROPERTY(SaveGame)
    TSet<FName> CapturedCofradiaSources;

    UPROPERTY(SaveGame)
    TSet<FName> CapturedRegulatorSources;

    UPROPERTY(SaveGame)
    TSet<ECofradiaChapterHouse> InfiltratedCofradiaHouses;

    UPROPERTY(SaveGame)
    bool bVidalConfessionTaken = false;

    UPROPERTY(SaveGame)
    bool bUnactionedRegulatorContractRecovered = false;

    UPROPERTY(SaveGame)
    bool bSettlementReceiptsDestroyed = false;

    bool IsInvestigationOpen() const;
    bool IsCofradiaResolved() const;
    bool IsRegulatorResolved() const;
    void RefreshOverallResolution();
};
