#pragma once

#include "CoreMinimal.h"

class FJsonObject;

namespace DarkArisenOps
{
struct FParsedArgs
{
    TMap<FString, FString> Values;
    TSet<FString> Flags;
    TArray<FString> Positionals;

    FString Get(const FString& Key, const FString& Default = FString()) const;
    bool HasFlag(const FString& Key) const;
};

FParsedArgs ParseArgs(int32 ArgC, TCHAR* ArgV[]);
FString RepoRoot(const FParsedArgs& Args);
FString EngineRoot(const FParsedArgs& Args);
FString Quote(const FString& Value);
FString JoinParams(const TArray<FString>& Args);

bool RunProcess(
    const FString& Executable,
    const TArray<FString>& Arguments,
    FString* OutStdOut = nullptr,
    FString* OutStdErr = nullptr,
    const FString& WorkingDirectory = FString(),
    int32* OutExitCode = nullptr);

bool ReadText(const FString& Path, FString& OutText);
bool WriteText(const FString& Path, const FString& Text);
bool LoadJsonObject(const FString& Path, TSharedPtr<FJsonObject>& OutObject);
bool IsHex(const FString& Value, int32 RequiredLength);
bool IsSafeRevision(const FString& Value);
bool EnsureDirectory(const FString& Path);
bool IsFilesystemRoot(const FString& Path);
FString NormalizeFullPath(const FString& Path);
FString Sha256File(const FString& Path);
bool IsTrackedTreeClean(const FString& Root);
FString GitHead(const FString& Root);
int64 FreeDiskBytes(const FString& Path);
FString PlatformName();
FString OpsBinaryPath(const FString& Root);

int32 ValidateCommand(const FParsedArgs& Args);
int32 ValidateDungeonContentCommand(const FParsedArgs& Args);
int32 ValidateMissionContentCommand(const FParsedArgs& Args);
int32 ValidateBossContentCommand(const FParsedArgs& Args);
int32 RunnerCheckCommand(const FParsedArgs& Args);
int32 BuildCommand(const FParsedArgs& Args);
int32 PackageAlphaCommand(const FParsedArgs& Args);
int32 PromoteAlphaCommand(const FParsedArgs& Args);
int32 StudioKimiCommand(const FParsedArgs& Args);
int32 StreamingCommand(const FString& Command, const FParsedArgs& Args);
}
