#include "DarkArisenOps.h"

#include "Dom/JsonObject.h"
#include "HAL/PlatformMisc.h"
#include "HAL/PlatformProcess.h"
#include "HttpManager.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Misc/DateTime.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

namespace DarkArisenOps
{
namespace
{
constexpr const TCHAR* NvidiaKimiEndpoint = TEXT("https://integrate.api.nvidia.com/v1/chat/completions");
constexpr const TCHAR* NvidiaKimiModel = TEXT("moonshotai/kimi-k3");
constexpr int32 DefaultMaxTokens = 4096;
constexpr int32 MinimumMaxTokens = 512;
constexpr int32 MaximumMaxTokens = 16384;
constexpr int32 MaximumContextFiles = 64;
constexpr int32 MaximumContextCharacters = 750000;
constexpr double RequestTimeoutSeconds = 900.0;

bool IsPathInside(const FString& Root, const FString& Candidate)
{
    FString NormalRoot = NormalizeFullPath(Root);
    FString NormalCandidate = NormalizeFullPath(Candidate);
    FPaths::NormalizeDirectoryName(NormalRoot);
    FPaths::NormalizeFilename(NormalCandidate);

    if (NormalCandidate.Equals(NormalRoot, ESearchCase::IgnoreCase))
    {
        return true;
    }
    if (!NormalRoot.EndsWith(TEXT("/")))
    {
        NormalRoot += TEXT("/");
    }
    return NormalCandidate.StartsWith(NormalRoot, ESearchCase::IgnoreCase);
}

bool ResolveRepoFile(
    const FString& Root,
    const FString& Relative,
    FString& OutFullPath,
    FString& OutError)
{
    if (Relative.TrimStartAndEnd().IsEmpty())
    {
        OutError = TEXT("A required repository-relative file path is empty.");
        return false;
    }
    if (FPaths::IsRelative(Relative) == false)
    {
        OutError = FString::Printf(TEXT("Absolute input paths are forbidden: %s"), *Relative);
        return false;
    }

    const FString FullPath = NormalizeFullPath(FPaths::Combine(Root, Relative));
    if (!IsPathInside(Root, FullPath))
    {
        OutError = FString::Printf(TEXT("Input path escapes repository root: %s"), *Relative);
        return false;
    }

    const FString Lower = Relative.ToLower();
    const TArray<FString> ForbiddenSuffixes = {
        TEXT(".env"), TEXT(".pem"), TEXT(".key"), TEXT(".pfx"), TEXT(".p12"), TEXT(".keystore")};
    for (const FString& Suffix : ForbiddenSuffixes)
    {
        if (Lower.EndsWith(Suffix))
        {
            OutError = FString::Printf(TEXT("Sensitive file type may not be sent to an AI provider: %s"), *Relative);
            return false;
        }
    }

    if (Lower.Contains(TEXT("/saved/"))
        || Lower.Contains(TEXT("/binaries/"))
        || Lower.Contains(TEXT("/intermediate/")))
    {
        OutError = FString::Printf(TEXT("Generated/runtime directory may not be used as Kimi context: %s"), *Relative);
        return false;
    }

    if (!FPaths::FileExists(FullPath))
    {
        OutError = FString::Printf(TEXT("Kimi context file does not exist: %s"), *Relative);
        return false;
    }

    OutFullPath = FullPath;
    return true;
}

bool ResolveProposalOutput(
    const FString& Root,
    const FString& Relative,
    FString& OutFullPath,
    FString& OutError)
{
    if (Relative.TrimStartAndEnd().IsEmpty())
    {
        OutError = TEXT("--output is required.");
        return false;
    }
    if (!FPaths::IsRelative(Relative))
    {
        OutError = TEXT("Kimi output must be repository-relative.");
        return false;
    }

    FString NormalRelative = Relative;
    FPaths::NormalizeFilename(NormalRelative);
    if (!NormalRelative.StartsWith(TEXT("Docs/AIProposals/"), ESearchCase::CaseSensitive))
    {
        OutError = TEXT("Kimi output is restricted to Docs/AIProposals/ review material.");
        return false;
    }
    if (!NormalRelative.EndsWith(TEXT(".md"), ESearchCase::IgnoreCase))
    {
        OutError = TEXT("Kimi proposal output must be a Markdown review file.");
        return false;
    }

    const FString FullPath = NormalizeFullPath(FPaths::Combine(Root, NormalRelative));
    if (!IsPathInside(Root, FullPath))
    {
        OutError = TEXT("Kimi output path escapes repository root.");
        return false;
    }

    OutFullPath = FullPath;
    return true;
}

bool ParseBoundedMaxTokens(const FString& Raw, int32& OutValue, FString& OutError)
{
    if (Raw.IsEmpty())
    {
        OutValue = DefaultMaxTokens;
        return true;
    }

    for (const TCHAR Char : Raw)
    {
        if (!FChar::IsDigit(Char))
        {
            OutError = TEXT("--max-tokens must be an integer.");
            return false;
        }
    }

    const int64 Parsed = FCString::Atoi64(*Raw);
    if (Parsed < MinimumMaxTokens || Parsed > MaximumMaxTokens)
    {
        OutError = FString::Printf(
            TEXT("--max-tokens must remain between %d and %d."),
            MinimumMaxTokens,
            MaximumMaxTokens);
        return false;
    }
    OutValue = static_cast<int32>(Parsed);
    return true;
}

bool AppendContextFiles(
    const FString& Root,
    const FString& ContextListRelative,
    FString& InOutPrompt,
    TArray<FString>& OutIncludedFiles,
    FString& OutError)
{
    if (ContextListRelative.IsEmpty())
    {
        return true;
    }

    FString ContextListPath;
    if (!ResolveRepoFile(Root, ContextListRelative, ContextListPath, OutError))
    {
        return false;
    }

    FString ContextList;
    if (!ReadText(ContextListPath, ContextList))
    {
        OutError = FString::Printf(TEXT("Cannot read context list: %s"), *ContextListRelative);
        return false;
    }

    TArray<FString> Lines;
    ContextList.ParseIntoArrayLines(Lines, false);
    int32 AddedFiles = 0;
    int32 AddedCharacters = 0;
    for (FString Line : Lines)
    {
        Line.TrimStartAndEndInline();
        if (Line.IsEmpty() || Line.StartsWith(TEXT("#")))
        {
            continue;
        }
        if (++AddedFiles > MaximumContextFiles)
        {
            OutError = FString::Printf(TEXT("Context list exceeds %d files."), MaximumContextFiles);
            return false;
        }

        FString FullPath;
        if (!ResolveRepoFile(Root, Line, FullPath, OutError))
        {
            return false;
        }
        FString Text;
        if (!ReadText(FullPath, Text))
        {
            OutError = FString::Printf(TEXT("Cannot read Kimi context file: %s"), *Line);
            return false;
        }

        AddedCharacters += Text.Len();
        if (AddedCharacters > MaximumContextCharacters)
        {
            OutError = FString::Printf(TEXT("Combined Kimi context exceeds %d characters."), MaximumContextCharacters);
            return false;
        }

        InOutPrompt += FString::Printf(
            TEXT("\n\n--- BEGIN GOVERNING FILE: %s ---\n%s\n--- END GOVERNING FILE: %s ---"),
            *Line,
            *Text,
            *Line);
        OutIncludedFiles.Add(Line);
    }
    return true;
}

FString BuildSystemPrompt(const FString& RoleText)
{
    return FString::Printf(
        TEXT("You are a bounded game-production specialist assisting Dark Arisen. "
             "Apply the supplied Game Studios role as methodology, but Dark Arisen's governing files override it. "
             "Never invent missing canon, names, dialogue, rewards, mechanics, numbers, sites, markers, HUD elements, music cues, cutscenes, romance, fast travel, or outcomes merely to complete a count. "
             "When a source is silent or contradictory, emit a DESIGN-GAP with the exact files/sections involved. "
             "Executable project implementation is native Unreal Engine 5.5 C++; asset/level/animation work must be specified as authored production requirements rather than falsely claimed as existing. "
             "Protected playable moments must remain player-controlled. Music is non-reactive. Dungeon ambient score, dungeon map markers and child remains are forbidden. "
             "Do not claim compilation, runtime, performance or playtest evidence. "
             "Return Markdown with sections: Sources used, Proposed production work, C++ integration points, Asset/animation requirements, QA/acceptance, DESIGN-GAPS.\n\n"
             "GAME STUDIOS ROLE PROFILE:\n%s"),
        *RoleText);
}

bool SerializeRequestBody(
    const FString& SystemPrompt,
    const FString& UserPrompt,
    const int32 MaxTokens,
    FString& OutBody)
{
    TSharedRef<FJsonObject> Root = MakeShared<FJsonObject>();
    Root->SetStringField(TEXT("model"), NvidiaKimiModel);
    Root->SetNumberField(TEXT("max_tokens"), MaxTokens);
    Root->SetNumberField(TEXT("temperature"), 0.2);
    Root->SetBoolField(TEXT("stream"), false);

    TArray<TSharedPtr<FJsonValue>> Messages;
    {
        TSharedRef<FJsonObject> Message = MakeShared<FJsonObject>();
        Message->SetStringField(TEXT("role"), TEXT("system"));
        Message->SetStringField(TEXT("content"), SystemPrompt);
        Messages.Add(MakeShared<FJsonValueObject>(Message));
    }
    {
        TSharedRef<FJsonObject> Message = MakeShared<FJsonObject>();
        Message->SetStringField(TEXT("role"), TEXT("user"));
        Message->SetStringField(TEXT("content"), UserPrompt);
        Messages.Add(MakeShared<FJsonValueObject>(Message));
    }
    Root->SetArrayField(TEXT("messages"), Messages);

    const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutBody);
    return FJsonSerializer::Serialize(Root, Writer);
}

bool ExtractAssistantContent(const FString& ResponseBody, FString& OutContent, FString& OutError)
{
    TSharedPtr<FJsonObject> Root;
    const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseBody);
    if (!FJsonSerializer::Deserialize(Reader, Root) || !Root.IsValid())
    {
        OutError = TEXT("NVIDIA returned non-JSON or malformed JSON.");
        return false;
    }

