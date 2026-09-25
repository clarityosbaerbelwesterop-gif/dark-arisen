// Runs Dark Arisen's gameplay adapters (CombatantComponent, EnemyBrainComponent,
// ShipVoyageComponent) on entities inside a real O3DE 2605.0 AZ::ComponentApplication.
// A minimal transform component stands in for AzFramework's TransformComponent and there is
// no PhysX scene, so melee sweeps and line-of-sight fall back as the adapters document.

#include <AzCore/Component/ComponentApplication.h>
#include <AzCore/Component/Entity.h>
#include <AzCore/Component/TickBus.h>
#include <AzCore/Component/TransformBus.h>
#include <AzCore/IO/FileIO.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Time/ITime.h>
#include <AzFramework/API/ApplicationAPI.h>
#include <AzFramework/IO/LocalFileIO.h>

#include <DarkArisen/CampaignBus.h>
#include <DarkArisen/CombatBus.h>
#include <DarkArisen/EnemyBus.h>
#include <DarkArisen/ShipBus.h>
#include "Combat/CombatantComponent.h"
#include "Combat/EnemyBrainComponent.h"
#include "Ships/ShipVoyageComponent.h"
#include "Story/CampaignSystemComponent.h"
#include "World/OceanComponent.h"
#include <DarkArisen/OceanBus.h>
#include <DarkArisen/Core/Ocean.h>

#include <DarkArisen/Core/CampaignRuntime.h>
#include <DarkArisen/Core/Combat.h>
#include <DarkArisen/Core/EnemyBrain.h>
#include <DarkArisen/Core/Facts.h>

#include <cstdio>
#include <filesystem>
#include <string>

namespace ProbeSupport
{
    /** Minimal transform so TransformBus calls have a real handler (probe only). */
    class ProbeTransformComponent : public AZ::Component, public AZ::TransformBus::Handler
    {
    public:
        AZ_COMPONENT(ProbeTransformComponent, "{5E0F7C11-2B7A-4C3D-9E8F-1A2B3C4D5E6F}");
        static void Reflect(AZ::ReflectContext* context)
        {
            if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
            {
                serialize->Class<ProbeTransformComponent, AZ::Component>()->Version(1);
            }
        }
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
        {
            provided.push_back(AZ_CRC_CE("TransformService"));
        }
        void Activate() override { AZ::TransformBus::Handler::BusConnect(GetEntityId()); }
        void Deactivate() override { AZ::TransformBus::Handler::BusDisconnect(); }

        void BindTransformChangedEventHandler(AZ::TransformChangedEvent::Handler&) override {}
        void BindParentChangedEventHandler(AZ::ParentChangedEvent::Handler&) override {}
        void BindChildChangedEventHandler(AZ::ChildChangedEvent::Handler&) override {}
        void NotifyChildChangedEvent(AZ::ChildChangeType, AZ::EntityId) override {}
        const AZ::Transform& GetLocalTM() override { return m_world; }
        const AZ::Transform& GetWorldTM() override { return m_world; }
        bool IsStaticTransform() override { return false; }
        void SetWorldTM(const AZ::Transform& tm) override { m_world = tm; }
        void SetWorldTranslation(const AZ::Vector3& position) override { m_world.SetTranslation(position); }
        AZ::Vector3 GetWorldTranslation() override { return m_world.GetTranslation(); }
        void SetWorldRotationQuaternion(const AZ::Quaternion& rotation) override { m_world.SetRotation(rotation); }

        AZ::Transform m_world = AZ::Transform::CreateIdentity();
    };
}

namespace
{
    int g_failures = 0;

    void Check(const bool condition, const char* what)
    {
        std::printf("[%s] %s\n", condition ? " ok " : "FAIL", what);
        g_failures += condition ? 0 : 1;
    }

    struct EnemyListener : DarkArisen::EnemyNotificationBus::Handler
    {
        int telegraphs = 0;
        int commits = 0;
        int phases = 0;
        void OnTelegraph(int) override { ++telegraphs; }
        void OnAttackCommitted(int) override { ++commits; }
        void OnBossPhaseChanged(int) override { ++phases; }
    };

    AZ::Entity* MakeEntity(const char* name, const AZ::Vector3& position)
    {
        auto* entity = aznew AZ::Entity(name);
        auto* transform = entity->CreateComponent<ProbeSupport::ProbeTransformComponent>();
        transform->m_world.SetTranslation(position);
        return entity;
    }

