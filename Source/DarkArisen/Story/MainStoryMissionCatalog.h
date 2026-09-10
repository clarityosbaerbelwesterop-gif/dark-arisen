#pragma once
#include "CoreMinimal.h"
#include "Story/MainStoryTypes.h"

/** September 2026 deterministic story authority. Never generates or shuffles missions. */
class DARKARISEN_API FMainStoryMissionCatalog
{
public:
    static const TArray<FMainStoryMissionDefinition>& Get();
    static int32 FindIndex(FName MissionId);
    static bool Validate(TArray<FString>& OutErrors);
    static constexpr int32 MissionCount = 34;
};
