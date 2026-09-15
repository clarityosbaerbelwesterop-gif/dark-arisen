#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HostileNavalShip.generated.h"
class USceneComponent; class UStaticMeshComponent; class UNavalCombatComponent;
/** Authored hostile vessel. Native hull/broadside authority; generated art may replace presentation only. */
UCLASS()
class DARKARISEN_API AHostileNavalShip : public AActor
{
    GENERATED_BODY()
public:
    AHostileNavalShip();
    virtual void Tick(float DeltaSeconds) override;
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly) TObjectPtr<USceneComponent> Root;
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly) TObjectPtr<UStaticMeshComponent> HullPresentation;
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly) TObjectPtr<UNavalCombatComponent> NavalCombat;
    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Naval|AI") float FireDecisionSeconds=2.0f;
private:
    float FireDecisionRemaining=0.f;
};
