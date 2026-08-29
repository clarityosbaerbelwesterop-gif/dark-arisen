// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ObsidianVaultComponent.generated.h"

class UAuthoredDungeonSiteComponent;

/**
 * Unique authored mechanics for the Obsidian Vault.
 *
 * The Bible locks the heat-channel puzzle and the unforgettable image of one hundred reflected
 * Jakes with exactly one moving wrong. It does not author an additional explanation or HUD cue.
 */
UCLASS(ClassGroup=(DarkArisen), meta=(BlueprintSpawnableComponent))
class DARKARISEN_API UObsidianVaultComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UObsidianVaultComponent();
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category="Obsidian Vault|Progress")
    bool MarkEnteredThroughLavaFissure();

    UFUNCTION(BlueprintCallable, Category="Obsidian Vault|Puzzle")
    bool MarkHeatChannelSolved();

    /** Marks the authored image only when the level presents the exact 100 / 1 relationship. */
    UFUNCTION(BlueprintCallable, Category="Obsidian Vault|Image")
    bool RecordReflectionImageObservation(int32 VisibleJakeReflections, int32 WrongMovingReflections);

    UFUNCTION(BlueprintCallable, Category="Obsidian Vault|Progress")
    bool ResolveReflectionEncounter(FName OutcomeId);

    UFUNCTION(BlueprintCallable, Category="Obsidian Vault|Progress")
    bool OpenMandatoryReturnShortcut();

    UFUNCTION(BlueprintCallable, Category="Obsidian Vault|Reward")
    bool RecoverLegendaryObsidian();

    UFUNCTION(BlueprintPure, Category="Obsidian Vault|Progress")
    bool IsDungeonStructurallyComplete() const;

    UFUNCTION(BlueprintPure, Category="Obsidian Vault|Puzzle")
    bool IsHeatChannelSolved() const { return bHeatChannelSolved; }

    UFUNCTION(BlueprintPure, Category="Obsidian Vault|Reward")
    bool HasLegendaryObsidian() const { return bLegendaryObsidianRecovered; }

    static bool IsCanonicalReflectionImage(int32 VisibleJakeReflections, int32 WrongMovingReflections);

    static constexpr int32 CanonicalVisibleJakeReflections = 100;
    static constexpr int32 CanonicalWrongMovingReflections = 1;

private:
    bool BindSiteAuthority();

    UPROPERTY(Transient)
    TObjectPtr<UAuthoredDungeonSiteComponent> SiteAuthority;

    UPROPERTY(SaveGame)
    bool bHeatChannelSolved = false;

    UPROPERTY(SaveGame)
    bool bLegendaryObsidianRecovered = false;
};
