#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "DarkArisenMaterializeStoryCommandlet.generated.h"

/** Materialises authored physical main-story contracts into real Alpha maps. */
UCLASS()
class DARKARISENEDITOR_API UDarkArisenMaterializeStoryCommandlet : public UCommandlet
{
    GENERATED_BODY()

public:
    UDarkArisenMaterializeStoryCommandlet();
    virtual int32 Main(const FString& Params) override;
};
