#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "DarkArisenMaterializeAlphaCommandlet.generated.h"

UCLASS()
class DARKARISENEDITOR_API UDarkArisenMaterializeAlphaCommandlet : public UCommandlet
{
    GENERATED_BODY()

public:
    UDarkArisenMaterializeAlphaCommandlet();
    virtual int32 Main(const FString& Params) override;
};
