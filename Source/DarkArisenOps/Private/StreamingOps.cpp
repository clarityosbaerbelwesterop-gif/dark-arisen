#include "DarkArisenOps.h"

#include "Dom/JsonObject.h"
#include "HAL/FileManager.h"
#include "HAL/PlatformProcess.h"
#include "Misc/DateTime.h"
#include "Misc/Guid.h"
#include "Misc/Paths.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

#if PLATFORM_WINDOWS
#include "Windows/AllowWindowsPlatformTypes.h"
#include <Windows.h>
#include <wintrust.h>
#include <softpub.h>
#include <wincrypt.h>
#include "Windows/HideWindowsPlatformTypes.h"
#endif

namespace DarkArisenOps
{
namespace
{
FString Env(const TCHAR* Name)
{
    return FPlatformMisc::GetEnvironmentVariable(Name);
}

bool RequireEnv(const TCHAR* Name, FString& Out)
{
    Out = Env(Name);
    if (Out.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("Required environment value is missing: %s"), Name);
        return false;
    }
    return true;
}

FString DefaultRuntimeRoot()
{
#if PLATFORM_WINDOWS
    FString ProgramData = Env(TEXT("ProgramData"));
    if (ProgramData.IsEmpty()) ProgramData = TEXT("C:/ProgramData");
    return FPaths::Combine(ProgramData, TEXT("DarkArisen"));
#else
    return TEXT("/var/lib/dark-arisen");
#endif
}

FString FindFirst(const FString& Root, const FString& Pattern)
{
    TArray<FString> Matches;
    IFileManager::Get().FindFilesRecursive(Matches, *Root, *Pattern, true, false, false);
    return Matches.IsEmpty() ? FString() : Matches[0];
}

bool SecureRuntimeFile(const FString& Path)
{
#if PLATFORM_WINDOWS
    FString Identity = Env(TEXT("USERNAME"));
    if (Identity.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("USERNAME is required to secure runtime file ACLs."));
        return false;
    }
    return RunProcess(TEXT("icacls.exe"), {
        Path,
        TEXT("/inheritance:r"),
        TEXT("/grant:r"),
        Identity + TEXT(":(F)"),
        TEXT("SYSTEM:(F)")});
#else
    return RunProcess(TEXT("chmod"), {TEXT("600"), Path});
#endif
}

bool SetMachineEnvironment(const FString& Name, const FString& Value)
{
#if PLATFORM_WINDOWS
    return RunProcess(TEXT("setx.exe"), {Name, Value, TEXT("/M")});
#else
    UE_LOG(LogTemp, Error, TEXT("Machine environment mutation is currently supported only on Windows streaming hosts."));
    return false;
#endif
}

int32 BootstrapStreaming(const FParsedArgs& Args)
{
#if !PLATFORM_WINDOWS
    UE_LOG(LogTemp, Error, TEXT("Pixel Streaming bootstrap is supported on the Windows streaming host."));
    return 1;
#else
    FString InfrastructureRoot = Args.Get(TEXT("infra"), Env(TEXT("PIXEL_STREAMING_INFRA_ROOT")));
    if (InfrastructureRoot.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("PIXEL_STREAMING_INFRA_ROOT or --infra is required."));
        return 2;
    }
    InfrastructureRoot = NormalizeFullPath(InfrastructureRoot);
    const FString Repository = TEXT("https://github.com/EpicGames/PixelStreamingInfrastructure.git");
    const FString PinnedCommit = Args.Get(TEXT("revision"), Env(TEXT("PIXEL_STREAMING_INFRA_REVISION"))).ToLower();
    if (!IsHex(PinnedCommit, 40))
    {
        UE_LOG(LogTemp, Error, TEXT("A reviewed 40-character UE5.8 Pixel Streaming Infrastructure commit is required via --revision or PIXEL_STREAMING_INFRA_REVISION."));
        return 2;
    }
    if (!IFileManager::Get().DirectoryExists(*InfrastructureRoot))
    {
        if (!RunProcess(TEXT("git"), {TEXT("clone"), TEXT("--branch"), TEXT("UE5.8"), TEXT("--single-branch"), Repository, InfrastructureRoot}))
        {
            return 1;
        }
    }
    FString Status;
    if (!RunProcess(TEXT("git"), {TEXT("-C"), InfrastructureRoot, TEXT("status"), TEXT("--porcelain")}, &Status))
    {
        return 1;
    }
    Status.TrimStartAndEndInline();
    if (!Status.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("Pixel Streaming Infrastructure contains unmanaged changes."));
        return 1;
    }
    if (!RunProcess(TEXT("git"), {TEXT("-C"), InfrastructureRoot, TEXT("fetch"), TEXT("origin"), PinnedCommit, TEXT("--depth"), TEXT("1")})
        || !RunProcess(TEXT("git"), {TEXT("-C"), InfrastructureRoot, TEXT("checkout"), TEXT("--detach"), PinnedCommit}))
    {
        return 1;
    }
    const FString Setup = FPaths::Combine(InfrastructureRoot, TEXT("SignallingWebServer/platform_scripts/cmd/setup.bat"));
    if (!FPaths::FileExists(Setup))
    {
        UE_LOG(LogTemp, Error, TEXT("Pixel Streaming setup.bat is missing: %s"), *Setup);
        return 1;
    }
    const TArray<FString> SetupArgs = Args.HasFlag(TEXT("force-rebuild"))
        ? TArray<FString>{TEXT("--deps"), TEXT("--rebuild")}
        : TArray<FString>{TEXT("--build"), TEXT("--build-wilbur")};
    if (!RunProcess(Setup, SetupArgs))
    {
        return 1;
    }
    for (const FString& Output : {
        FPaths::Combine(InfrastructureRoot, TEXT("SignallingWebServer/dist/index.js")),
        FPaths::Combine(InfrastructureRoot, TEXT("SignallingWebServer/www/player.html"))})
    {
        if (!FPaths::FileExists(Output))
        {
            UE_LOG(LogTemp, Error, TEXT("Expected Pixel Streaming output is missing: %s"), *Output);
            return 1;
        }
    }
    UE_LOG(LogTemp, Display, TEXT("Pixel Streaming Infrastructure UE5.8 pinned and built with Epic's stock frontend."));
    return 0;
#endif
}