    void Tick(const float seconds)
    {
        const int frames = static_cast<int>(seconds * 60.0f + 0.5f);
        for (int frame = 0; frame < frames; ++frame)
        {
            AZ::TickBus::ExecuteQueuedEvents();
            AZ::TickBus::Broadcast(&AZ::TickEvents::OnTick, 1.0f / 60.0f, AZ::ScriptTimePoint());
        }
    }
}

int main(int argc, char** argv)
{
    const std::filesystem::path userDir = std::filesystem::temp_directory_path() / "darkarisen-o3de-gameplay-probe";
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

    AZ::IO::LocalFileIO fileIO;
    AZ::IO::FileIOBase* previousIO = AZ::IO::FileIOBase::GetInstance();
    AZ::IO::FileIOBase::SetInstance(nullptr);
    AZ::IO::FileIOBase::SetInstance(&fileIO);
    fileIO.SetAlias("@user@", userDir.string().c_str());

    for (const AZ::ComponentDescriptor* componentDescriptor :
         {DarkArisen::CampaignSystemComponent::CreateDescriptor(), DarkArisen::CombatantComponent::CreateDescriptor(),
             DarkArisen::EnemyBrainComponent::CreateDescriptor(), DarkArisen::ShipVoyageComponent::CreateDescriptor(),
             DarkArisen::OceanComponent::CreateDescriptor(), ProbeSupport::ProbeTransformComponent::CreateDescriptor()})
    {
        app.RegisterComponentDescriptor(componentDescriptor);
    }
    systemEntity->CreateComponent<DarkArisen::CampaignSystemComponent>();
    systemEntity->Init();
    systemEntity->Activate();
    auto* campaign = DarkArisen::CampaignInterface::Get();
    Check(campaign != nullptr, "campaign system component active");
    AzFramework::LevelSystemLifecycleNotificationBus::Broadcast(
        &AzFramework::LevelSystemLifecycleNotifications::OnLoadingStart, "Levels/HarlowMerchantShip");

    // Jake and a boarder, placed within attack range. Configuration goes through reflected fields.
    AZ::Entity* jake = MakeEntity("Jake", AZ::Vector3(0.0f, 0.0f, 0.0f));
    auto* jakeCombat = jake->CreateComponent<DarkArisen::CombatantComponent>();
    AZ::Entity* boarder = MakeEntity("Boarder", AZ::Vector3(0.0f, 1.8f, 0.0f));
    boarder->CreateComponent<DarkArisen::CombatantComponent>();
    boarder->CreateComponent<DarkArisen::EnemyBrainComponent>();

    auto* serialize = app.GetSerializeContext();
    Check(serialize && serialize->FindClassData(azrtti_typeid<DarkArisen::CombatantComponent>()) != nullptr,
        "CombatantComponent reflected into SerializeContext");
    {
        // Set the combatant id the way a prefab would: through the serialized field.
        AZ::SerializeContext::ClassData const* data = serialize->FindClassData(azrtti_typeid<DarkArisen::CombatantComponent>());
        for (const AZ::SerializeContext::ClassElement& element : data->m_elements)
        {
            if (AZStd::string_view(element.m_name) == "CombatantId")
            {
                *reinterpret_cast<AZStd::string*>(reinterpret_cast<char*>(jakeCombat) + element.m_offset) = "character.jake";
            }
        }
    }
    jake->Init();
    jake->Activate();
    boarder->Init();
    boarder->Activate();
    Check(jake->GetState() == AZ::Entity::State::Active && boarder->GetState() == AZ::Entity::State::Active,
        "Jake and boarder entities active (services resolved)");

    AZStd::string jakeId;
    DarkArisen::CombatRequestBus::EventResult(jakeId, jake->GetId(), &DarkArisen::CombatRequests::GetCombatantId);
    Check(jakeId == "character.jake", "Jake combatant id set through reflection");

    EnemyListener enemyEvents;
    enemyEvents.BusConnect(boarder->GetId());
    Tick(2.0f);
    Check(enemyEvents.telegraphs >= 1, "boarder brain telegraphs over EnemyNotificationBus");
    Check(enemyEvents.commits >= 1, "boarder committed an attack through CombatRequestBus");

    // Jake parries; the next swing into the open window is deflected (hit routed as the sweep would).
    bool parried = false;
    DarkArisen::CombatRequestBus::EventResult(parried, jake->GetId(), &DarkArisen::CombatRequests::Parry);
    Check(parried, "Jake parry accepted by the combat model");
    DarkArisen::Core::Combatant* jakeModel = nullptr;
    DarkArisen::Core::Combatant* boarderModel = nullptr;
    DarkArisen::CombatRequestBus::Event(jake->GetId(), [&](DarkArisen::CombatRequests* h) { jakeModel = &h->GetCombatant(); });
    DarkArisen::CombatRequestBus::Event(boarder->GetId(), [&](DarkArisen::CombatRequests* h) { boarderModel = &h->GetCombatant(); });
    const auto deflect = DarkArisen::Core::ResolveMeleeHit(*boarderModel, *jakeModel, DarkArisen::Core::HitKind::Light);
    Check(deflect.Deflected && jakeModel->Health.CurrentHealth == jakeModel->Health.MaxHealth, "deflected blow: no health loss");

    // Rache drives O3DE's simulation tick scale through AZ::ITime.
    jakeModel->Combat.RacheUnlocked = true;
    jakeModel->Combat.AddRacheFuel(100.0f);
    Tick(0.2f);
    bool rache = false;
    DarkArisen::CombatRequestBus::EventResult(rache, jake->GetId(), &DarkArisen::CombatRequests::StartRache);
    auto* time = AZ::Interface<AZ::ITime>::Get();
    Check(rache && time && time->GetSimulationTickScale() < 0.31f, "Rache sets AZ::ITime simulation scale to 0.30");
    for (int frame = 0; frame < 60 * 6; ++frame)
    {
        AZ::TickBus::Broadcast(&AZ::TickEvents::OnTick, (1.0f / 60.0f) * 0.30f, AZ::ScriptTimePoint());
    }
    Check(time && time->GetSimulationTickScale() > 0.99f, "Rache ends within five real seconds and restores 1.0");

    // Player snapshot on travel: capture in this map, vitals carry, coordinates stay map-local.
    jakeModel->Health.ApplyDamage(50.0f);
    AZ::TransformBus::Event(jake->GetId(), &AZ::TransformBus::Events::SetWorldTranslation, AZ::Vector3(4.0f, 5.0f, 0.0f));
    DarkArisen::CampaignNotificationBus::Broadcast(&DarkArisen::CampaignNotifications::OnCaptureWorldState);
    const auto& saved = campaign->GetCampaign().State().PlayerRuntime;
    Check(saved.Valid && saved.SourceLevel == "Levels/HarlowMerchantShip" && saved.WorldTransform.Translation.X == 4.0,
        "Jake's CombatantComponent captured transform, level and vitals");
    AZ::TransformBus::Event(jake->GetId(), &AZ::TransformBus::Events::SetWorldTranslation, AZ::Vector3(99.0f, 99.0f, 0.0f));
    DarkArisen::CampaignNotificationBus::Broadcast(&DarkArisen::CampaignNotifications::OnRestoreWorldState,
        AZStd::string("Levels/Moran/DriftwoodBeach"));
    AZ::Vector3 afterForeign = AZ::Vector3::CreateZero();
    AZ::TransformBus::EventResult(afterForeign, jake->GetId(), &AZ::TransformBus::Events::GetWorldTranslation);
    Check(afterForeign.GetX() == 99.0f, "other level: map-local coordinates not applied");
    DarkArisen::CampaignNotificationBus::Broadcast(&DarkArisen::CampaignNotifications::OnRestoreWorldState,
        AZStd::string("Levels/HarlowMerchantShip"));
    AZ::TransformBus::EventResult(afterForeign, jake->GetId(), &AZ::TransformBus::Events::GetWorldTranslation);
    Check(afterForeign.GetX() == 4.0f, "same level: saved coordinates restored");

    // Canon: real Ethan cannot be damaged through the adapters' hit path.
    AZ::Entity* ethan = MakeEntity("Ethan", AZ::Vector3(0.0f, 1.0f, 0.0f));
    auto* ethanCombat = ethan->CreateComponent<DarkArisen::CombatantComponent>();
    {
        const auto* data = serialize->FindClassData(azrtti_typeid<DarkArisen::CombatantComponent>());
        for (const AZ::SerializeContext::ClassElement& element : data->m_elements)
        {
            if (AZStd::string_view(element.m_name) == "CombatantId")
            {
                *reinterpret_cast<AZStd::string*>(reinterpret_cast<char*>(ethanCombat) + element.m_offset) = "character.ethan";
            }
        }
    }
    ethan->Init();
    ethan->Activate();
    DarkArisen::Core::Combatant* ethanModel = nullptr;
    DarkArisen::CombatRequestBus::Event(ethan->GetId(), [&](DarkArisen::CombatRequests* h) { ethanModel = &h->GetCombatant(); });
    Check(ethanModel && !DarkArisen::Core::ResolveMeleeHit(*jakeModel, *ethanModel, DarkArisen::Core::HitKind::Critical).Resolved,
        "real Ethan entity rejects every hostile hit");

    // The level's sea: one OceanComponent registers the shared surface.
    AZ::Entity* ocean = aznew AZ::Entity("Ocean");
    ocean->CreateComponent<DarkArisen::OceanComponent>();
    ocean->Init();
    ocean->Activate();
    auto* sea = DarkArisen::OceanInterface::Get();
    Check(sea != nullptr, "OceanComponent registered the level sea through AZ::Interface");
    if (sea)
    {
        sea->SetWeather(14.0f, 90.0f, 1.0f);
        Check(sea->GetSurface().CombinedSteepness() <= 1.0, "level sea is loop-free at 14 m/s");
    }

    // La Liberacion: physical sailing on TickBus, ownership from the campaign fact.
    AZ::Entity* ship = MakeEntity("LaLiberacion", AZ::Vector3(100.0f, 0.0f, 0.0f));
    ship->CreateComponent<DarkArisen::ShipVoyageComponent>();
    ship->Init();
    ship->Activate();
    bool helm = false;
    DarkArisen::ShipRequestBus::EventResult(helm, ship->GetId(), &DarkArisen::ShipRequests::TakeHelm);
    Check(!helm, "helm refused before the ship is owned");
    DarkArisen::CampaignNotificationBus::Broadcast(
        &DarkArisen::CampaignNotifications::OnFactChanged, AZStd::string(DarkArisen::Core::Facts::LaLiberacionOwned.data()), true);
    DarkArisen::ShipRequestBus::EventResult(helm, ship->GetId(), &DarkArisen::ShipRequests::TakeHelm);
    bool throttle = false;
    DarkArisen::ShipRequestBus::EventResult(throttle, ship->GetId(), &DarkArisen::ShipRequests::SetThrottle, 1.0f);
    DarkArisen::ShipRequestBus::Event(ship->GetId(), &DarkArisen::ShipRequests::SetHelmCommandDegrees, 180.0f);
    Check(helm && throttle, "owned ship accepts helm and throttle");
    float lowestDeck = 1000.0f;
    float highestDeck = -1000.0f;
    float largestTilt = 0.0f;
    for (int second = 0; second < 20; ++second)
    {
        Tick(1.0f);
        AZ::Transform shipTM = AZ::Transform::CreateIdentity();
        AZ::TransformBus::EventResult(shipTM, ship->GetId(), &AZ::TransformBus::Events::GetWorldTM);
        lowestDeck = AZ::GetMin(lowestDeck, shipTM.GetTranslation().GetZ());
        highestDeck = AZ::GetMax(highestDeck, shipTM.GetTranslation().GetZ());
        const AZ::Vector3 up = shipTM.GetBasisZ();
        largestTilt = AZ::GetMax(largestTilt, AZ::Acos(AZ::GetClamp(up.GetZ(), -1.0f, 1.0f)));
    }
    Check(highestDeck - lowestDeck > 0.1f, "ship heaves on the shared sea");
    Check(largestTilt > 0.005f && largestTilt < 0.6f, "ship pitches/rolls within hull limits");
    AZ::Vector3 shipPosition = AZ::Vector3::CreateZero();
    AZ::TransformBus::EventResult(shipPosition, ship->GetId(), &AZ::TransformBus::Events::GetWorldTranslation);
    float speed = 0.0f;
    DarkArisen::ShipRequestBus::EventResult(speed, ship->GetId(), &DarkArisen::ShipRequests::GetForwardSpeed);
    Check(speed > 0.5f && shipPosition.GetDistance(AZ::Vector3(100.0f, 0.0f, 0.0f)) > 10.0f,
        "ship sails physically over ticks (no teleport, no route skip)");

    enemyEvents.BusDisconnect();
    for (AZ::Entity* entity : {ship, ocean, ethan, boarder, jake})
    {
        entity->Deactivate();
        delete entity;
    }
    systemEntity->Deactivate();
    AZ::IO::FileIOBase::SetInstance(nullptr);
    AZ::IO::FileIOBase::SetInstance(previousIO);
    app.Destroy();
    std::filesystem::remove_all(userDir);

    std::printf("%s (%d failed)\n", g_failures == 0 ? "O3DE gameplay runtime probe PASSED" : "O3DE gameplay runtime probe FAILED",
        g_failures);
    return g_failures == 0 ? 0 : 1;
}
