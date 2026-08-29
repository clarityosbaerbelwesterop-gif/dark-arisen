// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LightElvesThreadComponent.generated.h"

UENUM(BlueprintType)
enum class ELightElvesThreadStage : uint8
{
    Dormant,
    RecruitedAtDroversRest,
    LordsRefused,
    TakingsInvestigated,
    ReturnedManHeard,
    PracticesLearned,
    ThreePlacesResolved,
    ShaftBossResolved,
    LetterHandedOver,
    Complete
};

UENUM(BlueprintType)
enum class ELightElvesLordRefusal : uint8
{
    Cadwall,
    Mere,
    Thorn,
    Ashby
};

UENUM(BlueprintType)
enum class ELightElvesMoorSite : uint8
{
    StandingStones,
    Barrow,
    BarrenGround
};

UENUM(BlueprintType)
enum class EEllisLetterResponse : uint8
{
    None,
    SayNothing,
    AskWhy,
    StrikeEllis
};

/**
 * Runtime owner for `thread.highmoore.light-elves` / the Eleven Days.
 * All state corresponds to explicit `wizard questline.md` stages. No marker, fast-travel,
 * westward reminder, adaptive music or invented taking-rate system exists here.
 */
UCLASS(ClassGroup=(DarkArisen), meta=(BlueprintSpawnableComponent))
class DARKARISEN_API ULightElvesThreadComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    ULightElvesThreadComponent();

    static const FName ThreadId;
    static constexpr int64 ElevenDaysInWorldMinutes = 11LL * 24LL * 60LL;

    /** External proof is the authored Arion ejection; the current Princess owner begins later. */
    UFUNCTION(BlueprintCallable, Category="Highmoore|Light Elves")
    bool BeginAtDroversRest(bool bArionEjectionCompleted);

    UFUNCTION(BlueprintCallable, Category="Highmoore|Light Elves")
    bool RecordLordRefusal(ELightElvesLordRefusal Lord);

    UFUNCTION(BlueprintCallable, Category="Highmoore|Light Elves")
    bool RecordTakingAccount(FName VillageAccountId);

    UFUNCTION(BlueprintCallable, Category="Highmoore|Light Elves")
    bool RecordAbbeyRecordsCounted();

    UFUNCTION(BlueprintCallable, Category="Highmoore|Light Elves")
    bool RecordReturnedManHeard();

    UFUNCTION(BlueprintCallable, Category="Highmoore|Light Elves")
    bool RecordPracticesLearned();

    UFUNCTION(BlueprintCallable, Category="Highmoore|Light Elves")
    bool RecordMoorSiteResolved(ELightElvesMoorSite Site);

    /** The boss resolution remains a separate encounter/asset gate; this records its authored outcome. */
    UFUNCTION(BlueprintCallable, Category="Highmoore|Light Elves")
    bool RecordShaftBossResolved();

    UFUNCTION(BlueprintCallable, Category="Highmoore|Light Elves")
    bool RecordLetterHandedOver(EEllisLetterResponse Response);

    /** Threat never fully expires; an authored world event may report another taking. */
    UFUNCTION(BlueprintCallable, Category="Highmoore|Light Elves")
    bool RecordAdditionalTaking();

    UFUNCTION(BlueprintPure, Category="Highmoore|Light Elves")
    int64 GetElapsedWorldMinutes() const;

    UFUNCTION(BlueprintPure, Category="Highmoore|Light Elves")
    bool HasReachedElevenDays() const;

    UFUNCTION(BlueprintPure, Category="Highmoore|Light Elves|Presentation")
    bool AllowsFirstStoppedShaftCutscene() const { return false; }

    UFUNCTION(BlueprintPure, Category="Highmoore|Light Elves|Presentation")
    bool AllowsBossEndingCutscene() const { return false; }

    UFUNCTION(BlueprintPure, Category="Highmoore|Light Elves|Presentation")
    bool AllowsWestwardQuestReminder() const { return false; }

    UFUNCTION(BlueprintPure, Category="Highmoore|Light Elves|Presentation")
    bool AllowsFastTravelForCampaign() const { return false; }

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame, Category="Highmoore|Light Elves")
    ELightElvesThreadStage Stage = ELightElvesThreadStage::Dormant;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame, Category="Highmoore|Light Elves")
    EEllisLetterResponse LetterResponse = EEllisLetterResponse::None;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame, Category="Highmoore|Light Elves")
    int32 AdditionalTakingCount = 0;

private:
    UPROPERTY(SaveGame)
    TSet<ELightElvesLordRefusal> LordRefusals;

    UPROPERTY(SaveGame)
    TSet<FName> TakingAccounts;

    UPROPERTY(SaveGame)
    TSet<ELightElvesMoorSite> ResolvedMoorSites;

    UPROPERTY(SaveGame)
    int64 CampaignStartWorldMinutes = INDEX_NONE;

    bool IsAtLeast(ELightElvesThreadStage Required) const;
    int64 ReadWorldMinutes() const;
};
