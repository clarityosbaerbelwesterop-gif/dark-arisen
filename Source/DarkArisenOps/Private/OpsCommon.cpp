#include "DarkArisenOps.h"

#include "Dom/JsonObject.h"
#include "HAL/FileManager.h"
#include "HAL/PlatformProcess.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

#if PLATFORM_WINDOWS
#include "Windows/AllowWindowsPlatformTypes.h"
#include <Windows.h>
#include "Windows/HideWindowsPlatformTypes.h"
#elif PLATFORM_LINUX
#include <sys/statvfs.h>
#endif

namespace DarkArisenOps
{
FString FParsedArgs::Get(const FString& Key, const FString& Default) const
{
    if (const FString* Value = Values.Find(Key))
    {
        return *Value;
    }
    return Default;
}

bool FParsedArgs::HasFlag(const FString& Key) const
{
    return Flags.Contains(Key);
}

FParsedArgs ParseArgs(int32 ArgC, TCHAR* ArgV[])
{
    FParsedArgs Parsed;
    for (int32 Index = 2; Index < ArgC; ++Index)
    {
        FString Token(ArgV[Index]);
        if (Token.StartsWith(TEXT("--")))
        {
            Token.RightChopInline(2, EAllowShrinking::No);
            FString Key;
            FString Value;
            if (Token.Split(TEXT("="), &Key, &Value))
            {
                Parsed.Values.Add(Key, Value);
            }
            else
            {
                Parsed.Flags.Add(Token);
            }
        }
        else
        {
            Parsed.Positionals.Add(Token);
        }
    }
    return Parsed;
}

FString NormalizeFullPath(const FString& Path)
{
    FString Result = FPaths::ConvertRelativePathToFull(Path);
    FPaths::NormalizeFilename(Result);
    return Result;
}

FString RepoRoot(const FParsedArgs& Args)
{
    const FString Explicit = Args.Get(TEXT("root"));
    if (!Explicit.IsEmpty())
    {
        return NormalizeFullPath(Explicit);
    }
    return NormalizeFullPath(FPaths::Combine(FPlatformProcess::BaseDir(), TEXT(".."), TEXT("..")));
}

FString EngineRoot(const FParsedArgs& Args)
{
    const FString Explicit = Args.Get(TEXT("engine"));
    if (!Explicit.IsEmpty())
    {
        return NormalizeFullPath(Explicit);
    }
    const FString Env = FPlatformMisc::GetEnvironmentVariable(TEXT("UE55_ROOT"));
    return Env.IsEmpty() ? FString() : NormalizeFullPath(Env);
}

FString Quote(const FString& Value)
{
    FString Escaped = Value;
    Escaped.ReplaceInline(TEXT("\""), TEXT("\\\""));
    return FString::Printf(TEXT("\"%s\""), *Escaped);
}

FString JoinParams(const TArray<FString>& Args)
{
    FString Result;
    for (const FString& Arg : Args)
    {
        if (!Result.IsEmpty())
        {
            Result += TEXT(" ");
        }
        Result += Arg.Contains(TEXT(" ")) || Arg.Contains(TEXT("\t")) ? Quote(Arg) : Arg;
    }
    return Result;
}

bool RunProcess(
    const FString& Executable,
    const TArray<FString>& Arguments,
    FString* OutStdOut,
    FString* OutStdErr,
    const FString& WorkingDirectory,
    int32* OutExitCode)
{
    int32 ExitCode = INDEX_NONE;
    FString StdOut;
    FString StdErr;
    const FString Params = JoinParams(Arguments);
    const bool bStarted = FPlatformProcess::ExecProcess(
        *Executable,
        *Params,
        &ExitCode,
        &StdOut,
        &StdErr,
        WorkingDirectory.IsEmpty() ? nullptr : *WorkingDirectory);

    if (OutStdOut) *OutStdOut = StdOut;
    if (OutStdErr) *OutStdErr = StdErr;
    if (OutExitCode) *OutExitCode = ExitCode;

    if (!bStarted || ExitCode != 0)
    {
        UE_LOG(LogTemp, Error, TEXT("Command failed (%d): %s %s\n%s"), ExitCode, *Executable, *Params, *StdErr);
        return false;
    }
    return true;
}

bool ReadText(const FString& Path, FString& OutText)
{
    return FFileHelper::LoadFileToString(OutText, *Path);
}

bool EnsureDirectory(const FString& Path)
{
    return IFileManager::Get().MakeDirectory(*Path, true);
}

bool WriteText(const FString& Path, const FString& Text)
{
    const FString Parent = FPaths::GetPath(Path);
    if (!Parent.IsEmpty() && !EnsureDirectory(Parent))
    {
        return false;
    }
    return FFileHelper::SaveStringToFile(Text, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
}

bool LoadJsonObject(const FString& Path, TSharedPtr<FJsonObject>& OutObject)
{
    FString Text;
    if (!ReadText(Path, Text))
    {
        UE_LOG(LogTemp, Error, TEXT("Cannot read JSON file: %s"), *Path);
        return false;
    }
    const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Text);
    if (!FJsonSerializer::Deserialize(Reader, OutObject) || !OutObject.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid JSON file: %s"), *Path);
        return false;
    }
    return true;
}

bool IsHex(const FString& Value, int32 RequiredLength)
{
    if (Value.Len() != RequiredLength)
    {
        return false;
    }
    for (const TCHAR Character : Value)
    {
        if (!FChar::IsHexDigit(Character))
        {
            return false;
        }
    }
    return true;
}

bool IsSafeRevision(const FString& Value)
{
    if (Value.IsEmpty())
    {
        return false;
    }
    for (const TCHAR Character : Value)
    {
        if (!(FChar::IsAlnum(Character) || Character == TEXT('.') || Character == TEXT('_') || Character == TEXT('-')))
        {
            return false;
        }
    }
    return true;
}

bool IsFilesystemRoot(const FString& Path)
{
    const FString Full = NormalizeFullPath(Path);
#if PLATFORM_WINDOWS
    return Full.Len() <= 3 && Full.Len() >= 2 && Full[1] == TEXT(':');
#else
    return Full == TEXT("/");
#endif
}

FString PlatformName()
{
#if PLATFORM_WINDOWS
    return TEXT("Win64");
#elif PLATFORM_LINUX
    return TEXT("Linux");
#else
    return TEXT("Unsupported");
#endif
}

FString GitHead(const FString& Root)
{
    FString Out;
    if (!RunProcess(TEXT("git"), {TEXT("-C"), Root, TEXT("rev-parse"), TEXT("HEAD")}, &Out))
    {
        return FString();
    }
    Out.TrimStartAndEndInline();
    return Out.ToLower();
}

bool IsTrackedTreeClean(const FString& Root)
{
    FString Out;
    if (!RunProcess(TEXT("git"), {TEXT("-C"), Root, TEXT("status"), TEXT("--porcelain"), TEXT("--untracked-files=no")}, &Out))
    {
        return false;
    }
    Out.TrimStartAndEndInline();
    return Out.IsEmpty();
}

FString Sha256File(const FString& Path)
{
#if PLATFORM_WINDOWS
    FString Out;
    if (!RunProcess(TEXT("certutil.exe"), {TEXT("-hashfile"), Path, TEXT("SHA256")}, &Out))
    {
        return FString();
    }
    TArray<FString> Lines;
    Out.ParseIntoArrayLines(Lines, true);
    for (FString Line : Lines)
    {
        Line.ReplaceInline(TEXT(" "), TEXT(""));
        Line.TrimStartAndEndInline();
        if (IsHex(Line, 64))
        {
            return Line.ToLower();
        }
    }
#else
    FString Out;
    if (!RunProcess(TEXT("sha256sum"), {Path}, &Out))
    {
        return FString();
    }
    FString Hash;
    FString Rest;
    if (Out.Split(TEXT(" "), &Hash, &Rest))
    {
        Hash.TrimStartAndEndInline();
        if (IsHex(Hash, 64))
        {
            return Hash.ToLower();
        }
    }
#endif
    return FString();
}

int64 FreeDiskBytes(const FString& Path)
{
#if PLATFORM_WINDOWS
    ULARGE_INTEGER FreeBytesAvailable{};
    if (GetDiskFreeSpaceExW(*NormalizeFullPath(Path), &FreeBytesAvailable, nullptr, nullptr))
    {
        return static_cast<int64>(FreeBytesAvailable.QuadPart);
    }
#elif PLATFORM_LINUX
    struct statvfs Stats{};
    const FTCHARToUTF8 Utf8(*NormalizeFullPath(Path));
    if (statvfs(Utf8.Get(), &Stats) == 0)
    {
        return static_cast<int64>(Stats.f_bavail) * static_cast<int64>(Stats.f_frsize);
    }
#endif
    return -1;
}

FString OpsBinaryPath(const FString& Root)
{
#if PLATFORM_WINDOWS
    return FPaths::Combine(Root, TEXT("Binaries/Win64/DarkArisenOps.exe"));
#else
    return FPaths::Combine(Root, TEXT("Binaries/Linux/DarkArisenOps"));
#endif
}
}