    const TArray<TSharedPtr<FJsonValue>>* Choices = nullptr;
    if (!Root->TryGetArrayField(TEXT("choices"), Choices) || !Choices || Choices->IsEmpty())
    {
        FString ProviderMessage;
        const TSharedPtr<FJsonObject>* ErrorObject = nullptr;
        if (Root->TryGetObjectField(TEXT("error"), ErrorObject) && ErrorObject && ErrorObject->IsValid())
        {
            (*ErrorObject)->TryGetStringField(TEXT("message"), ProviderMessage);
        }
        OutError = ProviderMessage.IsEmpty()
            ? TEXT("NVIDIA response contains no choices.")
            : FString::Printf(TEXT("NVIDIA request failed: %s"), *ProviderMessage);
        return false;
    }

    const TSharedPtr<FJsonObject> Choice = (*Choices)[0]->AsObject();
    if (!Choice.IsValid())
    {
        OutError = TEXT("NVIDIA response choice is invalid.");
        return false;
    }
    const TSharedPtr<FJsonObject>* Message = nullptr;
    if (!Choice->TryGetObjectField(TEXT("message"), Message) || !Message || !Message->IsValid())
    {
        OutError = TEXT("NVIDIA response has no assistant message.");
        return false;
    }
    if (!(*Message)->TryGetStringField(TEXT("content"), OutContent) || OutContent.TrimStartAndEnd().IsEmpty())
    {
        OutError = TEXT("NVIDIA response assistant message is empty.");
        return false;
    }
    return true;
}
}

