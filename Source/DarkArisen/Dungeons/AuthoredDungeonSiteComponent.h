// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ContentScale/AuthoredDungeonCatalog.h"
#include "AuthoredDungeonSiteComponent.generated.h"

UENUM(BlueprintType)
enum class EDungeonArtifactDisposition : uint8
{
    None,
    Kept,
    Sold,
    Returned
};

/**
 * Common saveable runtime state for an individually authored dungeon site.
 *
 * This component owns no geometry, HUD, marker, music or procedural content. A level places it on
 * the authored site actor, supplies one stable ID from FAuthoredDungeonCatalog, and drives its state
 * from physical triggers/encounters. Unique mechanics stay in specialized C++ components such as
 * UCenoteFirstMotherComponent.
 */
UCLASS(ClassGroup=(DarkArisen), meta=(BlueprintSpawnableComponent))
class DARKARISEN_API UAuthoredDungeonSiteComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAuthoredDungeonSiteComponent();
    virtual void BeginPlay() override;

    /** Assign only one of the grounded stable IDs in FAuthoredDungeonCatalog. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Dungeon|Definition")
    FName DungeonStableId;

    /** Deterministic C++ initialization used by BeginPlay and native automation tests. */
    bool InitializeDefinition();

    UFUNCTION(BlueprintPure, Category="Dungeon|Definition")
    bool IsDefinitionValid() const { return bDefinitionValid; }

    UFUNCTION(BlueprintPure, Category="Dungeon|Definition")
    FString GetAuthoredDisplayName() const;

    UFUNCTION(BlueprintPure, Category="Dungeon|Definition")
    bool IsSecretSite() const;

    UFUNCTION(BlueprintPure, Category="Dungeon|Definition")
    bool IsNoBossSite() const;

    /** Cartographic discovery is the only mode that may be physically recorded before entry. */
    UFUNCTION(BlueprintPure, Category="Dungeon|Discovery")
    bool CanBeRecordedOnPhysicalMapBeforeEntry() const;

    /** All named sites may be physically annotated after Jake has actually entered them. */
    UFUNCTION(BlueprintPure, Category="Dungeon|Discovery")
    bool CanBeRecordedOnPhysicalMapNow() const;

    UFUNCTION(BlueprintCallable, Category="Dungeon|Progress")
    bool MarkEntered();

    UFUNCTION(BlueprintCallable, Category="Dungeon|Progress")
    bool MarkUnforgettableImageWitnessed();

    /**
     * Floor/bottom resolution. No-boss sites use this for the authored non-combat bottom state;
     * boss sites call it after their encounter owner has resolved. ResolutionId must be authored by
     * that site-specific caller; this common component never invents outcomes.
     */
    UFUNCTION(BlueprintCallable, Category="Dungeon|Progress")
    bool MarkBottomResolved(FName ResolutionId);

    /** The Return can be opened as soon as the player reaches its physical inside control. */
    UFUNCTION(BlueprintCallable, Category="Dungeon|Progress")
    bool OpenReturnShortcutFromInside();

    /** Optional artifact-return question. No-reward sites simply never call this. */
    UFUNCTION(BlueprintCallable, Category="Dungeon|Reward")
    bool RecordArtifactDisposition(EDungeonArtifactDisposition Disposition);

    UFUNCTION(BlueprintPure, Category="Dungeon|Progress")
    bool IsStructurallyComplete() const;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, SaveGame, Category="Dungeon|Progress")
    bool bEntered = false;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, SaveGame, Category="Dungeon|Progress")
    bool bUnforgettableImageWitnessed = false;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, SaveGame, Category="Dungeon|Progress")
    bool bBottomResolved = false;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, SaveGame, Category="Dungeon|Progress")
    FName BottomResolutionId;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, SaveGame, Category="Dungeon|Progress")
    bool bReturnShortcutOpened = false;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, SaveGame, Category="Dungeon|Reward")
    EDungeonArtifactDisposition ArtifactDisposition = EDungeonArtifactDisposition::None;

private:
    bool bDefinitionValid = false;
    FAuthoredDungeonCatalogEntry CachedDefinition;
};
