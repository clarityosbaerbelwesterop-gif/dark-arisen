// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ArchipelagoThreadsComponent.generated.h"

UENUM(BlueprintType)
enum class EArchipelagoThreadLifecycle : uint8
{
    Dormant,
    Active,
    AwaitingAuthoredDetail,
    Resolved,
    Expired
};

UENUM(BlueprintType)
enum class EKesslerInvestigationStage : uint8
{
    Dormant,
    InitialTrust,
    BribeOfficerInvestigated,
    NobleNetworkInvestigated,
    CrimsonProtectionInvestigated,
    CastilloDoradoLedgersRecovered,
    Resolved,
    Expired
};

UENUM(BlueprintType)
enum class ELedgerTrilogyRecord : uint8
{
    DrownedPlantation,
    ValdenaMine,
    FuerteEsperanza,
    CompanyWorks
};

UENUM(BlueprintType)
enum class ESchreiberDocumentationStage : uint8
{
    Dormant,
    ResearchRelationshipOpened,
    EthicalMethodEstablished,
    HarbourThatWasFirstReached,
    BoatsDocumentedAndLeft,
    FirstHarbourRecordRecovered,
    Resolved
};

UENUM(BlueprintType)
enum class EOldFortressSite : uint8
{
    LaGuardiaVieja,
    DrownedHold,
    AshBastion,
    FourthFortress
};

/**
 * Runtime authority for the Phase-11 archipelago Threads whose sources currently contain enough
 * concrete state to encode without writing new narrative. Kessler, Ledger Trilogy, Schreiber and
 * Old Fortresses are source-grounded. Captain Vasquez has a named six-hour Thread and an expiry,
 * but her actual multi-stage content remains generic in the cited source, so it fails closed after
 * activation rather than generating captain-peer filler.
 */
