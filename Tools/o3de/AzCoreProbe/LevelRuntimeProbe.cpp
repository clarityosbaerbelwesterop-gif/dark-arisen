// O3DE 2605.0 level runtime probe.
//
// Loads the level prefabs written by `DarkArisenO3DE materialize`, deserialises every Dark Arisen
// game component with O3DE's own JSON serializer (unknown or mistyped fields fail the probe),
// instantiates the entities on a real AZ::ComponentApplication and plays Chapter 1 through the
// materialised content: Harlow family, cargo manifest, black sails, boarders, Draven, The Taking,
// the level transition, the Outer Reef swim, Driftwood Beach and the walk to Driftwood Camp.
//
// What stands in for engine systems that need a GPU or a PhysX scene: transforms, the character
// controller (velocity integration plus a beach ground profile), trigger overlap (point-in-box on
// the materialised trigger colliders), the game entity context (activate/deactivate) and the
// level system ("LoadLevel" console command). Rendering components are counted, not run.

#include "LevelProbeSupport.h"

int main(int argc, char** argv)
{
    const std::filesystem::path repo = argc > 1 ? std::filesystem::path(argv[1]) : std::filesystem::path(DARKARISEN_REPO_ROOT);
    const std::filesystem::path levels = repo / "Engine/O3DE/DarkArisen/Levels";
    const std::filesystem::path userDir = std::filesystem::temp_directory_path() / "darkarisen_level_probe";
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
             DarkArisen::OpeningDirectorComponent::CreateDescriptor(), DarkArisen::MapTransitionComponent::CreateDescriptor(),
             DarkArisen::PlayerSpawnComponent::CreateDescriptor(), DarkArisen::SwimmerComponent::CreateDescriptor(),
             DarkArisen::WaterVolumeComponent::CreateDescriptor(), DarkArisen::OceanComponent::CreateDescriptor(),
             DarkArisen::CameraRigComponent::CreateDescriptor(), DarkArisen::LockOnComponent::CreateDescriptor(),
             ProbeSupport::ProbeTransformComponent::CreateDescriptor(), ProbeSupport::ProbeCharacterComponent::CreateDescriptor()})
    {
        app.RegisterComponentDescriptor(componentDescriptor);
    }
    systemEntity->CreateComponent<DarkArisen::CampaignSystemComponent>();
    systemEntity->Init();
    systemEntity->Activate();
    auto* campaign = DarkArisen::CampaignInterface::Get();
    Check(campaign != nullptr, "campaign system component active");
    ProbeGameEntityContext gameContext;
    AZ::SerializeContext* serialize = app.GetSerializeContext();

    const auto open = [&](const char* name, LoadedLevel& level)
    {
        AzFramework::LevelSystemLifecycleNotificationBus::Broadcast(&AzFramework::LevelSystemLifecycleNotifications::OnLoadingStart, name);
        const bool loaded = LoadPrefab(levels / name / (std::string(name) + ".prefab"), name, serialize, level);
        AzFramework::LevelSystemLifecycleNotificationBus::Broadcast(&AzFramework::LevelSystemLifecycleNotifications::OnLoadingComplete, name);
        return loaded;
    };

    // ---- L_HarlowOpening: Chapter 1, missions 1-3.
    LoadedLevel level;
    Check(open("L_HarlowOpening", level), "L_HarlowOpening.prefab parsed");
    std::printf("  %d game components loaded, %d rejected, %d engine components counted\n", level.m_gameComponents,
        level.m_rejectedComponents, level.m_engineComponents);
    Check(level.m_gameComponents == 18 && level.m_rejectedComponents == 0,
        "Harlow: every game component deserialised by O3DE's JSON serializer with no unknown fields");
    {
        // The drawn ocean (material written by the materialiser) and the simulated one are the same sea.
        std::ifstream materialStream(repo / "Engine/O3DE/DarkArisen/Assets/Materials/DarkArisenOcean.material", std::ios::binary);
        const std::string materialText((std::istreambuf_iterator<char>(materialStream)), std::istreambuf_iterator<char>());
        rapidjson::Document material;
        material.Parse(materialText.c_str());
        const DarkArisen::OceanRequests* sea = DarkArisen::OceanInterface::Get();
        bool same = sea != nullptr && !material.HasParseError() && material.HasMember("propertyValues");
        if (same)
        {
            const std::vector<float> constants = sea->GetSurface().PackShaderConstants();
            for (size_t wave = 0; wave < constants.size() / 4 && same; ++wave)
            {
                const AZStd::string key = AZStd::string::format("waves.w%zu", wave);
                const auto found = material["propertyValues"].FindMember(key.c_str());
                same = found != material["propertyValues"].MemberEnd() && found->value.IsArray() && found->value.Size() == 4;
                for (rapidjson::SizeType component = 0; same && component < 4; ++component)
                {
                    same = AZ::IsClose(found->value[component].GetFloat(), constants[wave * 4 + component], 1e-6f);
                }
            }
        }
        Check(same, "ocean material waves equal the live OceanComponent surface (drawn sea = simulated sea)");
    }
    const AZ::EntityId jake = level.Id("Jake");
    Check(Position(jake).IsClose(AZ::Vector3(-9.0f, 0.0f, 2.2f)), "Harlow: player spawn resolved to Spawn.Jake.HarlowOpening");
    const AZ::EntityId boarders[] = {level.Id("Boarder Boarding.Port.A"), level.Id("Boarder Boarding.Port.B"), level.Id("Boarder Boarding.Starboard.A")};
    Check(!IsActive(boarders[0]) && !IsActive(boarders[1]) && !IsActive(boarders[2]), "Harlow: boarders placed inactive");

    bool accepted = false;
    DarkArisen::StoryTriggerRequestBus::EventResult(accepted, level.Id("Cargo Manifest"), &DarkArisen::StoryTriggerRequests::Interact,
        AZStd::string("character.jake"));
    Check(!accepted, "cargo manifest refused before the family is spoken to");
    for (const char* member : {"Family marc", "Family denise", "Family ethan"})
    {
        accepted = false;
        DarkArisen::StoryTriggerRequestBus::EventResult(accepted, level.Id(member), &DarkArisen::StoryTriggerRequests::Interact,
            AZStd::string("character.jake"));
        Check(accepted, member);
    }
    DarkArisen::Core::CampaignRuntime* runtime = &campaign->GetCampaign();
    Check(runtime->GetMissionState("Main.C01.01.HomeWater") == DarkArisen::Core::MissionState::Completed, "Home Water complete");
    DarkArisen::StoryTriggerRequestBus::EventResult(accepted, level.Id("Cargo Manifest"), &DarkArisen::StoryTriggerRequests::Interact,
        AZStd::string("character.jake"));
    Check(accepted, "cargo manifest: black sails sighted");
    Tick(level, jake, 7.0f);
    Check(IsActive(boarders[0]) && IsActive(boarders[1]) && IsActive(boarders[2]), "black sails presentation released three boarders");
    Check(campaign->GetOpening().Progress().RaidState == DarkArisen::Core::OpeningRaidState::FirstEncounter, "boarding encounter running");
    Check(Position(boarders[0]).GetY() > 0.0f && Position(boarders[2]).GetY() < 0.0f,
        "Harlow: port boarders on +Y (left when facing the bow), starboard on -Y");

    // Lock-on and camera: face the bow (+X), lock the best boarder in front, the camera turns to it.
    AZ::TransformBus::Event(jake, &AZ::TransformBus::Events::SetWorldRotationQuaternion,
        AZ::Quaternion::CreateRotationZ(AZ::DegToRad(-90.0f)));
    bool locked = false;
    DarkArisen::LockOnRequestBus::EventResult(locked, jake, &DarkArisen::LockOnRequests::ToggleLockOn);
    AZ::EntityId lockTarget;
    DarkArisen::LockOnRequestBus::EventResult(lockTarget, jake, &DarkArisen::LockOnRequests::GetLockTarget);
    Check(locked && (lockTarget == boarders[0] || lockTarget == boarders[2]), "lock-on takes the nearest boarder in front of Jake");
    Tick(level, jake, 1.0f);
    float cameraYaw = 0.0f;
    DarkArisen::CameraRigRequestBus::BroadcastResult(cameraYaw, &DarkArisen::CameraRigRequests::GetYawDegrees);
    const AZ::Vector3 toTarget = Position(lockTarget) - Position(jake);
    const float expectedYaw = AZ::RadToDeg(AZ::Atan2(toTarget.GetX(), toTarget.GetY()));
    const AZ::Vector3 cameraPosition = Position(level.Id("Jake Camera"));
    const float cameraDistance = (cameraPosition - (Position(jake) + AZ::Vector3(0.0f, 0.0f, 0.96f))).GetLength();
    Check(AZ::GetAbs(cameraYaw - expectedYaw) < 3.0f && cameraDistance > 3.5f && cameraDistance < 3.8f,
        "camera rig frames the lock target from the 3.6 m boom behind Jake");
    for (const AZ::EntityId& boarder : boarders)
    {
        if (DarkArisen::Core::Combatant* combatant = CombatantOf(boarder))
        {
            combatant->Health.ApplyDamage(10000.0f);
        }
    }
    Tick(level, jake, 0.5f);
    Check(campaign->GetOpening().Progress().RaidState == DarkArisen::Core::OpeningRaidState::DravenAboard, "deck cleared: Draven boards");
    DarkArisen::LockOnRequestBus::EventResult(locked, jake, &DarkArisen::LockOnRequests::IsLockedOn);
    Check(!locked, "lock released when the target falls");
    Tick(level, jake, 4.0f);
    Check(campaign->GetOpening().Progress().RaidState == DarkArisen::Core::OpeningRaidState::Taking, "Draven presentation: The Taking starts");
    Tick(level, jake, 17.0f);
    Check(!IsActive(level.Id("Family marc")) && !IsActive(level.Id("Family denise")) && !IsActive(level.Id("Family ethan")),
        "The Taking: Marc and Denise fall, Ethan is seized alive");
    Check(Position(jake).IsClose(Position(level.Id("Anchor Overboard"))), "Jake forced over the rail at the Overboard anchor");
    Check(runtime->HasFact(DarkArisen::Core::Facts::EthanAbducted) && !runtime->HasFact(DarkArisen::Core::Facts::EthanRecovered),
        "canon: Ethan abducted alive, not recovered");
    Tick(level, jake, 2.0f);
    Check(runtime->GetMissionState("Main.C01.04.Undertow") == DarkArisen::Core::MissionState::Active, "Undertow active");
    Check(g_requestedLevel == "L_DriftwoodBeach", "map transition travels to L_DriftwoodBeach");
    Check(runtime->State().PlayerRuntime.SourceLevel == "L_HarlowOpening", "travel captured the player in its source level");

    // ---- L_DriftwoodBeach: Undertow.
    g_requestedLevel.clear();
    UnloadLevel(level);
    Check(open("L_DriftwoodBeach", level), "L_DriftwoodBeach.prefab parsed");
    std::printf("  %d game components loaded, %d rejected, %d engine components counted\n", level.m_gameComponents,
        level.m_rejectedComponents, level.m_engineComponents);
    Check(level.m_gameComponents == 15 && level.m_rejectedComponents == 0, "Driftwood: every game component deserialised cleanly");
    const AZ::EntityId swimmer = level.Id("Jake");
    Check(Position(swimmer).IsClose(Position(level.Id("Anchor WaterEntry"))), "overboard arrival spawns Jake in the water, not on the beach");
    if (auto* character = ProbeGameEntityContext::Find(swimmer)->FindComponent<ProbeSupport::ProbeCharacterComponent>())
    {
        // Beach profile from the layout: seabed about 2 m down offshore, rising to 0.55 m at the recovery
        // point; inland the walk follows the authored anchor heights.
        character->m_ground = [](const float x, float)
        {
            return x < -20.0f ? -2.0f : AZ::GetMin(0.55f, -2.0f + (x + 20.0f) * (2.55f / 11.0f));
        };
    }
    Tick(level, swimmer, 0.2f);
    bool swimming = false;
    DarkArisen::SwimRequestBus::EventResult(swimming, swimmer, &DarkArisen::SwimRequests::IsSwimming);
    Check(swimming, "Current.A volume: Jake swims");
    Check(campaign->GetOpening().Progress().Location == DarkArisen::Core::OpeningLocation::OpenWater, "entered water signalled");

    // Death in the water: after the two-second death beat Jake is back at the overboard arrival,
    // swimming again with full vitals (the trigger never re-fires for a body placed inside it).
    AZ::TransformBus::Event(swimmer, &AZ::TransformBus::Events::SetWorldTranslation, AZ::Vector3(-60.0f, -5.0f, -1.4f));
    if (DarkArisen::Core::Combatant* body = CombatantOf(swimmer))
    {
        body->Health.ApplyDamage(10000.0f, DarkArisen::Core::RallyDamageClass::Environmental);
    }
    Tick(level, swimmer, 1.5f);
    bool dead = false;
    DarkArisen::CombatRequestBus::EventResult(dead, swimmer, &DarkArisen::CombatRequests::IsDead);
    Check(dead, "death beat: still down 1.5 s after dying");
    Tick(level, swimmer, 1.0f);
    DarkArisen::CombatRequestBus::EventResult(dead, swimmer, &DarkArisen::CombatRequests::IsDead);
    DarkArisen::SwimRequestBus::EventResult(swimming, swimmer, &DarkArisen::SwimRequests::IsSwimming);
    float health = 0.0f;
    DarkArisen::CombatRequestBus::EventResult(health, swimmer, &DarkArisen::CombatRequests::GetHealthFraction);
    Check(!dead && health == 1.0f && swimming && Position(swimmer).GetDistance(Position(level.Id("Anchor WaterEntry"))) < 3.0f,
        "respawn at the overboard arrival, full health, swimming again (no failure screen)");
    const AZ::Vector3 waypoints[] = {Position(level.Id("Water Reef.SafeGap")), Position(level.Id("Water Shallows")), AZ::Vector3(-11.0f, 0.0f, 0.0f)};
    for (const AZ::Vector3& waypoint : waypoints)
    {
        for (int frame = 0; frame < 60 * 60 && g_requestedLevel.empty(); ++frame)
        {
            AZ::Vector3 delta = waypoint - Position(swimmer);
            delta.SetZ(0.0f);
            if (delta.GetLength() < 1.5f)
            {
                break;
            }
            DarkArisen::SwimRequestBus::Event(swimmer, &DarkArisen::SwimRequests::SetSwimIntent,
                AZ::Vector2(delta.GetX(), delta.GetY()).GetNormalized(), false);
            Tick(level, swimmer, 1.0f / 60.0f);
        }
    }
    Check(campaign->GetOpening().Progress().Location == DarkArisen::Core::OpeningLocation::OuterReef, "Outer Reef safe gap reached by swimming");
    DarkArisen::SwimRequestBus::EventResult(swimming, swimmer, &DarkArisen::SwimRequests::IsSwimming);
    Check(!swimming, "shallow exit at wading depth: Jake walks out");
    Walk(level, swimmer, Position(level.Id("Anchor Spawn.Moran.DriftwoodBeach.Recovery")));
    Check(campaign->GetOpening().Progress().Location == DarkArisen::Core::OpeningLocation::DriftwoodBeach, "Driftwood Beach reached");
    Walk(level, swimmer, Position(level.Id("Beat Camp Smoke Sighted")));
    Check(runtime->GetMissionState("Main.C01.04.Undertow") == DarkArisen::Core::MissionState::Completed &&
              runtime->GetMissionState("Main.C02.01.ShatteredCoast") == DarkArisen::Core::MissionState::Active,
        "camp smoke sighted: Undertow complete, Shattered Coast active");
    Check(runtime->State().SpawnId == "Spawn.Moran.DriftwoodBeach.Recovery", "beach checkpoint set");
    std::error_code ignored;
    bool autosave = false;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(userDir, ignored))
    {
        autosave = autosave || entry.path().extension() == ".dasave";
    }
    Check(autosave, "chapter 1 -> 2 boundary wrote an autosave through O3DE LocalFileIO");
    // Death after the recovery: back at the beach checkpoint.
    if (DarkArisen::Core::Combatant* body = CombatantOf(swimmer))
    {
        body->Health.ApplyDamage(10000.0f);
    }
    Tick(level, swimmer, 2.5f);
    DarkArisen::CombatRequestBus::EventResult(dead, swimmer, &DarkArisen::CombatRequests::IsDead);
    Check(!dead && Position(swimmer).GetDistance(Position(level.Id("Anchor Spawn.Moran.DriftwoodBeach.Recovery"))) < 0.5f,
        "respawn at the Driftwood Beach checkpoint");
    Walk(level, swimmer, Position(level.Id("Beat Route Driftwood Camp")));
    Tick(level, swimmer, 0.2f);
    Check(runtime->HasFact("World.DriftwoodCampReached") && g_requestedLevel == "L_DriftwoodCamp", "camp route: travel to L_DriftwoodCamp");

    // ---- L_DriftwoodCamp.
    g_requestedLevel.clear();
    UnloadLevel(level);
    Check(open("L_DriftwoodCamp", level), "L_DriftwoodCamp.prefab parsed");
    Check(level.m_gameComponents == 5 && level.m_rejectedComponents == 0, "Camp: every game component deserialised cleanly");
    Check(Position(level.Id("Jake")).IsClose(Position(level.Id("Anchor Arrival"))), "camp arrival spawn");
    Check(campaign->GetOpening().Progress().Location == DarkArisen::Core::OpeningLocation::DriftwoodCamp, "vertical slice ends at Driftwood Camp");

    UnloadLevel(level);
    systemEntity->Deactivate();
    AZ::IO::FileIOBase::SetInstance(nullptr);
    AZ::IO::FileIOBase::SetInstance(previousIO);
    app.Destroy();
    std::filesystem::remove_all(userDir);
    std::printf("%s (%d failed)\n", g_failures == 0 ? "O3DE level runtime probe PASSED" : "O3DE level runtime probe FAILED", g_failures);
    return g_failures == 0 ? 0 : 1;
}