int32 LaunchSignalling(const FParsedArgs& Args)
{
#if !PLATFORM_WINDOWS
    return 1;
#else
    FString InfrastructureRoot = Args.Get(TEXT("infra"), Env(TEXT("PIXEL_STREAMING_INFRA_ROOT")));
    FString RuntimeRoot = NormalizeFullPath(Args.Get(TEXT("runtime"), DefaultRuntimeRoot()));
    FString TurnHost, TurnUser, TurnCredential;
    if (InfrastructureRoot.IsEmpty()
        || !RequireEnv(TEXT("DARKARISEN_TURN_HOST"), TurnHost)
        || !RequireEnv(TEXT("DARKARISEN_TURN_USERNAME"), TurnUser)
        || !RequireEnv(TEXT("DARKARISEN_TURN_CREDENTIAL"), TurnCredential))
    {
        return 2;
    }
    InfrastructureRoot = NormalizeFullPath(InfrastructureRoot);
    const FString Node = FPaths::Combine(InfrastructureRoot, TEXT("SignallingWebServer/platform_scripts/cmd/node/node.exe"));
    const FString Server = FPaths::Combine(InfrastructureRoot, TEXT("SignallingWebServer/dist/index.js"));
    const FString WebRoot = FPaths::Combine(InfrastructureRoot, TEXT("SignallingWebServer/www"));
    if (!FPaths::FileExists(Node) || !FPaths::FileExists(Server) || !FPaths::FileExists(FPaths::Combine(WebRoot, TEXT("player.html"))))
    {
        UE_LOG(LogTemp, Error, TEXT("Pixel Streaming signalling runtime is incomplete."));
        return 1;
    }
    EnsureDirectory(RuntimeRoot);

    TSharedRef<FJsonObject> Ice = MakeShared<FJsonObject>();
    TArray<TSharedPtr<FJsonValue>> Urls;
    Urls.Add(MakeShared<FJsonValueString>(TEXT("stun:") + TurnHost));
    Urls.Add(MakeShared<FJsonValueString>(TEXT("turn:") + TurnHost + TEXT("?transport=udp")));
    Urls.Add(MakeShared<FJsonValueString>(TEXT("turn:") + TurnHost + TEXT("?transport=tcp")));
    Ice->SetArrayField(TEXT("urls"), Urls);
    Ice->SetStringField(TEXT("username"), TurnUser);
    Ice->SetStringField(TEXT("credential"), TurnCredential);

    TSharedRef<FJsonObject> Peer = MakeShared<FJsonObject>();
    Peer->SetArrayField(TEXT("iceServers"), {MakeShared<FJsonValueObject>(Ice)});

    TSharedRef<FJsonObject> Config = MakeShared<FJsonObject>();
    Config->SetStringField(TEXT("log_folder"), FPaths::Combine(RuntimeRoot, TEXT("signalling-logs")));
    Config->SetStringField(TEXT("log_level_console"), TEXT("info"));
    Config->SetStringField(TEXT("log_level_file"), TEXT("info"));
    Config->SetStringField(TEXT("streamer_port"), TEXT("8888"));
    Config->SetStringField(TEXT("player_port"), TEXT("8080"));
    Config->SetStringField(TEXT("sfu_port"), TEXT("8889"));
    Config->SetStringField(TEXT("max_players"), TEXT("1"));
    Config->SetBoolField(TEXT("serve"), true);
    Config->SetStringField(TEXT("http_root"), WebRoot);
    Config->SetStringField(TEXT("homepage"), TEXT("player.html"));
    Config->SetBoolField(TEXT("https"), false);
    Config->SetBoolField(TEXT("https_redirect"), false);
    Config->SetBoolField(TEXT("rest_api"), false);
    Config->SetObjectField(TEXT("peer_options"), Peer);
    Config->SetBoolField(TEXT("log_config"), false);
    Config->SetBoolField(TEXT("stdin"), false);

    FString Json;
    FJsonSerializer::Serialize(Config, TJsonWriterFactory<>::Create(&Json));
    const FString ConfigPath = FPaths::Combine(RuntimeRoot, TEXT("signalling.runtime.json"));
    if (!WriteText(ConfigPath, Json + TEXT("\n")) || !SecureRuntimeFile(ConfigPath))
    {
        return 1;
    }
    return RunProcess(Node, {Server, TEXT("--config_file"), ConfigPath}) ? 0 : 1;
#endif
}

int32 LaunchTurn(const FParsedArgs& Args)
{
#if !PLATFORM_WINDOWS
    return 1;
#else
    FString InfrastructureRoot = Args.Get(TEXT("infra"), Env(TEXT("PIXEL_STREAMING_INFRA_ROOT")));
    FString RuntimeRoot = NormalizeFullPath(Args.Get(TEXT("runtime"), DefaultRuntimeRoot()));
    FString User, Credential, Realm, LocalIp;
    if (InfrastructureRoot.IsEmpty()
        || !RequireEnv(TEXT("DARKARISEN_TURN_USERNAME"), User)
        || !RequireEnv(TEXT("DARKARISEN_TURN_CREDENTIAL"), Credential)
        || !RequireEnv(TEXT("DARKARISEN_TURN_REALM"), Realm)
        || !RequireEnv(TEXT("DARKARISEN_LOCAL_IP"), LocalIp))
    {
        return 2;
    }
    const bool bPrivate = Env(TEXT("DARKARISEN_PRIVATE_OVERLAY")) == TEXT("1");
    FString PublicIp;
    if (!bPrivate && !RequireEnv(TEXT("DARKARISEN_PUBLIC_IP"), PublicIp))
    {
        return 2;
    }
    const FString TurnServer = FPaths::Combine(NormalizeFullPath(InfrastructureRoot), TEXT("SignallingWebServer/platform_scripts/cmd/coturn/turnserver.exe"));
    if (!FPaths::FileExists(TurnServer))
    {
        UE_LOG(LogTemp, Error, TEXT("coturn is missing; run bootstrap-streaming first."));
        return 1;
    }
    EnsureDirectory(RuntimeRoot);
    const int32 Port = FCString::Atoi(*Args.Get(TEXT("port"), TEXT("3478")));
    const int32 RelayStart = FCString::Atoi(*Args.Get(TEXT("relay-start"), TEXT("49160")));
    const int32 RelayEnd = FCString::Atoi(*Args.Get(TEXT("relay-end"), TEXT("49200")));
    FString Config = FString::Printf(
        TEXT("listening-port=%d\nmin-port=%d\nmax-port=%d\nlistening-ip=%s\nrelay-ip=%s\n"),
        Port, RelayStart, RelayEnd, *LocalIp, *LocalIp);
    if (!bPrivate)
    {
        Config += FString::Printf(TEXT("external-ip=%s/%s\n"), *PublicIp, *LocalIp);
    }
    Config += FString::Printf(
        TEXT("realm=%s\nfingerprint\nlt-cred-mech\nuser=%s:%s\nno-cli\nno-tls\nno-dtls\nno-multicast-peers\nstale-nonce=600\n"),
        *Realm, *User, *Credential);
    const FString ConfigPath = FPaths::Combine(RuntimeRoot, TEXT("turnserver.runtime.conf"));
    if (!WriteText(ConfigPath, Config) || !SecureRuntimeFile(ConfigPath))
    {
        return 1;
    }
    return RunProcess(TurnServer, {TEXT("-c"), ConfigPath}) ? 0 : 1;
#endif
}

