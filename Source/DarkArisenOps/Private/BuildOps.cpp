#include "DarkArisenOps.h"

#include "Dom/JsonObject.h"
#include "HAL/FileManager.h"
#include "Misc/DateTime.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

namespace DarkArisenOps
{
namespace
{
FString BuildScript(const FString& Engine)
{
#if PLATFORM_WINDOWS
    return FPaths::Combine(Engine, TEXT("Engine/Build/BatchFiles/Build.bat"));
#else
    return FPaths::Combine(Engine, TEXT("Engine/Build/BatchFiles/Linux/Build.sh"));
#endif
}

FString EditorCommand(const FString& Engine)
{
#if PLATFORM_WINDOWS
    return FPaths::Combine(Engine, TEXT("Engine/Binaries/Win64/UnrealEditor-Cmd.exe"));
#else
    return FPaths::Combine(Engine, TEXT("Engine/Binaries/Linux/UnrealEditor-Cmd"));
#endif
}

FString RunUat(const FString& Engine)
{
#if PLATFORM_WINDOWS
    return FPaths::Combine(Engine, TEXT("Engine/Build/BatchFiles/RunUAT.bat"));
#else
    return FPaths::Combine(Engine, TEXT("Engine/Build/BatchFiles/RunUAT.sh"));
#endif
}

bool CheckEngine55(const FString& Engine, FString* OutVersion = nullptr)
{
    TSharedPtr<FJsonObject> Version;
    if (!LoadJsonObject(FPaths::Combine(Engine, TEXT("Engine/Build/Build.version")), Version))
    {
        return false;
    }
    const int32 Major = Version->GetIntegerField(TEXT("MajorVersion"));
    const int32 Minor = Version->GetIntegerField(TEXT("MinorVersion"));
    const int32 Patch = Version->GetIntegerField(TEXT("PatchVersion"));
    if (OutVersion)
    {
        *OutVersion = FString::Printf(TEXT("%d.%d.%d"), Major, Minor, Patch);
    }
    return Major == 5 && Minor == 5;
}

bool CheckProject55(const FString& Root)
{
    TSharedPtr<FJsonObject> Project;
    return LoadJsonObject(FPaths::Combine(Root, TEXT("DarkArisen.uproject")), Project)
        && Project->GetStringField(TEXT("EngineAssociation")) == TEXT("5.5");
}

bool BuildTarget(const FString& Root, const FString& Engine, const FString& Target, const FString& Configuration)
{
    const FString Project = FPaths::Combine(Root, TEXT("DarkArisen.uproject"));
    return RunProcess(BuildScript(Engine), {
        Target,
        PlatformName(),
        Configuration,
        FString::Printf(TEXT("-Project=%s"), *Project),
        TEXT("-WaitMutex"),
        TEXT("-NoHotReloadFromIDE"),
        TEXT("-WarningsAsErrors")});
}

bool RunAutomation(const FString& Root, const FString& Engine)
{
    const FString Editor = EditorCommand(Engine);
    if (!FPaths::FileExists(Editor))
    {
        UE_LOG(LogTemp, Error, TEXT("UnrealEditor-Cmd is missing: %s"), *Editor);
        return false;
    }
    const FString LogDir = FPaths::Combine(Root, TEXT("Saved/Logs"));
    EnsureDirectory(LogDir);
#if PLATFORM_WINDOWS
    const FString LogPath = FPaths::Combine(LogDir, TEXT("DarkArisenAutomation.log"));
#else
    const FString LogPath = FPaths::Combine(LogDir, TEXT("DarkArisenAutomation-Linux.log"));
#endif
    const FString Project = FPaths::Combine(Root, TEXT("DarkArisen.uproject"));
    if (!RunProcess(Editor, {
        Project,
        TEXT("-NullRHI"),
        TEXT("-Unattended"),
        TEXT("-NoPause"),
        TEXT("-NoSplash"),
        TEXT("-NoP4"),
        TEXT("-ExecCmds=Automation RunTests DarkArisen.; Quit"),
        TEXT("-TestExit=Automation Test Queue Empty"),
        FString::Printf(TEXT("-AbsLog=%s"), *LogPath)}))
    {
        return false;
    }
    FString Log;
    if (!ReadText(LogPath, Log))
    {
        UE_LOG(LogTemp, Error, TEXT("Automation log is missing: %s"), *LogPath);
        return false;
    }
    // Unreal writes "Result={Failed}". A literal "Result={Fail}" is not a substring of it, so
    // that spelling alone can never match and would leave this gate resting on the log line
    // below it. Both spellings are accepted, and a reported failure counts as well as an error.
    if (Log.Contains(TEXT("Result={Failed}"))
        || Log.Contains(TEXT("Result={Fail}"))
        || Log.Contains(TEXT("Automation Test Failed"))
        || Log.Contains(TEXT("LogAutomationController: Error"))
        || Log.Contains(TEXT("LogAutomationController: Warning: Test Failed")))
    {
        UE_LOG(LogTemp, Error, TEXT("At least one DarkArisen automation test failed: %s"), *LogPath);
        return false;
    }
    if (!Log.Contains(TEXT("DarkArisen.DesignLaws.Constants")))
    {
        UE_LOG(LogTemp, Error, TEXT("Required design-law automation test did not run."));
        return false;
    }
    return true;
}

bool LooksCredentialLike(const FString& Filename)
{
    const FString Name = FPaths::GetCleanFilename(Filename).ToLower();
    const FString Ext = TEXT(".") + FPaths::GetExtension(Name, false);
    return Name == TEXT(".env")
        || Name.StartsWith(TEXT(".env."))
        || Name == TEXT("id_rsa")
        || Name.StartsWith(TEXT("credentials"))
        || Ext == TEXT(".pem") || Ext == TEXT(".pfx") || Ext == TEXT(".p12") || Ext == TEXT(".key")
        || Name.EndsWith(TEXT(".local.json")) || Name.EndsWith(TEXT(".local.psd1"));
}

bool ScanStageForCredentials(const FString& StageRoot)
{
    static const TSet<FString> TextExtensions = {
        TEXT("ini"), TEXT("json"), TEXT("txt"), TEXT("cfg"), TEXT("xml"), TEXT("yaml"), TEXT("yml")};
    TArray<FString> Files;
    IFileManager::Get().FindFilesRecursive(Files, *StageRoot, TEXT("*"), true, false, false);
    for (const FString& File : Files)
    {
        if (LooksCredentialLike(File))
        {
            UE_LOG(LogTemp, Error, TEXT("Candidate contains forbidden credential-like file: %s"), *File);
            return false;
        }
        const FString Extension = FPaths::GetExtension(File, false).ToLower();
        if (TextExtensions.Contains(Extension))
        {
            FString Text;
            if (ReadText(File, Text))
            {
                if (Text.Contains(TEXT("-----BEGIN PRIVATE KEY-----"))
                    || Text.Contains(TEXT("-----BEGIN RSA PRIVATE KEY-----"))
                    || Text.Contains(TEXT("-----BEGIN EC PRIVATE KEY-----"))
                    || Text.Contains(TEXT("-----BEGIN OPENSSH PRIVATE KEY-----"))
                    || Text.Contains(TEXT("tskey-")))
                {
                    UE_LOG(LogTemp, Error, TEXT("Candidate contains credential material: %s"), *File);
                    return false;
                }
                const int32 Akia = Text.Find(TEXT("AKIA"));
                if (Akia != INDEX_NONE && Text.Len() >= Akia + 20)
                {
                    bool bLooksLikeKey = true;
                    for (int32 Index = Akia + 4; Index < Akia + 20; ++Index)
                    {
                        const TCHAR C = Text[Index];
                        bLooksLikeKey &= FChar::IsDigit(C) || (C >= TEXT('A') && C <= TEXT('Z'));
                    }
                    if (bLooksLikeKey)
                    {
                        UE_LOG(LogTemp, Error, TEXT("Candidate contains AWS access-key material: %s"), *File);
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

bool WriteManifest(
    const FString& StageRoot,
    const FString& Revision,
    const FString& EngineVersion,
    const FString& Platform)
{
    TArray<FString> Files;
    IFileManager::Get().FindFilesRecursive(Files, *StageRoot, TEXT("*"), true, false, false);
    int64 Bytes = 0;
    for (const FString& File : Files)
    {
        const int64 Size = IFileManager::Get().FileSize(*File);
        if (Size > 0) Bytes += Size;
    }

    TSharedRef<FJsonObject> Manifest = MakeShared<FJsonObject>();
    Manifest->SetNumberField(TEXT("schema_version"), 1);
    Manifest->SetStringField(TEXT("artifact_kind"), TEXT("candidate"));
    Manifest->SetStringField(TEXT("alpha_status"), TEXT("not-accepted"));
    Manifest->SetStringField(TEXT("project"), TEXT("DarkArisen"));
    Manifest->SetStringField(TEXT("platform"), Platform);
    Manifest->SetStringField(TEXT("configuration"), TEXT("Shipping"));
    Manifest->SetStringField(TEXT("candidate_commit"), Revision);
    Manifest->SetStringField(TEXT("engine_version"), EngineVersion);
    Manifest->SetBoolField(TEXT("pixel_streaming"), true);
    Manifest->SetNumberField(TEXT("content_file_count"), Files.Num());
    Manifest->SetNumberField(TEXT("content_bytes"), static_cast<double>(Bytes));
    Manifest->SetStringField(TEXT("generated_utc"), FDateTime::UtcNow().ToIso8601());
    Manifest->SetStringField(TEXT("notice"), TEXT("Private single-reviewer candidate. Not an accepted Alpha until promoted with complete evidence."));

    FString Json;
    const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Json);
    if (!FJsonSerializer::Serialize(Manifest, Writer))
    {
        return false;
    }
    return WriteText(FPaths::Combine(StageRoot, TEXT("DarkArisen-build-manifest.json")), Json + TEXT("\n"));
}

bool CreateZip(const FString& StageRoot, const FString& ZipPath)
{
#if PLATFORM_WINDOWS
    return RunProcess(TEXT("tar.exe"), {TEXT("-a"), TEXT("-c"), TEXT("-f"), ZipPath, TEXT("-C"), StageRoot, TEXT(".")});
#else
    return RunProcess(TEXT("zip"), {TEXT("-q"), TEXT("-r"), ZipPath, TEXT(".")}, nullptr, nullptr, StageRoot);
#endif
}

bool ListZip(const FString& ZipPath, TArray<FString>& OutEntries)
{
    FString Out;
#if PLATFORM_WINDOWS
    if (!RunProcess(TEXT("tar.exe"), {TEXT("-tf"), ZipPath}, &Out))
#else
    if (!RunProcess(TEXT("unzip"), {TEXT("-Z1"), ZipPath}, &Out))
#endif
    {
        return false;
    }
    Out.ParseIntoArrayLines(OutEntries, true);
    return true;
}

bool ExtractZipText(const FString& ZipPath, const FString& Entry, FString& OutText)
{
#if PLATFORM_WINDOWS
    return RunProcess(TEXT("tar.exe"), {TEXT("-xOf"), ZipPath, Entry}, &OutText);
#else
    return RunProcess(TEXT("unzip"), {TEXT("-p"), ZipPath, Entry}, &OutText);
#endif
}

bool ValidateCandidateZip(const FString& Path, const FString& ExpectedPlatform, const FString& ExpectedCommit, const FString& ExpectedHash)
{
    if (!FPaths::FileExists(Path))
    {
        UE_LOG(LogTemp, Error, TEXT("Candidate ZIP is missing: %s"), *Path);
        return false;
    }
    const FString ActualHash = Sha256File(Path);
    if (!ActualHash.Equals(ExpectedHash, ESearchCase::IgnoreCase))
    {
        UE_LOG(LogTemp, Error, TEXT("Candidate checksum mismatch for %s"), *Path);
        return false;
    }
    TArray<FString> Entries;
    if (!ListZip(Path, Entries))
    {
        return false;
    }
    TArray<FString> Matches;
    for (const FString& Entry : Entries)
    {
        FString Clean = Entry;
        Clean.RemoveFromEnd(TEXT("/"));
        if (Clean.EndsWith(TEXT("DarkArisen-build-manifest.json")))
        {
            Matches.Add(Entry);
        }
    }
    if (Matches.Num() != 1)
    {
        UE_LOG(LogTemp, Error, TEXT("%s must contain exactly one build manifest."), *Path);
        return false;
    }
    FString ManifestText;
    if (!ExtractZipText(Path, Matches[0], ManifestText))
    {
        return false;
    }
    TSharedPtr<FJsonObject> Manifest;
    const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ManifestText);
    if (!FJsonSerializer::Deserialize(Reader, Manifest) || !Manifest.IsValid())
    {
        return false;
    }
    return Manifest->GetIntegerField(TEXT("schema_version")) == 1
        && Manifest->GetStringField(TEXT("artifact_kind")) == TEXT("candidate")
        && Manifest->GetStringField(TEXT("alpha_status")) == TEXT("not-accepted")
        && Manifest->GetStringField(TEXT("project")) == TEXT("DarkArisen")
        && Manifest->GetStringField(TEXT("platform")) == ExpectedPlatform
        && Manifest->GetStringField(TEXT("configuration")) == TEXT("Shipping")
        && Manifest->GetStringField(TEXT("candidate_commit")).Equals(ExpectedCommit, ESearchCase::IgnoreCase)
        && Manifest->GetBoolField(TEXT("pixel_streaming"))
        && Manifest->GetStringField(TEXT("engine_version")).StartsWith(TEXT("5.5."));
}
}

int32 RunnerCheckCommand(const FParsedArgs& Args)
{
    const FString Root = RepoRoot(Args);
    const FString Engine = EngineRoot(Args);
    if (Engine.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("UE55_ROOT or --engine is required."));
        return 1;
    }
#if PLATFORM_WINDOWS
    if (!FPlatformMisc::Is64bitOperatingSystem())
    {
        UE_LOG(LogTemp, Error, TEXT("Windows runner must be x64."));
        return 1;
    }
#elif PLATFORM_LINUX
    FString Uname;
    if (!RunProcess(TEXT("uname"), {TEXT("-m")}, &Uname) || !Uname.Contains(TEXT("x86_64")))
    {
        UE_LOG(LogTemp, Error, TEXT("Linux runner must be x86_64."));
        return 1;
    }
#endif
    const TArray<FString> RequiredFiles = {
        FPaths::Combine(Root, TEXT("DarkArisen.uproject")),
        FPaths::Combine(Engine, TEXT("Engine/Build/Build.version")),
        BuildScript(Engine),
        EditorCommand(Engine)};
    for (const FString& Required : RequiredFiles)
    {
        if (!FPaths::FileExists(Required))
        {
            UE_LOG(LogTemp, Error, TEXT("Required runner file is missing: %s"), *Required);
            return 1;
        }
    }
    if (!CheckEngine55(Engine) || !CheckProject55(Root))
    {
        UE_LOG(LogTemp, Error, TEXT("Runner/project is not pinned to Unreal Engine 5.5."));
        return 1;
    }
    if (!RunProcess(TEXT("git"), {TEXT("lfs"), TEXT("version")}))
    {
        UE_LOG(LogTemp, Error, TEXT("Git LFS is required."));
        return 1;
    }
#if PLATFORM_LINUX
    FString ClangVersion;
    if (!RunProcess(TEXT("clang"), {TEXT("--version")}, &ClangVersion) || !ClangVersion.Contains(TEXT("version 18")))
    {
        UE_LOG(LogTemp, Error, TEXT("UE 5.5 Linux runner requires clang 18.x."));
        return 1;
    }
    const TArray<FString> LinuxCommands = {TEXT("zip"), TEXT("unzip"), TEXT("sha256sum")};
    for (const FString& Command : LinuxCommands)
    {
        if (!RunProcess(TEXT("which"), {Command}))
        {
            UE_LOG(LogTemp, Error, TEXT("Linux runner command is missing: %s"), *Command);
            return 1;
        }
    }
#else
    if (!RunProcess(TEXT("where.exe"), {TEXT("tar.exe")}))
    {
        UE_LOG(LogTemp, Error, TEXT("Windows bsdtar is required for private candidate archives."));
        return 1;
    }
#endif
    // Atoi64 yields 0 for a non-numeric argument, which would silently disable the gate.
    // The floor may be raised, never lowered or switched off.
    constexpr int64 MinimumGbFloor = 120;
    const int64 RequestedGb = FCString::Atoi64(*Args.Get(TEXT("min-disk-gb"), TEXT("120")));
    const int64 MinimumGb = FMath::Max(RequestedGb, MinimumGbFloor);
    const int64 FreeBytes = FreeDiskBytes(Root);
    if (FreeBytes < MinimumGb * 1024ll * 1024ll * 1024ll)
    {
        UE_LOG(LogTemp, Error, TEXT("Runner free disk is below %lld GB."), MinimumGb);
        return 1;
    }
    UE_LOG(LogTemp, Display, TEXT("Runner preflight passed: %s x64, UE 5.5, Git LFS, native C++ ops, %lld GB free."), *PlatformName(), FreeBytes / 1024ll / 1024ll / 1024ll);
    return 0;
}

int32 BuildCommand(const FParsedArgs& Args)
{
    const FString Root = RepoRoot(Args);
    const FString Engine = EngineRoot(Args);
    if (RunnerCheckCommand(Args) != 0)
    {
        return 1;
    }
    if (!BuildTarget(Root, Engine, TEXT("DarkArisenEditor"), TEXT("Development"))
        || !BuildTarget(Root, Engine, TEXT("DarkArisen"), TEXT("Development"))
        || !BuildTarget(Root, Engine, TEXT("DarkArisen"), TEXT("Shipping")))
    {
        return 1;
    }
    if (Args.HasFlag(TEXT("automation")) && !RunAutomation(Root, Engine))
    {
        return 1;
    }
    UE_LOG(LogTemp, Display, TEXT("Unreal build%s passed."), Args.HasFlag(TEXT("automation")) ? TEXT(" + automation") : TEXT(""));
    return 0;
}

int32 PackageAlphaCommand(const FParsedArgs& Args)
{
    const FString Root = RepoRoot(Args);
    const FString Engine = EngineRoot(Args);
    const FString Revision = Args.Get(TEXT("revision")).ToLower();
    if (!IsHex(Revision, 40))
    {
        UE_LOG(LogTemp, Error, TEXT("--revision must be a complete 40-character commit SHA."));
        return 2;
    }
    if (RunnerCheckCommand(Args) != 0 || !Revision.Equals(GitHead(Root), ESearchCase::IgnoreCase) || !IsTrackedTreeClean(Root))
    {
        UE_LOG(LogTemp, Error, TEXT("Package only an exact clean checked-out revision."));
        return 1;
    }
    FString EngineVersion;
    if (!CheckEngine55(Engine, &EngineVersion))
    {
        return 1;
    }
    const FString OutputRoot = NormalizeFullPath(Args.Get(TEXT("output"), FPaths::Combine(Root, TEXT("Artifacts/AlphaCandidates"))));
    if (IsFilesystemRoot(OutputRoot))
    {
        UE_LOG(LogTemp, Error, TEXT("Output directory may not be a filesystem root."));
        return 1;
    }
    const FString StageRoot = FPaths::Combine(Root, TEXT("Saved/AlphaPackaging"), PlatformName(), Revision);
    if (IFileManager::Get().DirectoryExists(*StageRoot) || !EnsureDirectory(StageRoot) || !EnsureDirectory(OutputRoot))
    {
        UE_LOG(LogTemp, Error, TEXT("Candidate staging path already exists or cannot be created: %s"), *StageRoot);
        return 1;
    }
    const FString Project = FPaths::Combine(Root, TEXT("DarkArisen.uproject"));
    TArray<FString> UatArgs = {
        TEXT("BuildCookRun"),
        FString::Printf(TEXT("-project=%s"), *Project),
        TEXT("-noP4"),
        FString::Printf(TEXT("-platform=%s"), *PlatformName()),
        TEXT("-clientconfig=Shipping"),
        TEXT("-build"), TEXT("-cook"), TEXT("-allmaps"), TEXT("-stage"), TEXT("-pak"), TEXT("-archive"),
        FString::Printf(TEXT("-archivedirectory=%s"), *StageRoot),
        TEXT("-nodebuginfo"), TEXT("-utf8output")};
#if PLATFORM_WINDOWS
    UatArgs.Add(TEXT("-prereqs"));
#endif
    if (!RunProcess(RunUat(Engine), UatArgs))
    {
        return 1;
    }
    TArray<FString> Executables;
    TArray<FString> Paks;
#if PLATFORM_WINDOWS
    IFileManager::Get().FindFilesRecursive(Executables, *StageRoot, TEXT("DarkArisen.exe"), true, false, false);
#else
    IFileManager::Get().FindFilesRecursive(Executables, *StageRoot, TEXT("DarkArisen"), true, false, false);
    if (Executables.IsEmpty())
    {
        IFileManager::Get().FindFilesRecursive(Executables, *StageRoot, TEXT("DarkArisen-Linux-Shipping"), true, false, false);
    }
#endif
    IFileManager::Get().FindFilesRecursive(Paks, *StageRoot, TEXT("*.pak"), true, false, false);
    if (Executables.IsEmpty() || Paks.IsEmpty() || !ScanStageForCredentials(StageRoot)
        || !WriteManifest(StageRoot, Revision, EngineVersion, PlatformName()))
    {
        UE_LOG(LogTemp, Error, TEXT("Packaged payload verification failed."));
        return 1;
    }
    const FString Short = Revision.Left(12);
#if PLATFORM_WINDOWS
    const FString ZipName = FString::Printf(TEXT("DarkArisen-Candidate-Windows-Arcware-%s.zip"), *Short);
#else
    const FString ZipName = FString::Printf(TEXT("DarkArisen-Candidate-Linux-x86_64-%s.zip"), *Short);
#endif
    const FString ZipPath = FPaths::Combine(OutputRoot, ZipName);
    if (FPaths::FileExists(ZipPath) || !CreateZip(StageRoot, ZipPath))
    {
        UE_LOG(LogTemp, Error, TEXT("Candidate archive already exists or could not be created: %s"), *ZipPath);
        return 1;
    }
    const FString Hash = Sha256File(ZipPath);
    if (!IsHex(Hash, 64) || !WriteText(ZipPath + TEXT(".sha256"), Hash + TEXT("  ") + ZipName + TEXT("\n")))
    {
        return 1;
    }
    UE_LOG(LogTemp, Display, TEXT("Private candidate created: %s\nSHA-256: %s"), *ZipPath, *Hash);
    return 0;
}

int32 PromoteAlphaCommand(const FParsedArgs& Args)
{
    const FString EvidenceArg = Args.Get(TEXT("evidence"));
    const FString WindowsArg = Args.Get(TEXT("windows"));
    const FString LinuxArg = Args.Get(TEXT("linux"));
    const FString OutputArg = Args.Get(TEXT("output"));
    if (EvidenceArg.IsEmpty() || WindowsArg.IsEmpty() || LinuxArg.IsEmpty() || OutputArg.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("promote-alpha requires --evidence, --windows, --linux and --output."));
        return 2;
    }
    const FString EvidencePath = NormalizeFullPath(EvidenceArg);
    const FString WindowsPath = NormalizeFullPath(WindowsArg);
    const FString LinuxPath = NormalizeFullPath(LinuxArg);
    const FString Output = NormalizeFullPath(OutputArg);
    if (IsFilesystemRoot(Output))
    {
        return 2;
    }
    TSharedPtr<FJsonObject> Evidence;
    if (!LoadJsonObject(EvidencePath, Evidence)
        || Evidence->GetIntegerField(TEXT("schema_version")) != 1
        || Evidence->GetStringField(TEXT("status")) != TEXT("accepted-alpha"))
    {
        UE_LOG(LogTemp, Error, TEXT("Evidence must use schema 1 and status accepted-alpha."));
        return 1;
    }
    const FString Commit = Evidence->GetStringField(TEXT("candidate_commit")).ToLower();
    if (!IsHex(Commit, 40))
    {
        return 1;
    }
    const TSharedPtr<FJsonObject> Gates = Evidence->GetObjectField(TEXT("gates"));
    const TArray<FString> RequiredGates = {
        TEXT("full_content_m0_m8"),
        TEXT("windows_ue55_build_and_automation"),
        TEXT("linux_ue55_build_and_automation"),
        TEXT("windows_start_to_credits_playthrough"),
        TEXT("linux_start_to_credits_playthrough"),
        TEXT("content_manifest_reconciled"),
        TEXT("performance_floor_60_fps"),
        TEXT("privacy_access_denied"),
        TEXT("credits_music_and_licences_approved"),
        TEXT("flo_alpha_approval")};
    for (const FString& Gate : RequiredGates)
    {
        bool bGreen = false;
        if (!Gates.IsValid() || !Gates->TryGetBoolField(Gate, bGreen) || !bGreen)
        {
            UE_LOG(LogTemp, Error, TEXT("Alpha acceptance gate is not green: %s"), *Gate);
            return 1;
        }
    }
    const TSharedPtr<FJsonObject> Artifacts = Evidence->GetObjectField(TEXT("artifacts"));
    if (!Artifacts.IsValid()) return 1;
    const FString WindowsHash = Artifacts->GetObjectField(TEXT("windows"))->GetStringField(TEXT("sha256")).ToLower();
    const FString LinuxHash = Artifacts->GetObjectField(TEXT("linux"))->GetStringField(TEXT("sha256")).ToLower();
    if (!IsHex(WindowsHash, 64) || !IsHex(LinuxHash, 64)
        || !ValidateCandidateZip(WindowsPath, TEXT("Win64"), Commit, WindowsHash)
        || !ValidateCandidateZip(LinuxPath, TEXT("Linux"), Commit, LinuxHash))
    {
        UE_LOG(LogTemp, Error, TEXT("Candidate validation failed; promotion refused."));
        return 1;
    }
    if (!EnsureDirectory(Output))
    {
        return 1;
    }
    struct FPromotion
    {
        FString Source;
        FString Hash;
        FString Name;
    };
    const TArray<FPromotion> Items = {
        {WindowsPath, WindowsHash, TEXT("DarkArisen-Alpha-Windows-Arcware.zip")},
        {LinuxPath, LinuxHash, TEXT("DarkArisen-Alpha-Linux-x86_64.zip")}};
    TArray<FString> Partials;
    for (const FPromotion& Item : Items)
    {
        const FString Destination = FPaths::Combine(Output, Item.Name);
        const FString Partial = Destination + TEXT(".partial");
        if (FPaths::FileExists(Destination) || FPaths::FileExists(Destination + TEXT(".sha256"))
            || IFileManager::Get().Copy(*Partial, *Item.Source, true, true) != COPY_OK
            || !Sha256File(Partial).Equals(Item.Hash, ESearchCase::IgnoreCase))
        {
            UE_LOG(LogTemp, Error, TEXT("Promotion copy verification failed for %s"), *Item.Name);
            for (const FString& Existing : Partials) IFileManager::Get().Delete(*Existing);
            IFileManager::Get().Delete(*Partial);
            return 1;
        }
        Partials.Add(Partial);
    }
    for (int32 Index = 0; Index < Items.Num(); ++Index)
    {
        const FPromotion& Item = Items[Index];
        const FString Destination = FPaths::Combine(Output, Item.Name);
        if (!IFileManager::Get().Move(*Destination, *Partials[Index], true, false, false, true)
            || !WriteText(Destination + TEXT(".sha256"), Item.Hash + TEXT("  ") + Item.Name + TEXT("\n")))
        {
            return 1;
        }
        UE_LOG(LogTemp, Display, TEXT("Accepted private Alpha artifact: %s"), *Destination);
    }
    return 0;
}
}
