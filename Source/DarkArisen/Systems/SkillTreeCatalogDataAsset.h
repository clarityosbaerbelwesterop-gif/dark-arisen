// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Systems/ProgressionEconomyComponent.h"
#include "SkillTreeCatalogDataAsset.generated.h"

/**
 * Authoring boundary for the complete 68-node Craft tree.
 *
 * Source code contains only capabilities the bible actually names. The remaining authored
 * definitions belong in data, not fabricated C++ names. This asset refuses registration unless
 * the complete set satisfies the locked 68 / 16-12-14-13-13 / 141 / 23 teacher / 11 Standing
 * contract and every prerequisite resolves inside the same catalog.
 */
UCLASS(BlueprintType)
class DARKARISEN_API USkillTreeCatalogDataAsset : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Skill Tree")
    TArray<FSkillNodeDefinition> Nodes;

    UFUNCTION(BlueprintPure, Category="Skill Tree")
    bool IsCatalogStructurallyValid() const;

    UFUNCTION(BlueprintCallable, Category="Skill Tree")
    bool RegisterInto(UProgressionEconomyComponent* Progression) const;
};