int32 ProviderShutdown(const FParsedArgs& Args)
{
    const FString Provider = Args.Get(TEXT("provider"), Env(TEXT("DARKARISEN_SHUTDOWN_PROVIDER"))).ToLower();
    const bool bValidate = Args.HasFlag(TEXT("validate-only"));
    if (Provider == TEXT("aws"))
    {
        FString InstanceId, Region;
        if (!RequireEnv(TEXT("DARKARISEN_AWS_INSTANCE_ID"), InstanceId) || !RequireEnv(TEXT("AWS_REGION"), Region))
        {
            return 2;
        }
        const TArray<FString> Command = bValidate
            ? TArray<FString>{TEXT("ec2"), TEXT("describe-instances"), TEXT("--instance-ids"), InstanceId, TEXT("--region"), Region, TEXT("--query"), TEXT("Reservations[0].Instances[0].InstanceId"), TEXT("--output"), TEXT("text")}
            : TArray<FString>{TEXT("ec2"), TEXT("stop-instances"), TEXT("--instance-ids"), InstanceId, TEXT("--region"), Region, TEXT("--output"), TEXT("json")};
        return RunProcess(TEXT("aws"), Command) ? 0 : 1;
    }
    if (Provider == TEXT("azure"))
    {
        FString ResourceId;
        if (!RequireEnv(TEXT("DARKARISEN_AZURE_VM_RESOURCE_ID"), ResourceId))
        {
            return 2;
        }
        if (!RunProcess(TEXT("az"), {TEXT("login"), TEXT("--identity"), TEXT("--allow-no-subscriptions"), TEXT("--output"), TEXT("none")}))
        {
            return 1;
        }
        const TArray<FString> Command = bValidate
            ? TArray<FString>{TEXT("vm"), TEXT("show"), TEXT("--ids"), ResourceId, TEXT("--query"), TEXT("id"), TEXT("--output"), TEXT("none"), TEXT("--only-show-errors")}
            : TArray<FString>{TEXT("vm"), TEXT("deallocate"), TEXT("--ids"), ResourceId, TEXT("--no-wait"), TEXT("--only-show-errors")};
        return RunProcess(TEXT("az"), Command) ? 0 : 1;
    }
    UE_LOG(LogTemp, Error, TEXT("--provider must be aws or azure."));
    return 2;
}

int32 CountLoopbackPlayerConnections(int32 Port)
{
#if !PLATFORM_WINDOWS
    return 0;
#else
    FString Out;
    int32 Exit = 0;
    if (!RunProcess(TEXT("netstat.exe"), {TEXT("-ano"), TEXT("-p"), TEXT("tcp")}, &Out, nullptr, FString(), &Exit))
    {
        return -1;
    }
    int32 Count = 0;
    TArray<FString> Lines;
    Out.ParseIntoArrayLines(Lines, true);
    const FString PortToken = FString::Printf(TEXT(":%d"), Port);
    for (const FString& Line : Lines)
    {
        if (Line.Contains(TEXT("ESTABLISHED"), ESearchCase::IgnoreCase)
            && Line.Contains(PortToken)
            && (Line.Contains(TEXT("127.0.0.1")) || Line.Contains(TEXT("[::1]"))))
        {
            ++Count;
        }
    }
    return Count;
#endif
}

int32 IdleShutdown(const FParsedArgs& Args)
{
#if !PLATFORM_WINDOWS
    return 1;
#else
    const int32 IdleMinutes = FCString::Atoi(*Args.Get(TEXT("idle-minutes"), TEXT("30")));
    const int32 PlayerPort = FCString::Atoi(*Args.Get(TEXT("player-port"), TEXT("8080")));
    if (IdleMinutes < 1)
    {
        return 2;
    }
    FDateTime IdleSince = FDateTime::UtcNow();
    while (true)
    {
        const int32 Count = CountLoopbackPlayerConnections(PlayerPort);
        if (Count > 0)
        {
            IdleSince = FDateTime::UtcNow();
        }
        else if ((FDateTime::UtcNow() - IdleSince).GetTotalMinutes() >= IdleMinutes)
        {
            return ProviderShutdown(Args);
        }
        FPlatformProcess::Sleep(30.0f);
    }
#endif
}

int32 LaunchStream(const FParsedArgs& Args)
{
#if !PLATFORM_WINDOWS
    UE_LOG(LogTemp, Error, TEXT("Interactive Pixel Streaming host launch is Windows-only for this Alpha."));
    return 1;
#else
    FString RuntimeRoot = NormalizeFullPath(Args.Get(TEXT("runtime"), DefaultRuntimeRoot()));
    EnsureDirectory(RuntimeRoot);
    FString Executable = Args.Get(TEXT("executable"));
    if (Executable.IsEmpty())
    {
        FString Pointer;
        if (!ReadText(FPaths::Combine(RuntimeRoot, TEXT("active-release.txt")), Pointer))
        {
            UE_LOG(LogTemp, Error, TEXT("No executable or active-release pointer supplied."));
            return 2;
        }
        Pointer.TrimStartAndEndInline();
        Executable = FindFirst(Pointer, TEXT("DarkArisen.exe"));
    }
    Executable = NormalizeFullPath(Executable);
    if (!FPaths::FileExists(Executable))
    {
        UE_LOG(LogTemp, Error, TEXT("Packaged executable does not exist: %s"), *Executable);
        return 1;
    }
    FString Gpu;
    if (!RunProcess(TEXT("nvidia-smi.exe"), {TEXT("--query-gpu=name"), TEXT("--format=csv,noheader")}, &Gpu) || Gpu.TrimStartAndEnd().IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("No usable NVIDIA GPU; NVENC cannot be verified."));
        return 1;
    }
    FString UserData = NormalizeFullPath(Args.Get(TEXT("user-data"), FPaths::Combine(RuntimeRoot, TEXT("UserData"))));
    EnsureDirectory(UserData);
    const FString LogPath = FPaths::Combine(RuntimeRoot, TEXT("DarkArisen-streaming.log"));
    const TArray<FString> Arguments = {
        TEXT("-PixelStreamingURL=ws://127.0.0.1:8888"),
        TEXT("-RenderOffscreen"), TEXT("-Unattended"), TEXT("-NoTextureStreaming"),
        TEXT("-ResX=1920"), TEXT("-ResY=1080"), TEXT("-ForceRes"),
        TEXT("-PixelStreamingWebRTCMaxFps=60"), TEXT("-AudioMixer"),
        FString::Printf(TEXT("-UserDir=%s"), *UserData),
        FString::Printf(TEXT("-AbsLog=%s"), *LogPath)};
    const FString Params = JoinParams(Arguments);
    FProcHandle Handle = FPlatformProcess::CreateProc(*Executable, *Params, true, false, false, nullptr, 0, nullptr, nullptr);
    if (!Handle.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("Unable to launch DarkArisen streaming process."));
        return 1;
    }
    const uint32 Pid = FPlatformProcess::GetProcId(Handle);
    WriteText(FPaths::Combine(RuntimeRoot, TEXT("game.pid")), FString::FromInt(static_cast<int32>(Pid)) + TEXT("\n"));
    const int32 TimeoutSeconds = FCString::Atoi(*Args.Get(TEXT("nvenc-timeout"), TEXT("90")));
    const FDateTime Deadline = FDateTime::UtcNow() + FTimespan::FromSeconds(TimeoutSeconds);
    bool bVerified = false;
    while (FDateTime::UtcNow() < Deadline && FPlatformProcess::IsProcRunning(Handle))
    {
        FString Log;
        if (ReadText(LogPath, Log))
        {
            const FString Lower = Log.ToLower();
            if ((Lower.Contains(TEXT("failed")) || Lower.Contains(TEXT("unable"))) && Lower.Contains(TEXT("nvenc"))
                || Lower.Contains(TEXT("software encoder")))
            {
                FPlatformProcess::TerminateProc(Handle, true);
                IFileManager::Get().Delete(*FPaths::Combine(RuntimeRoot, TEXT("game.pid")));
                UE_LOG(LogTemp, Error, TEXT("Pixel Streaming fell back from NVENC."));
                return 1;
            }
            if ((Lower.Contains(TEXT("nvenc")) || Lower.Contains(TEXT("nvidia video codec")))
                && (Lower.Contains(TEXT("initializ")) || Lower.Contains(TEXT("created")) || Lower.Contains(TEXT("active")) || Lower.Contains(TEXT("selected"))))
            {
                bVerified = true;
                break;
            }
        }
        FPlatformProcess::Sleep(2.0f);
    }
    if (!bVerified)
    {
        if (FPlatformProcess::IsProcRunning(Handle)) FPlatformProcess::TerminateProc(Handle, true);
        IFileManager::Get().Delete(*FPaths::Combine(RuntimeRoot, TEXT("game.pid")));
        UE_LOG(LogTemp, Error, TEXT("NVENC activation was not proven within %d seconds."), TimeoutSeconds);
        return 1;
    }
    UE_LOG(LogTemp, Display, TEXT("Dark Arisen connected to local signalling with NVENC verified."));
    if (Args.HasFlag(TEXT("wait")))
    {
        FPlatformProcess::WaitForProc(Handle);
        int32 ReturnCode = 0;
        FPlatformProcess::GetProcReturnCode(Handle, &ReturnCode);
        IFileManager::Get().Delete(*FPaths::Combine(RuntimeRoot, TEXT("game.pid")));
        return ReturnCode;
    }
    return 0;
