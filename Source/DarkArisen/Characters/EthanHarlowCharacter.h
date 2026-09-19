#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EthanHarlowCharacter.generated.h"
class UHealthComponent;
UENUM(BlueprintType) enum class EEthanRuntimeState:uint8{Captive,Rescued,Aboard};
/** Canonical real Ethan Harlow. Friendly story character; deliberately has no boss/combat authority. */
UCLASS()
class DARKARISEN_API AEthanHarlowCharacter : public ACharacter
{
    GENERATED_BODY()
public:
    AEthanHarlowCharacter();
    virtual void BeginPlay() override;
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Ethan") TObjectPtr<UHealthComponent> HealthComponent;
    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Ethan") FName CharacterId=TEXT("ethan.harlow.real");
    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Ethan") EEthanRuntimeState RuntimeState=EEthanRuntimeState::Captive;
    UFUNCTION(BlueprintCallable,Category="Ethan") bool MarkRescued();
    UFUNCTION(BlueprintCallable,Category="Ethan") bool MarkAboard();
    UFUNCTION(BlueprintPure,Category="Ethan") bool IsFriendly() const{return true;}
    UFUNCTION(BlueprintPure,Category="Ethan") bool IsCaptive() const{return RuntimeState==EEthanRuntimeState::Captive;}
};
