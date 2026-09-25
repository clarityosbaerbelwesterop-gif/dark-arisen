// Runs the Dark Arisen campaign system component inside a real O3DE 2605.0
// AZ::ComponentApplication (AzCore built from the pinned sources), with real
// EBus notifications and real file IO through AzFramework::LocalFileIO.
// It proves the adapter layer executes on O3DE's framework; it does not replace
// Editor/GameLauncher verification on a full engine build.

#include <AzCore/Component/ComponentApplication.h>
#include <AzCore/Component/Entity.h>
#include <AzCore/IO/FileIO.h>
#include <AzFramework/API/ApplicationAPI.h>
#include <AzFramework/IO/LocalFileIO.h>

#include <DarkArisen/CampaignBus.h>
#include "Story/CampaignSystemComponent.h"

#include <DarkArisen/Core/CampaignRuntime.h>
#include <DarkArisen/Core/Facts.h>
#include <DarkArisen/Core/OpeningRuntime.h>

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace
{
    int g_failures = 0;

    void Check(const bool condition, const char* what)
    {
        std::printf("[%s] %s\n", condition ? " ok " : "FAIL", what);
        g_failures += condition ? 0 : 1;
    }

    /** Stands in for Jake's CombatantComponent: records notifications and pushes a player snapshot. */
    struct ProbeListener : DarkArisen::CampaignNotificationBus::Handler
    {
        std::vector<std::string> missionEvents;
        std::vector<int> chapterBoundaries;
        std::vector<std::string> cinematics;
        int captures = 0;
        int restores = 0;
        AZStd::string lastRestoreLevel;
        bool sawChapterFactBeforeListener = false;

        void OnMissionStateChanged(const AZStd::string& missionId, const int state) override
        {
            missionEvents.emplace_back(std::string(missionId.c_str()) + ":" + std::to_string(state));
            if (missionId == "Main.C01.04.Undertow" && state == static_cast<int>(DarkArisen::Core::MissionState::Completed))
            {
                sawChapterFactBeforeListener = DarkArisen::CampaignInterface::Get()->HasFact("Chapter.01.Complete");
            }
        }
        void OnChapterBoundary(const int chapter) override { chapterBoundaries.push_back(chapter); }
        void OnCinematicRequested(const AZStd::string& id) override { cinematics.emplace_back(id.c_str()); }
        void OnCaptureWorldState() override
        {
            ++captures;
            auto* campaign = DarkArisen::CampaignInterface::Get();
            DarkArisen::Core::PlayerRuntimeSnapshot player;
            player.Valid = true;
            player.SourceLevel = campaign->GetCurrentLevelName().c_str();
            player.WorldTransform.Translation = {12.5, -3.0, 1.0};
            player.HealthFraction = 0.75f;
            player.StaminaFraction = 0.5f;
            campaign->GetCampaign().CapturePlayerRuntime(player);
        }
        void OnRestoreWorldState(const AZStd::string& level) override
        {
            ++restores;
            lastRestoreLevel = level;
        }
    };
}