#endif
}

#if PLATFORM_WINDOWS
FString ProcessImagePath(uint32 Pid)
{
    FString Result;
    HANDLE Process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, Pid);
    if (!Process) return Result;
    TCHAR Buffer[32768];
    DWORD Size = UE_ARRAY_COUNT(Buffer);
    if (QueryFullProcessImageNameW(Process, 0, Buffer, &Size))
    {
        Result = FString(static_cast<int32>(Size), Buffer);
        Result = NormalizeFullPath(Result);
    }
    CloseHandle(Process);
    return Result;
}
#endif

int32 DeployStream(const FParsedArgs& Args)
{
#if !PLATFORM_WINDOWS
    return 1;
#else
    const FString Root = RepoRoot(Args);
    const FString Engine = EngineRoot(Args);
    const FString Revision = Args.Get(TEXT("revision"), TEXT("manual"));
    if (Engine.IsEmpty() || !IsSafeRevision(Revision))
    {
        UE_LOG(LogTemp, Error, TEXT("UE_ROOT/--engine and a safe --revision are required."));
        return 2;
    }
    const FString DeploymentRoot = NormalizeFullPath(Args.Get(TEXT("deployment-root"), TEXT("C:/DarkArisen/Deployments")));
    const FString RuntimeRoot = NormalizeFullPath(Args.Get(TEXT("runtime"), DefaultRuntimeRoot()));
    if (IsFilesystemRoot(DeploymentRoot))
    {
        return 1;
    }
    const FString ReleaseRoot = FPaths::Combine(DeploymentRoot, Revision);
    if (IFileManager::Get().DirectoryExists(*ReleaseRoot) || !EnsureDirectory(ReleaseRoot) || !EnsureDirectory(RuntimeRoot))
    {
        return 1;
    }
    const FString Uat = FPaths::Combine(Engine, TEXT("Engine/Build/BatchFiles/RunUAT.bat"));
    if (!RunProcess(Uat, {
        TEXT("BuildCookRun"), FString::Printf(TEXT("-project=%s"), *FPaths::Combine(Root, TEXT("DarkArisen.uproject"))),
        TEXT("-noP4"), TEXT("-platform=Win64"), TEXT("-clientconfig=Development"), TEXT("-build"), TEXT("-cook"),
        TEXT("-map=/Engine/Maps/Entry"), TEXT("-stage"), TEXT("-pak"), TEXT("-archive"),
        FString::Printf(TEXT("-archivedirectory=%s"), *ReleaseRoot), TEXT("-utf8output")}))
    {
        return 1;
    }
    const FString Executable = FindFirst(ReleaseRoot, TEXT("DarkArisen.exe"));
    if (Executable.IsEmpty())
    {
        return 1;
    }
    const FString PointerPath = FPaths::Combine(RuntimeRoot, TEXT("active-release.txt"));
    FString Previous;
    if (ReadText(PointerPath, Previous))
    {
        Previous.TrimStartAndEndInline();
        if (!Previous.IsEmpty())
        {
            Previous = NormalizeFullPath(Previous);
            const FString Prefix = DeploymentRoot.EndsWith(TEXT("/")) ? DeploymentRoot : DeploymentRoot + TEXT("/");
            if (!Previous.StartsWith(Prefix, ESearchCase::IgnoreCase))
            {
                UE_LOG(LogTemp, Error, TEXT("Active release pointer escapes DeploymentRoot."));
                return 1;
            }
        }
    }
    const FString PidPath = FPaths::Combine(RuntimeRoot, TEXT("game.pid"));
    FString PidText;
    if (ReadText(PidPath, PidText))
    {
        const uint32 Pid = static_cast<uint32>(FCString::Atoi(*PidText));
        if (Pid > 0)
        {
            FProcHandle Existing = FPlatformProcess::OpenProcess(Pid);
            if (Existing.IsValid() && FPlatformProcess::IsProcRunning(Existing))
            {
                const FString Image = ProcessImagePath(Pid);
                const FString ExpectedPrefix = Previous.IsEmpty() ? FString() : Previous + TEXT("/");
                if (Image.IsEmpty() || ExpectedPrefix.IsEmpty() || !Image.StartsWith(ExpectedPrefix, ESearchCase::IgnoreCase)
                    || !FPaths::GetCleanFilename(Image).Equals(TEXT("DarkArisen.exe"), ESearchCase::IgnoreCase))
                {
                    UE_LOG(LogTemp, Error, TEXT("Recorded PID does not identify the active DarkArisen release."));
                    return 1;
                }
                FPlatformProcess::TerminateProc(Existing, true);
            }
        }
        IFileManager::Get().Delete(*PidPath);
    }
    const FString TempPointer = PointerPath + TEXT(".new");
    if (!WriteText(TempPointer, ReleaseRoot + TEXT("\n"))
        || !IFileManager::Get().Move(*PointerPath, *TempPointer, true, true, false, true))
    {
        return 1;
    }
    FParsedArgs LaunchArgs = Args;
    LaunchArgs.Values.Add(TEXT("executable"), Executable);
    LaunchArgs.Values.Add(TEXT("runtime"), RuntimeRoot);
    if (LaunchStream(LaunchArgs) != 0)
    {
        if (!Previous.IsEmpty() && IFileManager::Get().DirectoryExists(*Previous))
        {
            WriteText(PointerPath, Previous + TEXT("\n"));
            FParsedArgs Rollback = Args;
            Rollback.Values.Remove(TEXT("executable"));
            Rollback.Values.Add(TEXT("runtime"), RuntimeRoot);
            LaunchStream(Rollback);
        }
        return 1;
    }
    UE_LOG(LogTemp, Display, TEXT("Deployment %s is active. Previous release retained."), *Revision);
    return 0;
#endif
}

