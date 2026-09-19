#pragma once
#include "CoreMinimal.h"
/** Runtime travel resolver for canonical story missions. It contains no story state. */
struct DARKARISEN_API FMainStoryMapCatalog
{
    static FName ResolveMap(FName MissionId);
};