int main(int argc, char** argv)
{
    const std::filesystem::path userDir = std::filesystem::temp_directory_path() / "darkarisen-o3de-probe";
    std::filesystem::remove_all(userDir);
    std::filesystem::create_directories(userDir);

    AZ::ComponentApplication app(argc, argv);
    AZ::ComponentApplication::Descriptor descriptor;
    AZ::ComponentApplication::StartupParameters startup;
    startup.m_loadSettingsRegistry = false;
    startup.m_loadStaticModules = false;
    startup.m_loadDynamicModules = false;
    startup.m_loadAssetCatalog = false;
    AZ::Entity* systemEntity = app.Create(descriptor, startup);
    Check(systemEntity != nullptr, "O3DE ComponentApplication created");

    AZ::IO::LocalFileIO fileIO;
    AZ::IO::FileIOBase* previousIO = AZ::IO::FileIOBase::GetInstance();
    AZ::IO::FileIOBase::SetInstance(nullptr);
    AZ::IO::FileIOBase::SetInstance(&fileIO);
    fileIO.SetAlias("@user@", userDir.string().c_str());

    app.RegisterComponentDescriptor(DarkArisen::CampaignSystemComponent::CreateDescriptor());
    systemEntity->CreateComponent<DarkArisen::CampaignSystemComponent>();
    systemEntity->Init();
    systemEntity->Activate();
    Check(systemEntity->GetState() == AZ::Entity::State::Active, "system entity with CampaignSystemComponent active");

    auto* campaign = DarkArisen::CampaignInterface::Get();
    Check(campaign != nullptr, "CampaignInterface registered through AZ::Interface");
    if (!campaign)
    {
        return 1;
    }

    ProbeListener listener;
    listener.BusConnect();

    // Level lifecycle as the O3DE level system would report it.
    AzFramework::LevelSystemLifecycleNotificationBus::Broadcast(
        &AzFramework::LevelSystemLifecycleNotifications::OnLoadingStart, "Levels/HarlowMerchantShip");
    Check(campaign->GetCurrentLevelName() == "Levels/HarlowMerchantShip", "level name known before entity activation");

    // Chapter 1 through the engine-facing opening runtime.
    DarkArisen::Core::OpeningRuntime& opening = campaign->GetOpening();
    bool raid = true;
    for (const char* member : {"character.marc", "character.denise", "character.ethan"})
    {
        raid = raid && opening.SignalFamilyInteraction(member);
    }
    raid = raid && opening.SignalFleetDetected() && opening.SignalBoardingStarted() &&
        opening.BeginBoardingEncounter("Encounter.HarlowDeck", 2) && opening.SignalBoarderDefeated("Encounter.HarlowDeck") &&
        opening.SignalBoarderDefeated("Encounter.HarlowDeck") && opening.SignalDravenBoarded("Encounter.HarlowDeck") &&
        opening.SignalTakingStarted() && opening.SignalTakingCinematicComplete();
    Check(raid, "Harlow raid beats accepted in order");
    Check(listener.cinematics.size() == 4, "4 cinematics requested over CampaignNotificationBus");
    Check(campaign->HasFact(DarkArisen::Core::Facts::EthanAbducted.data()) &&
              !campaign->HasFact(DarkArisen::Core::Facts::EthanRecovered.data()),
        "Ethan abducted alive, not recovered");

    const bool shore = opening.SignalEnteredWater() && opening.SignalReachedOuterReef() &&
        opening.SignalReachedLocation(DarkArisen::Core::OpeningLocation::DriftwoodBeach) && opening.SignalRecoveryComplete() &&
        opening.SignalReachedLocation(DarkArisen::Core::OpeningLocation::DriftwoodCamp);
    Check(shore, "open water -> Outer Reef -> Driftwood Beach -> Driftwood Camp");
    Check(listener.chapterBoundaries.size() == 1 && listener.chapterBoundaries.front() == 2, "chapter boundary broadcast once");
    Check(listener.sawChapterFactBeforeListener, "authored facts applied before completion listeners");
    Check(listener.captures >= 1, "autosave captured live world state first");

    const std::filesystem::path autosave = userDir / "Saves" / "Autosave.dasave";
    Check(std::filesystem::exists(autosave), "chapter-boundary autosave written through O3DE FileIO");
    Check(!std::filesystem::exists(autosave.string() + ".tmp"), "atomic write left no temporary file");

    // Load the autosave into a fresh session.
    const std::string missionBefore = campaign->GetCurrentMission().c_str();
    campaign->NewGame();
    Check(campaign->GetCurrentMission() == "Main.C01.01.HomeWater", "NewGame resets the campaign");
    Check(campaign->LoadFromSlot("Autosave"), "autosave loads, migrates and validates");
    Check(std::string(campaign->GetCurrentMission().c_str()) == missionBefore, "loaded mission matches the saved one");
    Check(campaign->GetOpening().Progress().Location == DarkArisen::Core::OpeningLocation::DriftwoodBeach ||
              campaign->GetOpening().Progress().Location == DarkArisen::Core::OpeningLocation::DriftwoodCamp,
        "opening progress restored from save");
    Check(listener.restores >= 1 && listener.lastRestoreLevel == "Levels/HarlowMerchantShip",
        "restore broadcast for the current level");

    // Corrupt save: rejected, running campaign untouched.
    {
        std::fstream stream(autosave, std::ios::in | std::ios::out | std::ios::binary);
        stream.seekp(80);
        stream.put('#');
    }
    const std::string missionBeforeCorrupt = campaign->GetCurrentMission().c_str();
    AZ::Debug::Trace::Instance().Printf("probe", "Expecting one 'Save rejected' error next.\n");
    Check(!campaign->LoadFromSlot("Autosave"), "corrupt save rejected");
    Check(std::string(campaign->GetCurrentMission().c_str()) == missionBeforeCorrupt, "rejected load left state untouched");
    Check(!campaign->LoadFromSlot("../escape"), "path-traversal slot name rejected");

    // Manual save and suppression.
    Check(campaign->SaveToSlot("Manual01"), "manual save succeeds");
    campaign->SetAutosaveSuppressed(true);
    std::filesystem::remove(autosave);
    Check(!campaign->CompleteRest() && !std::filesystem::exists(autosave), "suppressed window blocks the rest autosave");

    listener.BusDisconnect();
    systemEntity->Deactivate();
    AZ::IO::FileIOBase::SetInstance(nullptr);
    AZ::IO::FileIOBase::SetInstance(previousIO);
    app.Destroy();
    std::filesystem::remove_all(userDir);

    std::printf("%s (%d failed)\n", g_failures == 0 ? "O3DE AzCore runtime probe PASSED" : "O3DE AzCore runtime probe FAILED",
        g_failures);
    return g_failures == 0 ? 0 : 1;
}
