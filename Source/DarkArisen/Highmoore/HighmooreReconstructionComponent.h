// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Highmoore/PrincessQuestStateComponent.h"
#include "HighmooreReconstructionComponent.generated.h"

UENUM(BlueprintType)
enum class EHighmooreReconstructionPath : uint8
{
    NotAvailableYet,
    StandardAfterBelos,
    TurnedWest,
    PermanentlyUnavailableAfterStair
};

UENUM(BlueprintType)
enum class EHighmooreReconstructionProject : uint8
{
    ClearBelosWesternRange,
    RestoreBelosBowyer,
    SwearRoads,
    BuildFordBridge,
    RestoreMillsAndVillages,
    ReclaimBurntMarch,
    RestoreArionGreatHall,
    FillArionGalleryGap,
    FillRabbitHutches,
    ConnectArchipelagoSupplyLine
};

/**
 * Runtime state for `thread.highmoore.reconstruction`.
 *
 * This component does not award forgiveness, redemption, lordship or a moral score. It records only
 * material/aesthetic reconstruction facts proven by authored gameplay. Costs and build durations are
 * intentionally absent where the design source gives no exact values.
 */
UCLASS(ClassGroup=(DarkArisen), meta=(BlueprintSpawnableComponent))
class DARKARISEN_API UHighmooreReconstructionComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UHighmooreReconstructionComponent();

    static const FName ThreadId;

    /** May initialize only after the Princess return has reached the dock. */
    UFUNCTION(BlueprintCallable, Category="Highmoore|Reconstruction")
    bool InitializeFromPrincessState(const UPrincessQuestStateComponent* PrincessState);

    UFUNCTION(BlueprintPure, Category="Highmoore|Reconstruction")
    bool IsReconstructionAvailable() const;

    UFUNCTION(BlueprintPure, Category="Highmoore|Reconstruction")
    bool IsProjectAvailable(EHighmooreReconstructionProject Project) const;

    /** The ford bridge specifically requires Cadwall's masons. */
    UFUNCTION(BlueprintCallable, Category="Highmoore|Reconstruction")
    bool RecordCadwallMasonsSecured();

    /**
     * Records completion only after the external authored construction/level owner supplies proof.
     * This component never manufactures costs, timers or instant-build actions.
     */
    UFUNCTION(BlueprintCallable, Category="Highmoore|Reconstruction")
    bool RecordProjectCompleted(EHighmooreReconstructionProject Project, bool bExternalConstructionProof);

    UFUNCTION(BlueprintPure, Category="Highmoore|Reconstruction")
    bool IsProjectCompleted(EHighmooreReconstructionProject Project) const;

    /** If the real letter was missed, it remains on the pillow until the player physically returns. */
    UFUNCTION(BlueprintPure, Category="Highmoore|Reconstruction")
    bool IsRealLetterStillOnPillow() const { return bRealLetterStillOnPillow; }

    UFUNCTION(BlueprintCallable, Category="Highmoore|Reconstruction")
    bool RecordRealLetterReadLater();

    UFUNCTION(BlueprintPure, Category="Highmoore|Reconstruction|DesignLaw")
    bool AllowsRedemptionMeter() const { return false; }

    UFUNCTION(BlueprintPure, Category="Highmoore|Reconstruction|DesignLaw")
    bool AllowsForgivenessReward() const { return false; }

    UFUNCTION(BlueprintPure, Category="Highmoore|Reconstruction|DesignLaw")
    bool MakesJakeLordOfHighmoore() const { return false; }

    UFUNCTION(BlueprintPure, Category="Highmoore|Reconstruction|DesignLaw")
    bool RewardsGalleryGap() const { return false; }

    UFUNCTION(BlueprintPure, Category="Highmoore|Reconstruction|DesignLaw")
    bool RewardsRabbitHutches() const { return false; }

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame, Category="Highmoore|Reconstruction")
    EHighmooreReconstructionPath Path = EHighmooreReconstructionPath::NotAvailableYet;

private:
    UPROPERTY(SaveGame)
    TSet<EHighmooreReconstructionProject> CompletedProjects;

    UPROPERTY(SaveGame)
    bool bCadwallMasonsSecured = false;

    UPROPERTY(SaveGame)
    bool bRealLetterStillOnPillow = false;

    UPROPERTY(SaveGame)
    bool bInitialized = false;
};
