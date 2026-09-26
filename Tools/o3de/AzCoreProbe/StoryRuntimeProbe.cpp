// O3DE 2605.0 story runtime probe: Chapter 3 to the credits.
//
// Loads every level `DarkArisenO3DE materialize` writes from ContentSource/Story (chapters 3-10)
// plus L_Credits through O3DE's JSON serializer and plays them in campaign order on a real
// AZ::ComponentApplication with the Dark Arisen Gem components: activation at the entry, talking
// to contacts, reading evidence, defeating duelists and bosses, sinking hostile ships, the
// authored completion, and the map transition to the next mission's level. Canon, fail-closed
// behaviour, the colonial war, save/load and the credits are checked on the way.
//
// Stand-ins (see LevelProbeSupport.h): transforms, character movement, trigger overlap, the game
// entity context and the level system. Combat outcomes are applied directly (lethal damage, hull
// damage); the fights themselves are covered by the gameplay probe and the Core tests.

#include "LevelProbeSupport.h"
#include "Persistence/SaveSlotStore.h"
#include <AzCore/Time/ITime.h>
#include <AzFramework/Font/FontInterface.h>

#include <DarkArisen/NavalBus.h>

#include <DarkArisen/Core/EntityPolicy.h>
#include <DarkArisen/Core/MissionCatalog.h>
#include <DarkArisen/Core/SaveCodec.h>

#include <algorithm>
#include <fstream>
#include <iterator>
#include <map>
#include <string>
#include <vector>

namespace
{
    using DarkArisen::Core::MissionState;

    void CheckFormat(const bool condition, const std::string& what)
    {
        Check(condition, what.c_str());
    }

    bool Interact(const AZ::EntityId& id)
    {
        bool accepted = false;
        DarkArisen::StoryTriggerRequestBus::EventResult(accepted, id, &DarkArisen::StoryTriggerRequests::Interact,
            AZStd::string(DarkArisen::Core::EntityPolicy::JakeId.data(), DarkArisen::Core::EntityPolicy::JakeId.size()));
        return accepted;
    }

    void Kill(const AZ::EntityId& id)
    {
        if (DarkArisen::Core::Combatant* combatant = CombatantOf(id))
        {
            combatant->Health.ApplyDamage(100000.0f);
        }
    }

    bool HasCombatant(const AZ::EntityId& id)
    {
        bool found = false;
        DarkArisen::CombatRequestBus::Event(id, [&found](DarkArisen::CombatRequests*) { found = true; });
        return found;
    }

    /** Walks Jake to an entity (land stand-in) and ticks so triggers see him arrive. */
    void Approach(LoadedLevel& level, const AZ::EntityId& jake, const AZ::EntityId& target)
    {
        Walk(level, jake, Position(target) + AZ::Vector3(0.0f, 0.0f, 0.05f));
        Tick(level, jake, 1.0f / 30.0f);
    }

    bool TickUntilTravel(LoadedLevel& level, const AZ::EntityId& jake, const float seconds)
    {
        for (int frame = 0; frame < static_cast<int>(seconds * 60.0f) && g_requestedLevel.empty(); ++frame)
        {
            Tick(level, jake, 1.0f / 60.0f);
        }
        return !g_requestedLevel.empty();
    }
}

namespace
{
    /** Stands in for AtomFont: records every string the front end draws. */
    struct CapturedText final : AzFramework::FontDrawInterface
    {
        std::vector<std::string> lines;
        void DrawScreenAlignedText2d(const AzFramework::TextDrawParameters&, AZStd::string_view text) override
        {
            lines.emplace_back(text.data(), text.size());
        }
        void DrawScreenAlignedText3d(const AzFramework::TextDrawParameters&, AZStd::string_view) override {}
        AZ::Vector2 GetTextSize(const AzFramework::TextDrawParameters&, AZStd::string_view) override { return AZ::Vector2::CreateZero(); }
        bool Drew(const std::string& needle) const
        {
            return std::any_of(lines.begin(), lines.end(), [&needle](const std::string& line) { return line.find(needle) != std::string::npos; });
        }
    };