bool InstallNssmService(const FString& Nssm, const FString& Name, const FString& Application, const FString& Arguments, const FString& LogRoot)
{
#if !PLATFORM_WINDOWS
    return false;
#else
    RunProcess(Nssm, {TEXT("stop"), Name, TEXT("confirm")});
    RunProcess(Nssm, {TEXT("remove"), Name, TEXT("confirm")});
    return RunProcess(Nssm, {TEXT("install"), Name, Application, Arguments})
        && RunProcess(Nssm, {TEXT("set"), Name, TEXT("AppExit"), TEXT("Default"), TEXT("Restart")})
        && RunProcess(Nssm, {TEXT("set"), Name, TEXT("AppRestartDelay"), TEXT("5000")})
        && RunProcess(Nssm, {TEXT("set"), Name, TEXT("AppStdout"), FPaths::Combine(LogRoot, Name + TEXT(".out.log"))})
        && RunProcess(Nssm, {TEXT("set"), Name, TEXT("AppStderr"), FPaths::Combine(LogRoot, Name + TEXT(".err.log"))})
        && RunProcess(Nssm, {TEXT("set"), Name, TEXT("AppRotateFiles"), TEXT("1")})
        && RunProcess(Nssm, {TEXT("set"), Name, TEXT("AppRotateBytes"), TEXT("10485760")})
        && RunProcess(Nssm, {TEXT("set"), Name, TEXT("Start"), TEXT("SERVICE_AUTO_START")});
#endif
}

bool DeleteFirewallRule(const FString& Name)
{
#if PLATFORM_WINDOWS
    RunProcess(TEXT("netsh.exe"), {TEXT("advfirewall"), TEXT("firewall"), TEXT("delete"), TEXT("rule"), TEXT("name=") + Name});
    return true;
#else
    return false;
#endif
}

bool AddFirewallRule(const FString& Name, const FString& Action, const FString& Protocol, const FString& Ports, const FString& RemoteIp = FString())
{
#if PLATFORM_WINDOWS
    TArray<FString> Args = {
        TEXT("advfirewall"), TEXT("firewall"), TEXT("add"), TEXT("rule"),
        TEXT("name=") + Name, TEXT("dir=in"), TEXT("action=") + Action,
        TEXT("protocol=") + Protocol, TEXT("localport=") + Ports};
    if (!RemoteIp.IsEmpty()) Args.Add(TEXT("remoteip=") + RemoteIp);
    return RunProcess(TEXT("netsh.exe"), Args);
#else
    return false;
#endif
}

int32 InstallServices(const FParsedArgs& Args)
{
#if !PLATFORM_WINDOWS
    return 1;
#else
    const FString Root = RepoRoot(Args);
    const FString Ops = OpsBinaryPath(Root);
    const FString Nssm = NormalizeFullPath(Args.Get(TEXT("nssm")));
    const FString Caddy = NormalizeFullPath(Args.Get(TEXT("caddy")));
    const FString Infrastructure = NormalizeFullPath(Args.Get(TEXT("infra"), Env(TEXT("PIXEL_STREAMING_INFRA_ROOT"))));
    const FString Runtime = NormalizeFullPath(Args.Get(TEXT("runtime"), DefaultRuntimeRoot()));
    const FString Provider = Args.Get(TEXT("provider"), Env(TEXT("DARKARISEN_SHUTDOWN_PROVIDER"))).ToLower();
    const bool bPrivate = Args.HasFlag(TEXT("private-tailnet"));
    const FString Tailscale = NormalizeFullPath(Args.Get(TEXT("tailscale"), TEXT("C:/Program Files/Tailscale/tailscale.exe")));
    if (!FPaths::FileExists(Ops) || !FPaths::FileExists(Nssm) || Infrastructure.IsEmpty()
        || (!bPrivate && !FPaths::FileExists(Caddy)) || (bPrivate && !FPaths::FileExists(Tailscale))
        || (Provider != TEXT("aws") && Provider != TEXT("azure")))
    {
        UE_LOG(LogTemp, Error, TEXT("install-services prerequisites are incomplete."));
        return 2;
    }
    for (const TCHAR* Name : {TEXT("DARKARISEN_STREAM_HOST"), TEXT("DARKARISEN_TURN_HOST"), TEXT("DARKARISEN_TURN_USERNAME"), TEXT("DARKARISEN_TURN_CREDENTIAL"), TEXT("DARKARISEN_TURN_REALM"), TEXT("DARKARISEN_LOCAL_IP")})
    {
        if (Env(Name).IsEmpty()) return 2;
    }
    if (bPrivate && Env(TEXT("DARKARISEN_PRIVATE_OVERLAY")) != TEXT("1"))
    {
        UE_LOG(LogTemp, Error, TEXT("DARKARISEN_PRIVATE_OVERLAY must be 1 in private mode."));
        return 1;
    }
    EnsureDirectory(Runtime);
    const FString Logs = FPaths::Combine(Runtime, TEXT("service-logs"));
    EnsureDirectory(Logs);
    FParsedArgs ProviderCheck = Args;
    ProviderCheck.Values.Add(TEXT("provider"), Provider);
    ProviderCheck.Flags.Add(TEXT("validate-only"));
    if (ProviderShutdown(ProviderCheck) != 0)
    {
        return 1;
    }
    const FString Common = FString::Printf(TEXT("--root=%s --infra=%s --runtime=%s"), *Quote(Root), *Quote(Infrastructure), *Quote(Runtime));
    if (!InstallNssmService(Nssm, TEXT("DarkArisenTurn"), Ops, TEXT("stream-turn ") + Common, Logs)
        || !InstallNssmService(Nssm, TEXT("DarkArisenSignalling"), Ops, TEXT("stream-signalling ") + Common, Logs)
        || !InstallNssmService(Nssm, TEXT("DarkArisenIdleShutdown"), Ops, FString::Printf(TEXT("stream-idle-shutdown --provider=%s"), *Provider), Logs))
    {
        return 1;
    }
    if (bPrivate)
    {
        DeleteFirewallRule(TEXT("Dark Arisen TURN UDP"));
        DeleteFirewallRule(TEXT("Dark Arisen TURN TCP"));
        DeleteFirewallRule(TEXT("Dark Arisen HTTPS"));
        RunProcess(Nssm, {TEXT("stop"), TEXT("DarkArisenGateway"), TEXT("confirm")});
        RunProcess(Nssm, {TEXT("remove"), TEXT("DarkArisenGateway"), TEXT("confirm")});
        const FString Ranges = TEXT("100.64.0.0/10,fd7a:115c:a1e0::/48");
        for (const FString& Rule : {TEXT("Dark Arisen Tailnet HTTPS"), TEXT("Dark Arisen Tailnet TURN TCP"), TEXT("Dark Arisen Tailnet TURN UDP")}) DeleteFirewallRule(Rule);
        if (!AddFirewallRule(TEXT("Dark Arisen Tailnet HTTPS"), TEXT("allow"), TEXT("TCP"), TEXT("443"), Ranges)
            || !AddFirewallRule(TEXT("Dark Arisen Tailnet TURN TCP"), TEXT("allow"), TEXT("TCP"), TEXT("3478"), Ranges)
            || !AddFirewallRule(TEXT("Dark Arisen Tailnet TURN UDP"), TEXT("allow"), TEXT("UDP"), TEXT("3478,49160-49200"), Ranges)
            || !RunProcess(Tailscale, {TEXT("funnel"), TEXT("reset")})
            || !RunProcess(Tailscale, {TEXT("serve"), TEXT("--bg"), TEXT("--https=443"), TEXT("http://127.0.0.1:8080")}))
        {
            return 1;
        }
    }
    else
    {
        const FString Caddyfile = FPaths::Combine(Runtime, TEXT("Caddyfile"));
        if (IFileManager::Get().Copy(*Caddyfile, *FPaths::Combine(Root, TEXT("Tools/streaming/Caddyfile.example")), true, true) != COPY_OK
            || !RunProcess(Caddy, {TEXT("validate"), TEXT("--config"), Caddyfile, TEXT("--adapter"), TEXT("caddyfile")})
            || !InstallNssmService(Nssm, TEXT("DarkArisenGateway"), Caddy, FString::Printf(TEXT("run --config %s --adapter caddyfile"), *Quote(Caddyfile)), Logs))
        {
            return 1;
        }
        DeleteFirewallRule(TEXT("Dark Arisen TURN UDP"));
        DeleteFirewallRule(TEXT("Dark Arisen TURN TCP"));
        DeleteFirewallRule(TEXT("Dark Arisen HTTPS"));
        AddFirewallRule(TEXT("Dark Arisen TURN UDP"), TEXT("allow"), TEXT("UDP"), TEXT("3478,49160-49200"));
        AddFirewallRule(TEXT("Dark Arisen TURN TCP"), TEXT("allow"), TEXT("TCP"), TEXT("3478"));
        AddFirewallRule(TEXT("Dark Arisen HTTPS"), TEXT("allow"), TEXT("TCP"), TEXT("80,443"));
    }
    DeleteFirewallRule(TEXT("Dark Arisen Private TCP"));
    if (!AddFirewallRule(TEXT("Dark Arisen Private TCP"), TEXT("block"), TEXT("TCP"), TEXT("8080,8888,8889")))
    {
        return 1;
    }
    for (const FString& Service : {TEXT("DarkArisenTurn"), TEXT("DarkArisenSignalling"), TEXT("DarkArisenIdleShutdown")})
    {
        if (!RunProcess(TEXT("sc.exe"), {TEXT("start"), Service})) return 1;
    }
    if (!bPrivate && !RunProcess(TEXT("sc.exe"), {TEXT("start"), TEXT("DarkArisenGateway")})) return 1;
    return 0;
#endif
}

