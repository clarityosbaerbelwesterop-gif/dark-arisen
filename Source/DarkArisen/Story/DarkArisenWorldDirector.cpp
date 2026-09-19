#include "Story/DarkArisenWorldDirector.h"
#include "EngineUtils.h"
#include "Opening/OpeningRuntimeComponent.h"
#include "Story/MainStoryDirectorComponent.h"
ADarkArisenWorldDirector::ADarkArisenWorldDirector()
{
    PrimaryActorTick.bCanEverTick=false;
    MainStoryDirector=CreateDefaultSubobject<UMainStoryDirectorComponent>(TEXT("MainStoryDirector"));
    OpeningRuntime=CreateDefaultSubobject<UOpeningRuntimeComponent>(TEXT("OpeningRuntime"));
}
ADarkArisenWorldDirector* ADarkArisenWorldDirector::Resolve(const UObject* WorldContextObject)
{
    const UWorld* World=WorldContextObject?WorldContextObject->GetWorld():nullptr;
    if(!World)return nullptr;
    ADarkArisenWorldDirector* Found=nullptr;
    for(TActorIterator<ADarkArisenWorldDirector> It(World);It;++It)
    {
        if(Found)return nullptr;
        Found=*It;
    }
    return Found;
}