    struct CapturedFonts final : AzFramework::FontQueryInterface
    {
        CapturedText* text = nullptr;
        AzFramework::FontDrawInterface* GetFontDrawInterface(AzFramework::FontId) const override { return text; }
        AzFramework::FontDrawInterface* GetDefaultFontDrawInterface() const override { return text; }
    };
}

int main(int argc, char** argv)
{
    const std::filesystem::path repo = argc > 1 ? std::filesystem::path(argv[1]) : std::filesystem::path(DARKARISEN_REPO_ROOT);
    const std::filesystem::path levels = repo / "Engine/O3DE/DarkArisen/Levels";
    const std::filesystem::path userDir = std::filesystem::temp_directory_path() / "darkarisen_story_probe";
    std::filesystem::remove_all(userDir);
    std::filesystem::create_directories(userDir);

    AZ::ComponentApplication app(argc, argv);
    AZ::ComponentApplication::Descriptor descriptor;
    AZ::ComponentApplication::StartupParameters startup;
    startup.m_loadSettingsRegistry = false;
    startup.m_loadDynamicModules = false;
    startup.m_loadAssetCatalog = false;
    AZ::Entity* systemEntity = app.Create(descriptor, startup);

    AZ::IO::LocalFileIO fileIO;
    AZ::IO::FileIOBase* previousIO = AZ::IO::FileIOBase::GetInstance();
    AZ::IO::FileIOBase::SetInstance(nullptr);
    AZ::IO::FileIOBase::SetInstance(&fileIO);
    fileIO.SetAlias("@user@", userDir.string().c_str());

    for (const AZ::ComponentDescriptor* componentDescriptor :
         {DarkArisen::CampaignSystemComponent::CreateDescriptor(), DarkArisen::CombatantComponent::CreateDescriptor(),
             DarkArisen::EnemyBrainComponent::CreateDescriptor(), DarkArisen::StoryTriggerComponent::CreateDescriptor(),
             DarkArisen::StoryActorComponent::CreateDescriptor(), DarkArisen::MapTransitionComponent::CreateDescriptor(),
             DarkArisen::PlayerSpawnComponent::CreateDescriptor(), DarkArisen::SwimmerComponent::CreateDescriptor(),
             DarkArisen::WaterVolumeComponent::CreateDescriptor(), DarkArisen::OceanComponent::CreateDescriptor(),
             DarkArisen::CameraRigComponent::CreateDescriptor(), DarkArisen::LockOnComponent::CreateDescriptor(),
             DarkArisen::NavalCombatComponent::CreateDescriptor(), DarkArisen::ShipVoyageComponent::CreateDescriptor(),
             DarkArisen::CreditsComponent::CreateDescriptor(), DarkArisen::OpeningDirectorComponent::CreateDescriptor(),
             DarkArisen::FrontEndSystemComponent::CreateDescriptor(), DarkArisen::MainMenuComponent::CreateDescriptor(),
             ProbeSupport::ProbeTransformComponent::CreateDescriptor(), ProbeSupport::ProbeCharacterComponent::CreateDescriptor()})
    {
        app.RegisterComponentDescriptor(componentDescriptor);
    }
    systemEntity->CreateComponent<DarkArisen::CampaignSystemComponent>();
    systemEntity->CreateComponent<DarkArisen::FrontEndSystemComponent>();
    CapturedText drawn;
    CapturedFonts fonts;
    fonts.text = &drawn;
    AZ::Interface<AzFramework::FontQueryInterface>::Register(&fonts);
    systemEntity->Init();
    systemEntity->Activate();
    auto* campaign = DarkArisen::CampaignInterface::Get();
    Check(campaign != nullptr, "campaign system component active");
    if (!campaign)
    {
        return 1;
    }
    ProbeGameEntityContext gameContext;
    AZ::SerializeContext* serialize = app.GetSerializeContext();
    // NewGame and LoadFromSlot replace the runtime: always go through the campaign.
    const auto Runtime = [campaign]() -> DarkArisen::Core::CampaignRuntime& { return campaign->GetCampaign(); };

    // Chapters 1-2 are played by the level probe; here the campaign is advanced to Rexa directly.
    campaign->NewGame();
    for (const char* crew : {"crew.mira", "crew.big_tom", "crew.esteban"})
    {
        const std::string_view role = DarkArisen::Core::OpeningRuntime::OpeningCrewRole(crew);
        Runtime().MeetCrew(crew);
        Runtime().SetCrewAvailable(crew, role);
        Runtime().RecruitCrew(crew, role);
    }
    for (const DarkArisen::Core::MissionDefinition& mission : DarkArisen::Core::MissionCatalog::Missions())
    {
        if (mission.Id == "Main.C03.01.RexaHarbor") break;
        Runtime().ActivateMission(mission.Id);
        Runtime().CompleteMission(mission.Id);
    }
    Check(Runtime().GetMissionState("Main.C03.01.RexaHarbor") == MissionState::Available, "campaign advanced to Rexa Harbor");

    const auto open = [&](const std::string& name, LoadedLevel& level)
    {
        AzFramework::LevelSystemLifecycleNotificationBus::Broadcast(&AzFramework::LevelSystemLifecycleNotifications::OnLoadingStart, name.c_str());
        const bool loaded = LoadPrefab(levels / name / (name + ".prefab"), name.c_str(), serialize, level);
        AzFramework::LevelSystemLifecycleNotificationBus::Broadcast(&AzFramework::LevelSystemLifecycleNotifications::OnLoadingComplete, name.c_str());
        return loaded;
    };

    int levelsPlayed = 0;
    int missionsCompleted = 0;
    int actorsChecked = 0;
    int actorsOnBoxes = 0;
    bool unsupportedActor = false;
    std::string savedBeforeDraven;

    LoadedLevel level;
    for (const DarkArisen::Core::MissionDefinition& mission : DarkArisen::Core::MissionCatalog::Missions())
    {
        const int index = DarkArisen::Core::MissionCatalog::FindIndex(mission.Id);
        if (index < DarkArisen::Core::MissionCatalog::FindIndex("Main.C03.01.RexaHarbor"))
        {
            continue;
        }
        const std::string id(mission.Id);
        const std::string map(DarkArisen::Core::MissionCatalog::LevelFor(mission.Id));
        g_requestedLevel.clear();
        if (!open(map, level) || level.m_rejectedComponents != 0)
        {
            CheckFormat(false, map + ": prefab parsed with every game component deserialised cleanly");
            UnloadLevel(level);
            break;
        }
        ++levelsPlayed;
        const AZ::EntityId jake = level.Id("Jake");

        // Physical standing: every story character and object is on materialised ground.
        for (const GameComponentRecord& record : level.m_records)
        {
            const bool character = record.m_type == "EnemyBrainComponent" ||
                (record.m_type == "StoryActorComponent" && record.Field("Kind") != "0" && record.Field("Kind") != "10");
            if (!character) continue;
            ++actorsChecked;
            const AZ::Vector3 at = Position(record.m_entity);
            if (StandsOnSolidBox(level, at))
            {
                ++actorsOnBoxes;
                continue;
            }
            // Authored mesh geometry (converted glTF with a mesh collider) carries the rest.
            const bool nearMesh = AZStd::any_of(level.m_meshColliderOrigins.begin(), level.m_meshColliderOrigins.end(),
                [&at](const AZ::Vector3& origin) { return origin.GetDistance(at) < 40.0f; });
            if (!nearMesh)
            {
                unsupportedActor = true;
                std::printf("  unsupported: %s in %s at (%.1f, %.1f, %.1f)\n", record.m_entityName.c_str(), map.c_str(), at.GetX(),
                    at.GetY(), at.GetZ());
            }
        }

        // Entry: the activation trigger sits on the arrival spawn.
        Tick(level, jake, 0.1f);
        CheckFormat(Runtime().GetMissionState(mission.Id) == MissionState::Active, id + ": activated at its entry in " + map);

        // Collect this level's story actors by kind (Core::StoryActorKind ordinals).
        AZStd::vector<const GameComponentRecord*> contacts, evidence, completingEvidence, routes, wars, gates, locationsToComplete;
        AZStd::vector<AZ::EntityId> duelists, bosses, hostileShips;
        AZ::EntityId playerShip;
        for (const GameComponentRecord& record : level.m_records)
        {
            if (record.m_type == "StoryActorComponent")
            {
                const AZStd::string kind = record.Field("Kind");
                if (kind == "0" && record.Field("Action") == "1") locationsToComplete.push_back(&record);
                else if (kind == "1") contacts.push_back(&record);
                else if (kind == "2") (record.Field("CompletesMission") == "true" ? completingEvidence : evidence).push_back(&record);
                else if (kind == "3") routes.push_back(&record);
                else if (kind == "4") wars.push_back(&record);
                else if (kind == "10") gates.push_back(&record);
            }
            else if (record.m_type == "EnemyBrainComponent")
            {
                (record.Field("Profile") == "0" ? duelists : bosses).push_back(record.m_entity);
            }
            else if (record.m_type == "NavalCombatComponent")
            {
                if (record.Field("PlayerAligned") == "true") playerShip = record.m_entity;
                else hostileShips.push_back(record.m_entity);
            }
        }

        // Real Ethan is never a combatant; the one Ethan-shaped enemy is the Dream figure.
        for (const char* name : {"C08_02_EthanPhysical", "C08_03_EthanPhysical"})
        {
            if (level.Id(name).IsValid())
            {
                CheckFormat(!HasCombatant(level.Id(name)), id + ": real Ethan is present and not a combatant");
            }
        }

        for (const GameComponentRecord* contact : contacts)
        {
            Approach(level, jake, contact->m_entity);
            CheckFormat(Interact(contact->m_entity), id + ": Jake talks to " + std::string(contact->m_entityName.c_str()));
        }
        for (const GameComponentRecord* document : evidence)
        {
            Approach(level, jake, document->m_entity);
            Interact(document->m_entity);
        }

        // Authored fights cannot be bypassed: completion refused while duelists live.
        if (!duelists.empty() && !locationsToComplete.empty())
        {
            Approach(level, jake, locationsToComplete.front()->m_entity);
            CheckFormat(Runtime().GetMissionState(mission.Id) == MissionState::Active,
                id + ": completion refused while " + std::to_string(duelists.size()) + " duelists live");
        }
        for (const AZ::EntityId& duelist : duelists) Kill(duelist);

        // Herrera-style gating: the chart stays locked until the Holder falls.
        for (const GameComponentRecord* document : completingEvidence)
        {
            if (!document->Field("RequiredBossId").empty())
            {
                Approach(level, jake, document->m_entity);
                CheckFormat(!Interact(document->m_entity), id + ": " + std::string(document->m_entityName.c_str()) + " locked before its boss");
            }
        }

        if (id == "Main.C10.04.DravenVoss")
        {
            DarkArisen::CampaignRequestBus::Broadcast(&DarkArisen::CampaignRequests::SaveToSlot, AZStd::string("ProbeBeforeDraven"));
            savedBeforeDraven = DarkArisen::Core::SaveCodec::Encode(Runtime().State());
        }
        for (const AZ::EntityId& boss : bosses) Kill(boss);
        Tick(level, jake, 0.1f);

        // Naval screen: the hostiles go down, then La Liberacion sails to the gate.
        for (const AZ::EntityId& ship : hostileShips)
        {
            DarkArisen::NavalCombatRequestBus::Event(ship, &DarkArisen::NavalCombatRequests::ApplyHullDamage, 100000.0f);
        }
        for (const GameComponentRecord* gate : gates)
        {
            if (playerShip.IsValid())
            {
                AZ::TransformBus::Event(playerShip, &AZ::TransformBus::Events::SetWorldTranslation, Position(gate->m_entity));
            }
            Tick(level, jake, 0.5f);
        }

        // Authored completion.
        for (const GameComponentRecord* document : completingEvidence)
        {
            if (Runtime().GetMissionState(mission.Id) != MissionState::Active) break;
            Approach(level, jake, document->m_entity);
            Interact(document->m_entity);
        }
        if (!wars.empty() && Runtime().GetMissionState(mission.Id) == MissionState::Active)
        {
            // Break the imperial supply where the choice exists (the war must move); else the holding claim.
            const auto chosen = AZStd::find_if(wars.begin(), wars.end(), [](const GameComponentRecord* war) { return war->Field("Verb") == "1"; });
            const GameComponentRecord* war = chosen != wars.end() ? *chosen : wars.front();
            Approach(level, jake, war->m_entity);
            Interact(war->m_entity);
        }
        if (!routes.empty() && Runtime().GetMissionState(mission.Id) == MissionState::Active)
        {
            Approach(level, jake, routes.front()->m_entity);
            Interact(routes.front()->m_entity);
        }
        for (const GameComponentRecord* location : locationsToComplete)
        {
            if (Runtime().GetMissionState(mission.Id) != MissionState::Active) break;
            Approach(level, jake, location->m_entity);
        }
        Tick(level, jake, 0.1f);
        const bool completed = Runtime().GetMissionState(mission.Id) == MissionState::Completed;
        missionsCompleted += completed ? 1 : 0;
        CheckFormat(completed, id + ": completed through its authored physical completion");

        const bool final = id == "Main.C10.05.TheWakeAfter";
        const std::string next = final ? "L_Credits" : std::string(DarkArisen::Core::MissionCatalog::LevelFor(
                                                            DarkArisen::Core::MissionCatalog::Missions()[static_cast<size_t>(index + 1)].Id));
        TickUntilTravel(level, jake, 6.0f);
        const bool travelled = g_requestedLevel == next.c_str();
        CheckFormat(travelled, id + ": map transition to " + next);

        // Mission-specific consequences.
        if (id == "Main.C04.02.BrokenCompact")
        {
            const auto& war = Runtime().GetColonialWar();
            Check(war.Regions.count("Rexa") == 1 && war.Regions.at("Rexa").ImperialControl == 52 &&
                    Runtime().State().QuestOutcomes.count("WarChoice.Main.C04.02.BrokenCompact") == 1,
                "Broken Compact: breaking the imperial supply moved the Rexa war and recorded the choice");
        }
        if (id == "Main.C06.01.HighmooreRoad")
        {
            Check(Runtime().GetColonialWar().OwnedHoldings.count("Holding.C06.First") == 1 && Runtime().HasFact("Holding.FirstOwned"),
                "Highmoore Road: Jake holds the first holding (Unreal could not complete this mission)");
        }
        if (id == "Main.C08.03.HomewardBearing")
        {
            Check(Runtime().HasFact("Story.EthanRecovered") && Runtime().HasFact("Story.EthanNoLongerCaptive"),
                "Chapter 8: Ethan physically rescued and aboard La Liberacion");
        }
        if (id == "Main.C09.03.DreamFight")
        {
            Check(Runtime().HasBossDefeated("boss.dream_ethan") && Runtime().HasFact("Story.DreamResolved") &&
                    !Runtime().HasBossDefeated("character.ethan") && Runtime().HasFact("Story.EthanRecovered"),
                "Dream Fight: the dream figure is resolved; real Ethan untouched and still recovered");
        }
        if (id == "Main.C10.04.DravenVoss")
        {
            Check(Runtime().HasFact("Story.DravenDefeated") && Runtime().State().QuestOutcomes.at("Draven.Outcome") == "Defeated",
                "Draven Voss defeated on the Black Deck");
            // Load the save from before the duel: Draven stands again, then win once more.
            bool loaded = false;
            DarkArisen::CampaignRequestBus::BroadcastResult(loaded, &DarkArisen::CampaignRequests::LoadFromSlot, AZStd::string("ProbeBeforeDraven"));
            Check(loaded && DarkArisen::Core::SaveCodec::Encode(Runtime().State()) == savedBeforeDraven && !Runtime().HasFact("Story.DravenDefeated"),
                "save before Draven reloads exactly (checksummed slot through LocalFileIO)");
            Runtime().ActivateMission(mission.Id);
            DarkArisen::Core::ResolveBossDefeat(Runtime(), DarkArisen::Core::EnemyProfile::DravenVoss());
            Check(Runtime().GetMissionState(mission.Id) == MissionState::Completed, "Draven defeated again after the reload");
        }
        UnloadLevel(level);
        if (!completed || !travelled)
        {
            break;
        }
    }

    std::printf("  %d levels played, %d/27 story missions completed, %d actors checked (%d on materialised boxes)\n", levelsPlayed,
        missionsCompleted, actorsChecked, actorsOnBoxes);
    Check(levelsPlayed == 27 && missionsCompleted == 27, "chapters 3-10 played from Rexa Harbor to The Wake After");
    Check(!unsupportedActor, "every story character and object stands on materialised ground");
    Check(Runtime().HasFact("Story.MainComplete") && Runtime().HasFact("Story.CreditsReachable"), "Story.MainComplete only after the final mission");
    {
        const auto& evidence = Runtime().GetJournal().Evidence;
        DarkArisen::Core::CampaignState reloaded;
        std::vector<std::string> errors;
        const bool decoded = DarkArisen::Core::SaveCodec::Decode(DarkArisen::Core::SaveCodec::Encode(Runtime().State()), reloaded, errors);
        std::printf("  %zu documents in Jake's notebook\n", evidence.size());
        Check(!evidence.empty() && decoded && reloaded.Journal == Runtime().GetJournal(), "documents Jake read are kept in the saved notebook");
    }

    // ---- L_Credits.
    g_requestedLevel.clear();
    Check(open("L_Credits", level) && level.m_rejectedComponents == 0, "L_Credits.prefab parsed");
    bool rolling = false;
    if (AZ::Entity* credits = ProbeGameEntityContext::Find(level.Id("Credits Roll")))
    {
        const auto* component = credits->FindComponent<DarkArisen::CreditsComponent>();
        rolling = component && component->HasStarted();
    }
    Check(rolling, "credits roll after The Wake After");
    const GameComponentRecord* roll = nullptr;
    for (const GameComponentRecord& record : level.m_records)
    {
        if (record.m_type == "CreditsComponent") roll = &record;
    }
    Check(roll && roll->Field("RollText").find("O3DE 2605.0") != AZStd::string::npos &&
            roll->Field("RollText").find("Unreal") == AZStd::string::npos,
        "credits list the verified O3DE technology, not Unreal");

    // ---- The front end: credits roll on screen, return to L_FrontEnd, main menu, settings, pause.
    DarkArisen::FrontEndRequests* frontEnd = DarkArisen::FrontEndRequestBus::FindFirstHandler();
    Check(frontEnd && frontEnd->IsCreditsRolling(), "the front end rolls the credits it was handed");
    if (!frontEnd)
    {
        return 1;
    }
    drawn.lines.clear();
    Tick(level, AZ::EntityId(), 1.0f);
    Check(drawn.Drew("O3DE 2605.0") && drawn.Drew("Thank you for playing."), "the roll is drawn through AzFramework::FontDrawInterface");
    frontEnd->Back();
    Check(frontEnd->IsCreditsRolling(), "the roll cannot be skipped in its first 8 seconds");
    Tick(level, AZ::EntityId(), 8.0f);
    frontEnd->Back();
    Tick(level, AZ::EntityId(), 0.1f);
    Check(!frontEnd->IsCreditsRolling() && g_requestedLevel == "L_FrontEnd", "skipped after 8 s: the game returns to the front end");
    UnloadLevel(level);

    g_requestedLevel.clear();
    Check(open("L_FrontEnd", level) && level.m_rejectedComponents == 0, "L_FrontEnd.prefab parsed");
    const auto focusOn = [&](const std::string& label)
    {
        for (int tries = 0; tries < 16; ++tries)
        {
            drawn.lines.clear();
            Tick(level, AZ::EntityId(), 1.0f / 60.0f);
            if (drawn.Drew("> " + label)) return true;
            frontEnd->Navigate(1);
        }
        return false;
    };
    drawn.lines.clear();
    Tick(level, AZ::EntityId(), 0.1f);
    Check(frontEnd->IsMenuOpen() && drawn.Drew("DARK ARISEN") && drawn.Drew("NEW GAME") && drawn.Drew("CONTINUE") && drawn.Drew("QUIT"),
        "L_FrontEnd shows the main menu");
    Check(DarkArisen::SaveSlotStore::Exists(DarkArisen::SaveSlotStore::AutosaveSlot), "Continue is offered: the chapter autosave exists");
    Check(focusOn("OPTIONS / SETTINGS"), "menu focus moves with navigation");
    frontEnd->Confirm();
    Check(focusOn("GRAPHICS EPIC") && drawn.Drew("RAY TRACING: NOT MEASURED ON THIS MACHINE"),
        "settings page; ray tracing stays unavailable without a measurement on this machine");
    frontEnd->Confirm();
    std::ifstream settingsFile(userDir / "DarkArisen" / "settings.cfg");
    const std::string settingsText((std::istreambuf_iterator<char>(settingsFile)), std::istreambuf_iterator<char>());
    Check(frontEnd->GetUserSettings().Preset == DarkArisen::Core::GraphicsPreset::Epic && settingsText.find("preset=epic") != std::string::npos,
        "graphics preset applied and saved under @user@/DarkArisen/settings.cfg");
    frontEnd->Back();
    Check(focusOn("NEW GAME"), "back on the main page");
    frontEnd->Confirm();
    Check(!frontEnd->IsMenuOpen() && g_requestedLevel == "L_HarlowOpening" &&
            Runtime().GetMissionState("Main.C10.05.TheWakeAfter") != MissionState::Completed && Runtime().State().CurrentChapter == 1,
        "New Game resets the campaign and opens the Harlow opening");
    UnloadLevel(level);

    g_requestedLevel.clear();
    auto* time = AZ::Interface<AZ::ITime>::Get();
    frontEnd->Back();
    Check(frontEnd->IsPaused() && time && time->GetSimulationTickScale() == 0.0f, "Escape pauses play (simulation tick scale 0)");
    Check(focusOn("SAVE GAME") && drawn.Drew("PAUSED"), "the pause menu is drawn");
    frontEnd->Confirm();
    drawn.lines.clear();
    Tick(level, AZ::EntityId(), 1.0f / 60.0f);
    Check(DarkArisen::SaveSlotStore::Exists(DarkArisen::SaveSlotStore::ManualSlot) && drawn.Drew("Game saved."), "Save Game writes the manual slot");
    frontEnd->Back();
    Check(!frontEnd->IsPaused() && time && time->GetSimulationTickScale() == 1.0f, "Escape again resumes at the previous time scale");

    systemEntity->Deactivate();
    AZ::Interface<AzFramework::FontQueryInterface>::Unregister(&fonts);
    AZ::IO::FileIOBase::SetInstance(nullptr);
    AZ::IO::FileIOBase::SetInstance(previousIO);
    app.Destroy();
    std::filesystem::remove_all(userDir);
    std::printf("%s (%d failed)\n", g_failures == 0 ? "O3DE story runtime probe PASSED" : "O3DE story runtime probe FAILED", g_failures);
    return g_failures == 0 ? 0 : 1;
}