int32 TailnetJoin(const FParsedArgs& Args)
{
#if !PLATFORM_WINDOWS
    return 1;
#else
    const FString Arn = Args.Get(TEXT("auth-key-parameter-arn"));
    const FString Region = Args.Get(TEXT("region"), Env(TEXT("AWS_REGION")));
    const FString Hostname = Args.Get(TEXT("hostname"), TEXT("dark-arisen-alpha"));
    const FString Tag = Args.Get(TEXT("tag"), TEXT("tag:dark-arisen"));
    const FString Tailscale = NormalizeFullPath(Args.Get(TEXT("tailscale"), TEXT("C:/Program Files/Tailscale/tailscale.exe")));
    if (Arn.IsEmpty() || Region.IsEmpty() || Tag != TEXT("tag:dark-arisen") || !FPaths::FileExists(Tailscale)
        || !Arn.StartsWith(TEXT("arn:aws")) || !Arn.Contains(TEXT(":ssm:")) || !Arn.Contains(TEXT(":parameter/")))
    {
        UE_LOG(LogTemp, Error, TEXT("tailnet-join arguments are invalid."));
        return 2;
    }
    if (Hostname.IsEmpty() || Hostname.Len() > 63)
    {
        return 2;
    }
    if (Args.HasFlag(TEXT("validate-only")))
    {
        FString Aws;
        return RunProcess(TEXT("aws"), {TEXT("--version")}, &Aws) ? 0 : 1;
    }
    FString Key;
    if (!RunProcess(TEXT("aws"), {TEXT("ssm"), TEXT("get-parameter"), TEXT("--name"), Arn, TEXT("--with-decryption"), TEXT("--region"), Region, TEXT("--query"), TEXT("Parameter.Value"), TEXT("--output"), TEXT("text")}, &Key))
    {
        return 1;
    }
    Key.TrimStartAndEndInline();
    if (!Key.StartsWith(TEXT("tskey-auth-")))
    {
        UE_LOG(LogTemp, Error, TEXT("SSM SecureString does not contain a Tailscale auth key."));
        return 1;
    }
    const FString Runtime = DefaultRuntimeRoot();
    EnsureDirectory(Runtime);
    const FString KeyPath = FPaths::Combine(Runtime, TEXT("tailscale-auth-") + FGuid::NewGuid().ToString(EGuidFormats::Digits) + TEXT(".tmp"));
    if (!WriteText(KeyPath, Key) || !SecureRuntimeFile(KeyPath))
    {
        IFileManager::Get().Delete(*KeyPath);
        return 1;
    }
    const bool bJoined = RunProcess(Tailscale, {
        TEXT("up"), TEXT("--auth-key=file:") + KeyPath, TEXT("--hostname=") + Hostname,
        TEXT("--advertise-tags=") + Tag, TEXT("--unattended=true"), TEXT("--accept-routes=false"),
        TEXT("--accept-dns=true"), TEXT("--shields-up=false")});
    WriteText(KeyPath, FString());
    IFileManager::Get().Delete(*KeyPath);
    Key.Empty();
    if (!bJoined
        || !RunProcess(Tailscale, {TEXT("funnel"), TEXT("reset")})
        || !RunProcess(Tailscale, {TEXT("serve"), TEXT("reset")})
        || !RunProcess(Tailscale, {TEXT("serve"), TEXT("--bg"), TEXT("--https=443"), TEXT("http://127.0.0.1:8080")}))
    {
        return 1;
    }
    FString StatusText;
    if (!RunProcess(Tailscale, {TEXT("status"), TEXT("--json")}, &StatusText))
    {
        return 1;
    }
    TSharedPtr<FJsonObject> Status;
    const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(StatusText);
    if (!FJsonSerializer::Deserialize(Reader, Status) || !Status.IsValid()) return 1;
    FString Dns = Status->GetObjectField(TEXT("Self"))->GetStringField(TEXT("DNSName"));
    Dns.RemoveFromEnd(TEXT("."));
    FString TailnetIp;
    for (const TSharedPtr<FJsonValue>& Value : Status->GetArrayField(TEXT("TailscaleIPs")))
    {
        const FString Candidate = Value->AsString();
        if (Candidate.StartsWith(TEXT("100."))) { TailnetIp = Candidate; break; }
    }
    if (Dns.IsEmpty() || TailnetIp.IsEmpty()
        || !SetMachineEnvironment(TEXT("DARKARISEN_STREAM_HOST"), Dns)
        || !SetMachineEnvironment(TEXT("DARKARISEN_TURN_HOST"), TailnetIp)
        || !SetMachineEnvironment(TEXT("DARKARISEN_LOCAL_IP"), TailnetIp)
        || !SetMachineEnvironment(TEXT("DARKARISEN_PRIVATE_OVERLAY"), TEXT("1")))
    {
        return 1;
    }
    const FString Evidence = FString::Printf(
        TEXT("{\n  \"dns_name\": \"%s\",\n  \"funnel_enabled\": false,\n  \"https_url\": \"https://%s\",\n  \"player_scope\": \"tailnet-policy-only\",\n  \"tailnet_ip\": \"%s\"\n}\n"),
        *Dns, *Dns, *TailnetIp);
    WriteText(FPaths::Combine(Runtime, TEXT("private-tailnet.json")), Evidence);
    UE_LOG(LogTemp, Display, TEXT("Private tailnet endpoint configured. Revoke the one-off key and delete its SSM parameter."));
    return 0;
#endif
}

