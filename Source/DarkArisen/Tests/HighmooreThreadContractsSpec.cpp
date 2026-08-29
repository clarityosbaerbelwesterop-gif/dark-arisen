// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Highmoore/HighmooreReconstructionComponent.h"
#include "Highmoore/LightElvesThreadComponent.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenLightElvesThreadContractSpec,
    "DarkArisen.Content.Threads.Highmoore.LightElves.Contracts",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenLightElvesThreadContractSpec::RunTest(const FString& Parameters)
{
    ULightElvesThreadComponent* Thread = NewObject<ULightElvesThreadComponent>();
    TestNotNull(TEXT("Light Elves fixture exists"), Thread);
    if (!Thread)
    {
        return false;
    }

    TestFalse(TEXT("Ten days and one minute is still short of the authored eleven-day span"),
        ULightElvesThreadComponent::HasElevenDaySpanElapsed(100, 100 + (10 * 24 * 60) + 1));
    TestTrue(TEXT("Exactly eleven world days satisfies the span"),
        ULightElvesThreadComponent::HasElevenDaySpanElapsed(
            100,
            100 + ULightElvesThreadComponent::ElevenDaysInWorldMinutes));
    TestFalse(TEXT("A reversed world clock never satisfies the span"),
        ULightElvesThreadComponent::HasElevenDaySpanElapsed(100, 99));

    // BeginAtDroversRest binds to the real world clock. This source-only automation fixture starts
    // immediately after that authored recruitment so the finite stage graph can be checked without
    // pretending an editor/runtime world has been exercised.
    Thread->Stage = ELightElvesThreadStage::RecruitedAtDroversRest;
    TestTrue(TEXT("Cadwall refusal records"), Thread->RecordLordRefusal(ELightElvesLordRefusal::Cadwall));
    TestTrue(TEXT("Mere refusal records"), Thread->RecordLordRefusal(ELightElvesLordRefusal::Mere));
    TestTrue(TEXT("Thorn refusal records"), Thread->RecordLordRefusal(ELightElvesLordRefusal::Thorn));
    TestTrue(TEXT("Ashby refusal records"), Thread->RecordLordRefusal(ELightElvesLordRefusal::Ashby));
    TestEqual(TEXT("All four authored refusals advance the Thread"),
        Thread->Stage, ELightElvesThreadStage::LordsRefused);
    TestFalse(TEXT("A duplicate refusal cannot pad the count"),
        Thread->RecordLordRefusal(ELightElvesLordRefusal::Ashby));

    TestTrue(TEXT("First taking account records"), Thread->RecordTakingAccount(TEXT("taking.village.01")));
    TestTrue(TEXT("Second taking account records"), Thread->RecordTakingAccount(TEXT("taking.village.02")));
    TestTrue(TEXT("Third taking account records"), Thread->RecordTakingAccount(TEXT("taking.village.03")));
    TestTrue(TEXT("Fourth taking account records"), Thread->RecordTakingAccount(TEXT("taking.village.04")));
    TestFalse(TEXT("A fifth fabricated village account is rejected"),
        Thread->RecordTakingAccount(TEXT("taking.village.05")));
    TestTrue(TEXT("Abbey records close the taking investigation only after four accounts"),
        Thread->RecordAbbeyRecordsCounted());
    TestTrue(TEXT("Returned man stage records"), Thread->RecordReturnedManHeard());
    TestTrue(TEXT("Practices stage records"), Thread->RecordPracticesLearned());

    TestTrue(TEXT("Standing Stones resolve"), Thread->RecordMoorSiteResolved(ELightElvesMoorSite::StandingStones));
    TestTrue(TEXT("Barrow resolves"), Thread->RecordMoorSiteResolved(ELightElvesMoorSite::Barrow));
    TestTrue(TEXT("Barren ground resolves"), Thread->RecordMoorSiteResolved(ELightElvesMoorSite::BarrenGround));
    TestEqual(TEXT("Three source-authored sites reach the shaft gate"),
        Thread->Stage, ELightElvesThreadStage::ThreePlacesResolved);

    TestFalse(TEXT("The first stopped shaft sight remains playable"), Thread->AllowsFirstStoppedShaftCutscene());
    TestFalse(TEXT("The Light Elf ending remains playable"), Thread->AllowsBossEndingCutscene());
    TestFalse(TEXT("The campaign cannot create a westward reminder"), Thread->AllowsWestwardQuestReminder());
    TestFalse(TEXT("The campaign cannot create fast travel"), Thread->AllowsFastTravelForCampaign());

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenHighmooreReconstructionContractSpec,
    "DarkArisen.Content.Threads.Highmoore.Reconstruction.Contracts",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenHighmooreReconstructionContractSpec::RunTest(const FString& Parameters)
{
    TestEqual(TEXT("Katana Belos outcome opens standard reconstruction"),
        UHighmooreReconstructionComponent::ResolvePathFromPrincessFacts(
            EBelosPathResolution::KatanaAssaultInterrupted, false),
        EHighmooreReconstructionPath::StandardAfterBelos);
    TestEqual(TEXT("Ordinary-weapon Belos outcome also opens standard reconstruction"),
        UHighmooreReconstructionComponent::ResolvePathFromPrincessFacts(
            EBelosPathResolution::OrdinaryWeaponOverwhelmed, false),
        EHighmooreReconstructionPath::StandardAfterBelos);
    TestEqual(TEXT("Turning west is a distinct diplomatic path, not invented construction content"),
        UHighmooreReconstructionComponent::ResolvePathFromPrincessFacts(
            EBelosPathResolution::TurnedAway, false),
        EHighmooreReconstructionPath::TurnedWest);
    TestEqual(TEXT("Going to the undercroft stair permanently blocks reconstruction"),
        UHighmooreReconstructionComponent::ResolvePathFromPrincessFacts(
            EBelosPathResolution::KatanaAssaultInterrupted, true),
        EHighmooreReconstructionPath::PermanentlyUnavailableAfterStair);

    UHighmooreReconstructionComponent* Reconstruction = NewObject<UHighmooreReconstructionComponent>();
    TestNotNull(TEXT("Reconstruction fixture exists"), Reconstruction);
    if (!Reconstruction)
    {
        return false;
    }

    TestFalse(TEXT("There is no redemption meter"), Reconstruction->AllowsRedemptionMeter());
    TestFalse(TEXT("There is no forgiveness reward"), Reconstruction->AllowsForgivenessReward());
    TestFalse(TEXT("Jake never becomes Lord of Highmoore through reconstruction"),
        Reconstruction->MakesJakeLordOfHighmoore());
    TestFalse(TEXT("Filling the gallery gap carries no reward"), Reconstruction->RewardsGalleryGap());
    TestFalse(TEXT("Filling the rabbit hutches carries no reward"), Reconstruction->RewardsRabbitHutches());

    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