UCLASS(ClassGroup=(DarkArisen), meta=(BlueprintSpawnableComponent))
class DARKARISEN_API UArchipelagoThreadsComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UArchipelagoThreadsComponent();

    static const FName KesslerThreadId;
    static const FName LedgerThreadId;
    static const FName SchreiberThreadId;
    static const FName VasquezThreadId;
    static const FName OldFortressesThreadId;

    // Kessler — internal Imperial reform investigation.
    UFUNCTION(BlueprintCallable, Category="Quest|Archipelago|Kessler")
    bool ActivateKesslerInvestigation(bool bInitialTrustEstablished);

    UFUNCTION(BlueprintCallable, Category="Quest|Archipelago|Kessler")
    bool RecordKesslerBribeOfficerInvestigated();

    UFUNCTION(BlueprintCallable, Category="Quest|Archipelago|Kessler")
    bool RecordKesslerNobleNetworkInvestigated();

    UFUNCTION(BlueprintCallable, Category="Quest|Archipelago|Kessler")
    bool RecordKesslerCrimsonProtectionInvestigated();

    UFUNCTION(BlueprintCallable, Category="Quest|Archipelago|Kessler")
    bool RecordCastilloDoradoBribeLedgersRecovered();

    /** Exact world-state expiry: Castillo Dorado falls OR de Silva dies. No countdown is created. */
    UFUNCTION(BlueprintCallable, Category="Quest|Archipelago|Kessler")
    bool RecordKesslerExpiryWorldState(bool bCastilloDoradoFell, bool bDeSilvaDied);

    // Ledger Trilogy — four unrelated record sets become one family-reconnection chain.
    UFUNCTION(BlueprintCallable, Category="Quest|Archipelago|Ledger Trilogy")
    bool RecordLedgerRecovered(ELedgerTrilogyRecord Record);

    UFUNCTION(BlueprintPure, Category="Quest|Archipelago|Ledger Trilogy")
    bool IsLedgerTrilogyResolved() const { return LedgerLifecycle == EArchipelagoThreadLifecycle::Resolved; }

    // Doctor Schreiber — documentation, not extraction.
    UFUNCTION(BlueprintCallable, Category="Quest|Archipelago|Schreiber")
    bool ActivateSchreiberDocumentation(bool bResearchRelationshipOpened);

    UFUNCTION(BlueprintCallable, Category="Quest|Archipelago|Schreiber")
    bool RecordSchreiberEthicalMethodEstablished();

    UFUNCTION(BlueprintCallable, Category="Quest|Archipelago|Schreiber")
    bool RecordHarbourThatWasFirstReached();

    UFUNCTION(BlueprintCallable, Category="Quest|Archipelago|Schreiber")
    bool RecordFirstHarbourBoatsDocumentedAndLeftUntaken();

    UFUNCTION(BlueprintCallable, Category="Quest|Archipelago|Schreiber")
    bool RecordFirstHarbourDatableRecordRecovered();

    // Captain Vasquez — identity/expiry are known, exact six-hour authored stage chain is not.
    UFUNCTION(BlueprintCallable, Category="Quest|Archipelago|Vasquez")
    bool ActivateVasquezThread(bool bCaptainPeerRelationshipOpened);

    UFUNCTION(BlueprintCallable, Category="Quest|Archipelago|Vasquez")
    bool RecordVasquezPrivateEngagementEvidence(bool bEvidenceNamesVasquez);

    UFUNCTION(BlueprintCallable, Category="Quest|Archipelago|Vasquez")
    bool RecordVasquezSunk();

    // Old Fortresses — exactly four Alliance-gated physical reclamations.
    UFUNCTION(BlueprintCallable, Category="Quest|Archipelago|Old Fortresses")
    bool RecordOldFortressReclaimed(EOldFortressSite Site, bool bAllianceAccessProven,
        bool bPhysicalReclamationCompleted);

    UFUNCTION(BlueprintPure, Category="Quest|Archipelago|Old Fortresses")
    int32 GetReclaimedOldFortressCount() const { return ReclaimedOldFortresses.Num(); }

    UFUNCTION(BlueprintPure, Category="Quest|Archipelago")
    EArchipelagoThreadLifecycle GetKesslerLifecycle() const { return KesslerLifecycle; }

    UFUNCTION(BlueprintPure, Category="Quest|Archipelago")
    EArchipelagoThreadLifecycle GetLedgerLifecycle() const { return LedgerLifecycle; }

    UFUNCTION(BlueprintPure, Category="Quest|Archipelago")
    EArchipelagoThreadLifecycle GetSchreiberLifecycle() const { return SchreiberLifecycle; }

    UFUNCTION(BlueprintPure, Category="Quest|Archipelago")
    EArchipelagoThreadLifecycle GetVasquezLifecycle() const { return VasquezLifecycle; }

    UFUNCTION(BlueprintPure, Category="Quest|Archipelago")
    EArchipelagoThreadLifecycle GetOldFortressesLifecycle() const { return OldFortressesLifecycle; }

    UFUNCTION(BlueprintPure, Category="Quest|Archipelago|Kessler")
    EKesslerInvestigationStage GetKesslerStage() const { return KesslerStage; }

    UFUNCTION(BlueprintPure, Category="Quest|Archipelago|Schreiber")
    ESchreiberDocumentationStage GetSchreiberStage() const { return SchreiberStage; }

    UFUNCTION(BlueprintPure, Category="Quest|Archipelago|Vasquez")
    bool HasVasquezPrivateEngagementEvidence() const { return bVasquezNamedOnPrivateEngagement; }

    UFUNCTION(BlueprintPure, Category="Quest|Archipelago|Design Law")
    bool AllowsGeneratedThreadStages() const { return false; }

    UFUNCTION(BlueprintPure, Category="Quest|Archipelago|Design Law")
    bool AllowsQuestExpiryTimerUI() const { return false; }

private:
    UPROPERTY(SaveGame)
    EArchipelagoThreadLifecycle KesslerLifecycle = EArchipelagoThreadLifecycle::Dormant;

    UPROPERTY(SaveGame)
    EKesslerInvestigationStage KesslerStage = EKesslerInvestigationStage::Dormant;

    UPROPERTY(SaveGame)
    EArchipelagoThreadLifecycle LedgerLifecycle = EArchipelagoThreadLifecycle::Dormant;

    UPROPERTY(SaveGame)
    TSet<ELedgerTrilogyRecord> RecoveredLedgers;

    UPROPERTY(SaveGame)
    EArchipelagoThreadLifecycle SchreiberLifecycle = EArchipelagoThreadLifecycle::Dormant;

    UPROPERTY(SaveGame)
    ESchreiberDocumentationStage SchreiberStage = ESchreiberDocumentationStage::Dormant;

    UPROPERTY(SaveGame)
    EArchipelagoThreadLifecycle VasquezLifecycle = EArchipelagoThreadLifecycle::Dormant;

    UPROPERTY(SaveGame)
    bool bVasquezNamedOnPrivateEngagement = false;

    UPROPERTY(SaveGame)
    EArchipelagoThreadLifecycle OldFortressesLifecycle = EArchipelagoThreadLifecycle::Dormant;

    UPROPERTY(SaveGame)
    TSet<EOldFortressSite> ReclaimedOldFortresses;
};