#if PLATFORM_WINDOWS
bool VerifyTrustedNvidiaSignature(const FString& File)
{
    WINTRUST_FILE_INFO FileInfo{};
    FileInfo.cbStruct = sizeof(FileInfo);
    FileInfo.pcwszFilePath = *File;
    GUID Policy = WINTRUST_ACTION_GENERIC_VERIFY_V2;
    WINTRUST_DATA TrustData{};
    TrustData.cbStruct = sizeof(TrustData);
    TrustData.dwUIChoice = WTD_UI_NONE;
    TrustData.fdwRevocationChecks = WTD_REVOKE_NONE;
    TrustData.dwUnionChoice = WTD_CHOICE_FILE;
    TrustData.pFile = &FileInfo;
    TrustData.dwStateAction = WTD_STATEACTION_VERIFY;
    const LONG TrustResult = WinVerifyTrust(nullptr, &Policy, &TrustData);
    TrustData.dwStateAction = WTD_STATEACTION_CLOSE;
    WinVerifyTrust(nullptr, &Policy, &TrustData);
    if (TrustResult != ERROR_SUCCESS) return false;

    HCERTSTORE Store = nullptr;
    HCRYPTMSG Message = nullptr;
    DWORD Encoding = 0, Content = 0, Format = 0;
    if (!CryptQueryObject(CERT_QUERY_OBJECT_FILE, *File,
        CERT_QUERY_CONTENT_FLAG_PKCS7_SIGNED_EMBED,
        CERT_QUERY_FORMAT_FLAG_BINARY, 0, &Encoding, &Content, &Format, &Store, &Message, nullptr))
    {
        return false;
    }
    DWORD SignerSize = 0;
    bool bNvidia = false;
    if (CryptMsgGetParam(Message, CMSG_SIGNER_INFO_PARAM, 0, nullptr, &SignerSize) && SignerSize > 0)
    {
        TArray<uint8> Buffer;
        Buffer.SetNumUninitialized(SignerSize);
        if (CryptMsgGetParam(Message, CMSG_SIGNER_INFO_PARAM, 0, Buffer.GetData(), &SignerSize))
        {
            PCMSG_SIGNER_INFO Signer = reinterpret_cast<PCMSG_SIGNER_INFO>(Buffer.GetData());
            CERT_INFO CertInfo{};
            CertInfo.Issuer = Signer->Issuer;
            CertInfo.SerialNumber = Signer->SerialNumber;
            PCCERT_CONTEXT Cert = CertFindCertificateInStore(Store, X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 0, CERT_FIND_SUBJECT_CERT, &CertInfo, nullptr);
            if (Cert)
            {
                TCHAR Subject[1024];
                const DWORD Count = CertGetNameStringW(Cert, CERT_NAME_SIMPLE_DISPLAY_TYPE, 0, nullptr, Subject, UE_ARRAY_COUNT(Subject));
                if (Count > 1)
                {
                    bNvidia = FString(Subject).Contains(TEXT("NVIDIA"), ESearchCase::IgnoreCase);
                }
                CertFreeCertificateContext(Cert);
            }
        }
    }
    if (Message) CryptMsgClose(Message);
    if (Store) CertCloseStore(Store, 0);
    return bNvidia;
}
#endif

int32 InstallGridDriver(const FParsedArgs& Args)
{
#if !PLATFORM_WINDOWS
    return 1;
#else
    const FString Key = Args.Get(TEXT("s3-key"));
    const FString ExpectedHash = Args.Get(TEXT("sha256")).ToLower();
    const FString Bucket = Args.Get(TEXT("bucket"), TEXT("ec2-windows-nvidia-drivers"));
    const FString Region = Args.Get(TEXT("region"), TEXT("us-east-1"));
    if (Bucket != TEXT("ec2-windows-nvidia-drivers") || !Key.StartsWith(TEXT("latest/")) || !Key.EndsWith(TEXT(".exe")) || !IsHex(ExpectedHash, 64))
    {
        UE_LOG(LogTemp, Error, TEXT("GRID driver arguments are not an exact reviewed AWS NVIDIA installer."));
        return 2;
    }
    if (Args.HasFlag(TEXT("validate-only"))) return 0;
    if (!Args.HasFlag(TEXT("accept-eula")))
    {
        UE_LOG(LogTemp, Error, TEXT("--accept-eula is required after reviewing the AWS/NVIDIA GRID EULA."));
        return 2;
    }
    const FString Runtime = FPaths::Combine(DefaultRuntimeRoot(), TEXT("Nvidia"));
    EnsureDirectory(Runtime);
    const FString Installer = FPaths::Combine(Runtime, FPaths::GetCleanFilename(Key));
    if (!RunProcess(TEXT("aws"), {TEXT("s3"), TEXT("cp"), TEXT("s3://") + Bucket + TEXT("/") + Key, Installer, TEXT("--region"), Region})
        || !Sha256File(Installer).Equals(ExpectedHash, ESearchCase::IgnoreCase)
        || !VerifyTrustedNvidiaSignature(Installer))
    {
        UE_LOG(LogTemp, Error, TEXT("GRID driver integrity or NVIDIA signature verification failed."));
        return 1;
    }
    if (!RunProcess(Installer, {TEXT("-s")}))
    {
        return 1;
    }
    FString Gpu;
    if (!RunProcess(TEXT("nvidia-smi.exe"), {TEXT("--query-gpu=name,driver_version"), TEXT("--format=csv,noheader")}, &Gpu))
    {
        return 1;
    }
    UE_LOG(LogTemp, Display, TEXT("GRID driver installed and GPU detected: %s"), *Gpu.TrimStartAndEnd());
    if (!Args.HasFlag(TEXT("no-restart")))
    {
        return RunProcess(TEXT("shutdown.exe"), {TEXT("/r"), TEXT("/t"), TEXT("0")}) ? 0 : 1;
    }
    UE_LOG(LogTemp, Warning, TEXT("GRID driver installed; restart remains required before Unreal/NVENC validation."));
    return 0;
#endif
}

bool ServiceRunning(const FString& Name)
{
#if PLATFORM_WINDOWS
    FString Out;
    return RunProcess(TEXT("sc.exe"), {TEXT("query"), Name}, &Out) && Out.Contains(TEXT("RUNNING"), ESearchCase::IgnoreCase);
#else
    return false;
#endif
}

