// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "AuthoredWorldContentSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FLoadedRegionBinding
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FName RegionStableId;

    /** Actual loaded map package reported by the runtime. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FString RuntimeMapPackage;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    bool bSourceRegistryKnown = false;
};

/**
 * Runtime binding layer between a real UE level and the authored C++ region registry. It creates no
 * world geometry and cannot make an absent .umap exist. A production map must explicitly bind its
 * region stable ID; unknown IDs fail closed.
 */
UCLASS()
class DARKARISEN_API UAuthoredWorldContentSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="World|AuthoredContent")
    bool BindLoadedRegion(FName RegionStableId, const FString& RuntimeMapPackage);

    UFUNCTION(BlueprintCallable, Category="World|AuthoredContent")
    void ClearLoadedRegion();

    UFUNCTION(BlueprintPure, Category="World|AuthoredContent")
    bool HasValidLoadedRegion() const { return LoadedRegion.bSourceRegistryKnown; }

    UFUNCTION(BlueprintPure, Category="World|AuthoredContent")
    FLoadedRegionBinding GetLoadedRegion() const { return LoadedRegion; }

private:
    UPROPERTY(Transient)
    FLoadedRegionBinding LoadedRegion;
};