int32 StudioKimiCommand(const FParsedArgs& Args)
{
    const FString Root = RepoRoot(Args);
    const FString TaskRelative = Args.Get(TEXT("task-file"));
    const FString RoleRelative = Args.Get(TEXT("role-file"));
    const FString ContextListRelative = Args.Get(TEXT("context-list"));
    const FString OutputRelative = Args.Get(TEXT("output"));

    FString Error;
    FString TaskPath;
    FString RolePath;
    FString OutputPath;
    if (!ResolveRepoFile(Root, TaskRelative, TaskPath, Error)
        || !ResolveRepoFile(Root, RoleRelative, RolePath, Error)
        || !ResolveProposalOutput(Root, OutputRelative, OutputPath, Error))
    {
        UE_LOG(LogTemp, Error, TEXT("studio-kimi: %s"), *Error);
        return 2;
    }

    int32 MaxTokens = DefaultMaxTokens;
    if (!ParseBoundedMaxTokens(Args.Get(TEXT("max-tokens")), MaxTokens, Error))
    {
        UE_LOG(LogTemp, Error, TEXT("studio-kimi: %s"), *Error);
        return 2;
    }

    const FString ApiKey = FPlatformMisc::GetEnvironmentVariable(TEXT("NVIDIA_API_KEY"));
    if (ApiKey.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("studio-kimi: NVIDIA_API_KEY is not present in the process environment."));
        return 3;
    }

    FString TaskText;
    FString RoleText;
    if (!ReadText(TaskPath, TaskText) || !ReadText(RolePath, RoleText))
    {
        UE_LOG(LogTemp, Error, TEXT("studio-kimi: unable to read task or role file."));
        return 2;
    }

    TArray<FString> IncludedContextFiles;
    if (!AppendContextFiles(Root, ContextListRelative, TaskText, IncludedContextFiles, Error))
    {
        UE_LOG(LogTemp, Error, TEXT("studio-kimi: %s"), *Error);
        return 2;
    }

    FString RequestBody;
    if (!SerializeRequestBody(BuildSystemPrompt(RoleText), TaskText, MaxTokens, RequestBody))
    {
        UE_LOG(LogTemp, Error, TEXT("studio-kimi: failed to serialize request."));
        return 2;
    }

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(NvidiaKimiEndpoint);
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetHeader(TEXT("Accept"), TEXT("application/json"));
    Request->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *ApiKey));
    Request->SetContentAsString(RequestBody);

    bool bCompleted = false;
    bool bTransportSucceeded = false;
    int32 ResponseCode = 0;
    FString ResponseBody;
    Request->OnProcessRequestComplete().BindLambda(
        [&](FHttpRequestPtr, FHttpResponsePtr Response, const bool bSucceeded)
        {
            bTransportSucceeded = bSucceeded && Response.IsValid();
            if (Response.IsValid())
            {
                ResponseCode = Response->GetResponseCode();
                ResponseBody = Response->GetContentAsString();
            }
            bCompleted = true;
        });

    if (!Request->ProcessRequest())
    {
        UE_LOG(LogTemp, Error, TEXT("studio-kimi: NVIDIA request could not be started."));
        return 4;
    }

    const double Deadline = FPlatformTime::Seconds() + RequestTimeoutSeconds;
    double LastTick = FPlatformTime::Seconds();
    while (!bCompleted && FPlatformTime::Seconds() < Deadline)
    {
        const double Now = FPlatformTime::Seconds();
        const float DeltaSeconds = static_cast<float>(Now - LastTick);
        LastTick = Now;
        FHttpModule::Get().GetHttpManager().Tick(FMath::Max(DeltaSeconds, 0.001f));
        FPlatformProcess::Sleep(0.01f);
    }

    if (!bCompleted)
    {
        Request->CancelRequest();
        UE_LOG(LogTemp, Error, TEXT("studio-kimi: NVIDIA request timed out."));
        return 4;
    }
    if (!bTransportSucceeded || ResponseCode < 200 || ResponseCode >= 300)
    {
        // Deliberately do not print the full provider body: it is unnecessary for normal diagnostics
        // and avoids accidentally persisting provider-side echoes of supplied context.
        UE_LOG(LogTemp, Error, TEXT("studio-kimi: NVIDIA request failed with HTTP %d."), ResponseCode);
        return 4;
    }

    FString AssistantContent;
    if (!ExtractAssistantContent(ResponseBody, AssistantContent, Error))
    {
        UE_LOG(LogTemp, Error, TEXT("studio-kimi: %s"), *Error);
        return 4;
    }

    FString ContextSummary;
    for (const FString& Relative : IncludedContextFiles)
    {
        ContextSummary += FString::Printf(TEXT("- `%s`\n"), *Relative);
    }
    if (ContextSummary.IsEmpty())
    {
        ContextSummary = TEXT("- No additional context-list files supplied.\n");
    }

    const FString Proposal = FString::Printf(
        TEXT("# Kimi K3 studio proposal — REVIEW ONLY\n\n"
             "> Generated proposal material. This file is **not canon, not runtime evidence, and not merge approval**.\n"
             "> Model: `%s` via NVIDIA NIM. The API credential is not stored in this file.\n\n"
             "## Job metadata\n\n"
             "- Source commit at execution: `%s`\n"
             "- UTC generated: `%s`\n"
             "- Role file: `%s`\n"
             "- Task file: `%s`\n"
             "- Context files:\n%s\n"
             "## Proposal\n\n%s\n"),
        NvidiaKimiModel,
        *GitHead(Root),
        *FDateTime::UtcNow().ToIso8601(),
        *RoleRelative,
        *TaskRelative,
        *ContextSummary,
        *AssistantContent);

    if (!WriteText(OutputPath, Proposal))
    {
        UE_LOG(LogTemp, Error, TEXT("studio-kimi: cannot write proposal output."));
        return 5;
    }

    UE_LOG(LogTemp, Display, TEXT("studio-kimi: proposal written to %s (review-only)."), *OutputRelative);
    return 0;
}
}