int32 CollectHostEvidence(const FParsedArgs& Args)
{
#if !PLATFORM_WINDOWS
    return 1;
#else
    const FString Runtime = NormalizeFullPath(Args.Get(TEXT("runtime"), DefaultRuntimeRoot()));
    FString Host = Args.Get(TEXT("stream-host"), Env(TEXT("DARKARISEN_STREAM_HOST")));
    if (Host.IsEmpty()) return 2;
    const bool bPrivate = Env(TEXT("DARKARISEN_PRIVATE_OVERLAY")) == TEXT("1");
    const TArray<FString> Services = bPrivate
        ? TArray<FString>{TEXT("DarkArisenTurn"), TEXT("DarkArisenSignalling"), TEXT("DarkArisenIdleShutdown"), TEXT("Tailscale")}
        : TArray<FString>{TEXT("DarkArisenTurn"), TEXT("DarkArisenSignalling"), TEXT("DarkArisenIdleShutdown"), TEXT("DarkArisenGateway")};
    bool bServices = true;
    TArray<TSharedPtr<FJsonValue>> ServiceJson;
    for (const FString& Service : Services)
    {
        const bool bRunning = ServiceRunning(Service);
        bServices &= bRunning;
        TSharedRef<FJsonObject> Row = MakeShared<FJsonObject>();
        Row->SetStringField(TEXT("name"), Service);
        Row->SetStringField(TEXT("status"), bRunning ? TEXT("Running") : TEXT("MissingOrStopped"));
        Row->SetBoolField(TEXT("passed"), bRunning);
        ServiceJson.Add(MakeShared<FJsonValueObject>(Row));
    }
    const int32 PlayerConnections = CountLoopbackPlayerConnections(8080);
    FString Netstat;
    RunProcess(TEXT("netstat.exe"), {TEXT("-ano"), TEXT("-p"), TEXT("tcp")}, &Netstat);
    int32 StreamerConnections = 0;
    for (const FString& Line : TArray<FString>{}) {}
    TArray<FString> Lines;
    Netstat.ParseIntoArrayLines(Lines, true);
    for (const FString& Line : Lines)
    {
        if (Line.Contains(TEXT(":8888")) && Line.Contains(TEXT("ESTABLISHED"), ESearchCase::IgnoreCase)) ++StreamerConnections;
    }
    const bool bSignalling = StreamerConnections == 1 && PlayerConnections >= 0 && PlayerConnections <= 1;
    FString GameLog;
    ReadText(FPaths::Combine(Runtime, TEXT("DarkArisen-streaming.log")), GameLog);
    const FString Lower = GameLog.ToLower();
    const bool bNvenc = (Lower.Contains(TEXT("nvenc")) || Lower.Contains(TEXT("nvidia video codec")))
        && (Lower.Contains(TEXT("initializ")) || Lower.Contains(TEXT("created")) || Lower.Contains(TEXT("active")) || Lower.Contains(TEXT("selected")))
        && !Lower.Contains(TEXT("software encoder"));
    FString Firewall;
    const bool bFirewall = RunProcess(TEXT("netsh.exe"), {TEXT("advfirewall"), TEXT("firewall"), TEXT("show"), TEXT("rule"), TEXT("name=Dark Arisen Private TCP")}, &Firewall)
        && Firewall.Contains(TEXT("Block"), ESearchCase::IgnoreCase)
        && Firewall.Contains(TEXT("Yes"), ESearchCase::IgnoreCase);
    bool bAccess = false;
    TSharedRef<FJsonObject> Access = MakeShared<FJsonObject>();
    if (bPrivate)
    {
        const FString Tailscale = NormalizeFullPath(Args.Get(TEXT("tailscale"), TEXT("C:/Program Files/Tailscale/tailscale.exe")));
        FString Funnel, Serve;
        const bool bFunnelCmd = RunProcess(Tailscale, {TEXT("funnel"), TEXT("status")}, &Funnel);
        const bool bServeCmd = RunProcess(Tailscale, {TEXT("serve"), TEXT("status")}, &Serve);
        const bool bFunnelDisabled = bFunnelCmd && !Funnel.Contains(TEXT("available on the internet"), ESearchCase::IgnoreCase) && !Funnel.Contains(TEXT("http://"));
        const bool bServeEnabled = bServeCmd && (Serve.Contains(TEXT("available within your tailnet"), ESearchCase::IgnoreCase) || Serve.Contains(TEXT("https://")));
        bAccess = bFunnelDisabled && bServeEnabled;
        Access->SetStringField(TEXT("mode"), TEXT("tailnet-only"));
        Access->SetBoolField(TEXT("funnel_disabled"), bFunnelDisabled);
        Access->SetBoolField(TEXT("serve_enabled"), bServeEnabled);
    }
    else
    {
        FString Code;
        bAccess = RunProcess(TEXT("curl.exe"), {TEXT("-s"), TEXT("-o"), TEXT("NUL"), TEXT("-w"), TEXT("%{http_code}"), TEXT("-I"), TEXT("https://") + Host + TEXT("/")}, &Code)
            && Code.TrimStartAndEnd() == TEXT("401");
        Access->SetStringField(TEXT("mode"), TEXT("public-password"));
        Access->SetStringField(TEXT("status_code"), Code.TrimStartAndEnd());
    }
    Access->SetBoolField(TEXT("passed"), bAccess);
    const bool bPassed = bServices && bSignalling && bNvenc && bFirewall && bAccess;

    TSharedRef<FJsonObject> Evidence = MakeShared<FJsonObject>();
    Evidence->SetStringField(TEXT("schema"), TEXT("dark-arisen.streaming-host-evidence.v3-cpp"));
    Evidence->SetStringField(TEXT("collected_at_utc"), FDateTime::UtcNow().ToIso8601());
    Evidence->SetBoolField(TEXT("passed"), bPassed);
    Evidence->SetArrayField(TEXT("services"), ServiceJson);
    TSharedRef<FJsonObject> Signalling = MakeShared<FJsonObject>();
    Signalling->SetBoolField(TEXT("passed"), bSignalling);
    Signalling->SetNumberField(TEXT("streamer_connections"), StreamerConnections);
    Signalling->SetNumberField(TEXT("player_proxy_connections"), PlayerConnections);
    Evidence->SetObjectField(TEXT("signalling"), Signalling);
    TSharedRef<FJsonObject> Nvenc = MakeShared<FJsonObject>();
    Nvenc->SetBoolField(TEXT("passed"), bNvenc);
    Nvenc->SetStringField(TEXT("log"), FPaths::Combine(Runtime, TEXT("DarkArisen-streaming.log")));
    Evidence->SetObjectField(TEXT("nvenc"), Nvenc);
    Evidence->SetBoolField(TEXT("private_ports_blocked"), bFirewall);
    Evidence->SetObjectField(TEXT("access"), Access);

    FString Json;
    FJsonSerializer::Serialize(Evidence, TJsonWriterFactory<>::Create(&Json));
    const FString Output = NormalizeFullPath(Args.Get(TEXT("output"), FPaths::Combine(RepoRoot(Args), TEXT("Saved/StreamingEvidence/host-preflight.json"))));
    if (!WriteText(Output, Json + TEXT("\n"))) return 1;
    if (!bPassed)
    {
        UE_LOG(LogTemp, Error, TEXT("Streaming host preflight failed. Evidence: %s"), *Output);
        return 1;
    }
    UE_LOG(LogTemp, Display, TEXT("Streaming host preflight passed. Evidence: %s"), *Output);
    return 0;
#endif
}
}

int32 StreamingCommand(const FString& Command, const FParsedArgs& Args)
{
    if (Command == TEXT("bootstrap-streaming")) return BootstrapStreaming(Args);
    if (Command == TEXT("stream-signalling")) return LaunchSignalling(Args);
    if (Command == TEXT("stream-turn")) return LaunchTurn(Args);
    if (Command == TEXT("stream-provider-shutdown")) return ProviderShutdown(Args);
    if (Command == TEXT("stream-idle-shutdown")) return IdleShutdown(Args);
    if (Command == TEXT("stream-launch")) return LaunchStream(Args);
    if (Command == TEXT("stream-deploy")) return DeployStream(Args);
    if (Command == TEXT("stream-install-services")) return InstallServices(Args);
    if (Command == TEXT("tailnet-join")) return TailnetJoin(Args);
    if (Command == TEXT("install-grid-driver")) return InstallGridDriver(Args);
    if (Command == TEXT("collect-host-evidence")) return CollectHostEvidence(Args);
    UE_LOG(LogTemp, Error, TEXT("Unknown streaming command: %s"), *Command);
    return 2;
}
}
