#pragma once
#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "DarkArisenMaterializeNavalCommandlet.generated.h"
UCLASS()
class DARKARISENEDITOR_API UDarkArisenMaterializeNavalCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDarkArisenMaterializeNavalCommandlet();
    virtual int32 Main(const FString& Params) override;
};
