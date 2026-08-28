// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Rexa/RexaSettlementRoster.h"
#include "RexaSettlementResident.generated.h"

class UStaticMeshComponent;
class ARexaSettlementAnchor;

UENUM(BlueprintType)
enum class ERexaResidentSafetyState : uint8
{
    Routine,
    FleeingCombat,
    ShelteredOffscreen
};

/**
 * Lightweight non-combatant used by the authored Las Raíces roster.
 * Final character art, navigation and ambient performances remain level/content work.
 */
UCLASS()
class DARKARISEN_API ARexaSettlementResident : public ACharacter
{
    GENERATED_BODY()

public:
    ARexaSettlementResident();

    virtual void LaunchCharacter(
        FVector LaunchVelocity,
        bool bXYOverride,
        bool bZOverride) override;

    virtual float TakeDamage(
        float DamageAmount,
        const FDamageEvent& DamageEvent,
        AController* EventInstigator,
        AActor* DamageCauser) override;

    bool InitializeFromDefinition(const FRexaResidentDefinition& Definition);

    UFUNCTION(BlueprintCallable, Category = "Rexa|Schedule")
    FName RefreshPurposeAnchor(int64 GameMinute);

    bool MoveToPurposeAnchor(int64 GameMinute, ARexaSettlementAnchor* Anchor);
    void ClearPurposeRoute();

    bool EnterProtectedChildFlee(
        const FVector& CombatLocation,
        ARexaSettlementAnchor* SafetyAnchor);
    bool ShelterProtectedChild(ARexaSettlementAnchor* SafetyAnchor);
    bool RestoreProtectedChildAfterCombat(ARexaSettlementAnchor* SafetyAnchor);

    UFUNCTION(BlueprintPure, Category = "Rexa|Safety")
    bool IsProtectedChildRuntime() const;

    const FRexaResidentDefinition& GetResidentDefinition() const
    {
        return ResidentDefinition;
    }

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rexa|Presentation")
    TObjectPtr<UStaticMeshComponent> GreyboxBody;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Rexa|Identity")
    FRexaResidentDefinition ResidentDefinition;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Rexa|Schedule")
    FName CurrentPurposeAnchorId;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rexa|Schedule")
    float PurposeAnchorAcceptanceRadiusCentimetres = 90.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rexa|Schedule")
    float RoutineWalkSpeedCentimetresPerSecond = 180.0f;

    /** DESIGN-GAP: authored as maximum-speed flight; exact speed awaits controller playtest. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rexa|Safety")
    float ChildFleeSpeedCentimetresPerSecond = 520.0f;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Rexa|Safety")
    ERexaResidentSafetyState CurrentSafetyState = ERexaResidentSafetyState::Routine;

    static constexpr float ChildCombatFleeRadiusCentimetres = 5000.0f;

private:
    bool bDefinitionInitialized = false;

    void ApplyNonCombatantPolicy();
    bool RequestMoveToAnchor(ARexaSettlementAnchor* Anchor, float AcceptanceRadius);
};
