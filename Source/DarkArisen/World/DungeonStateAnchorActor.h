#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DungeonStateAnchorActor.generated.h"

class UBoxComponent;
UENUM()
enum class EDungeonStateAnchorAction : uint8 { Discover, Complete, OpenReturnShortcut };

/** Invisible physical threshold used by authored dungeon layouts; never a map marker. */
UCLASS()
class DARKARISEN_API ADungeonStateAnchorActor : public AActor
{
    GENERATED_BODY()
public:
    ADungeonStateAnchorActor();
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dungeon") FName DungeonId;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dungeon") EDungeonStateAnchorAction Action=EDungeonStateAnchorAction::Discover;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dungeon") FName ShortcutId;
private:
    UPROPERTY(VisibleAnywhere) TObjectPtr<UBoxComponent> Trigger;
    UFUNCTION() void HandleBeginOverlap(UPrimitiveComponent* Overlapped,AActor* Other,UPrimitiveComponent* OtherComp,int32 BodyIndex,bool bFromSweep,const FHitResult& Sweep);
};
