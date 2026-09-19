#include "DarkArisenOps.h"

#include "RequiredProgramMainCPPInclude.h"

IMPLEMENT_APPLICATION(DarkArisenOps, "DarkArisenOps");

namespace
{
void PrintUsage()
{
    UE_LOG(LogTemp, Display, TEXT("DarkArisenOps — native C++ project operations"));
    UE_LOG(LogTemp, Display, TEXT("Commands:"));
    UE_LOG(LogTemp, Display, TEXT("  validate --root=<repo>"));
    UE_LOG(LogTemp, Display, TEXT("  runner-check --root=<repo> --engine=<UE5.8> [--min-disk-gb=120]"));
    UE_LOG(LogTemp, Display, TEXT("  build --root=<repo> --engine=<UE5.8> [--automation]  (Editor -> materialise -> validate -> game builds)"));
    UE_LOG(LogTemp, Display, TEXT("  package-alpha --revision=<40sha> [--output=<dir>]  (fails closed without materialised Alpha content)"));
    UE_LOG(LogTemp, Display, TEXT("  promote-alpha --evidence=<json> --windows=<zip> --linux=<zip> --output=<dir>"));
    UE_LOG(LogTemp, Display, TEXT("  bootstrap-streaming --revision=<reviewed-40sha> [--infra=<dir>] [--force-rebuild]"));
    UE_LOG(LogTemp, Display, TEXT("  stream-signalling | stream-turn | stream-launch | stream-deploy"));
    UE_LOG(LogTemp, Display, TEXT("  stream-install-services | stream-idle-shutdown | stream-provider-shutdown"));
    UE_LOG(LogTemp, Display, TEXT("  tailnet-join | install-grid-driver | collect-host-evidence"));
}
}

INT32_MAIN_INT32_ARGC_TCHAR_ARGV()
{
    GEngineLoop.PreInit(ArgC, ArgV);

    int32 Result = 2;
    if (ArgC < 2)
    {
        PrintUsage();
    }
    else
    {
        const FString Command(ArgV[1]);
        const DarkArisenOps::FParsedArgs Args = DarkArisenOps::ParseArgs(ArgC, ArgV);
        if (Command == TEXT("validate"))
        {
            Result = DarkArisenOps::ValidateCommand(Args);
        }
        else if (Command == TEXT("runner-check"))
        {
            Result = DarkArisenOps::RunnerCheckCommand(Args);
        }
        else if (Command == TEXT("build"))
        {
            Result = DarkArisenOps::AlphaBuildCommand(Args);
        }
        else if (Command == TEXT("package-alpha"))
        {
            Result = DarkArisenOps::AlphaPackageCommand(Args);
        }
        else if (Command == TEXT("promote-alpha"))
        {
            Result = DarkArisenOps::PromoteAlphaCommand(Args);
        }
        else if (Command == TEXT("help") || Command == TEXT("--help") || Command == TEXT("-h"))
        {
            PrintUsage();
            Result = 0;
        }
        else
        {
            Result = DarkArisenOps::StreamingCommand(Command, Args);
        }
    }

    FEngineLoop::AppPreExit();
    FEngineLoop::AppExit();
    return Result;
}
