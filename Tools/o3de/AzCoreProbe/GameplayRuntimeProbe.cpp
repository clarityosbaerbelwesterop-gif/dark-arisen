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
#include "World/WaterVolumeComponent.h"
#include "Player/SwimmerComponent.h"
#include <DarkArisen/SwimBus.h>
#include <AzFramework/Physics/CharacterBus.h>
#include <PhysX/CharacterGameplayBus.h>
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

    /**
     * Stands in for the PhysX character controller and character gameplay components (probe only):
     * integrates the velocity requested each tick, and records the gravity multiplier. A locked
     * vertical axis simulates being pinned under wreckage or standing on the seabed.
     */
    class ProbeCharacterComponent
        : public AZ::Component
        , public Physics::CharacterRequestBus::Handler
        , public PhysX::CharacterGameplayRequestBus::Handler
        , public AZ::TickBus::Handler
    {
    public:
        AZ_COMPONENT(ProbeCharacterComponent, "{7C2E9A14-3B6D-4F80-A5C1-9E0D2B4F6A37}");
        static void Reflect(AZ::ReflectContext* context)
        {
            if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
            {
                serialize->Class<ProbeCharacterComponent, AZ::Component>()->Version(1);
            }
        }
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
        {
            provided.push_back(AZ_CRC_CE("PhysicsCharacterControllerService"));
            provided.push_back(AZ_CRC_CE("PhysicsCharacterGameplayService"));
        }
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
        {
            required.push_back(AZ_CRC_CE("TransformService"));
        }
        void Activate() override
        {
            Physics::CharacterRequestBus::Handler::BusConnect(GetEntityId());
            PhysX::CharacterGameplayRequestBus::Handler::BusConnect(GetEntityId());
            AZ::TickBus::Handler::BusConnect();
        }
        void Deactivate() override
        {
            AZ::TickBus::Handler::BusDisconnect();
            PhysX::CharacterGameplayRequestBus::Handler::BusDisconnect();
            Physics::CharacterRequestBus::Handler::BusDisconnect();
        }
        int GetTickOrder() override { return AZ::TICK_LAST; }
        void OnTick(const float deltaTime, AZ::ScriptTimePoint) override
        {
            AZ::Vector3 velocity = m_pending;
            if (m_lockVertical)
            {
                velocity.SetZ(0.0f);
            }
            AZ::Vector3 position = AZ::Vector3::CreateZero();
            AZ::TransformBus::EventResult(position, GetEntityId(), &AZ::TransformBus::Events::GetWorldTranslation);
            AZ::TransformBus::Event(GetEntityId(), &AZ::TransformBus::Events::SetWorldTranslation, position + velocity * deltaTime);
            m_velocity = velocity;
            m_pending = AZ::Vector3::CreateZero();
        }

        // Physics::CharacterRequests
        AZ::Vector3 GetBasePosition() const override
        {
            AZ::Vector3 position = AZ::Vector3::CreateZero();
            AZ::TransformBus::EventResult(position, GetEntityId(), &AZ::TransformBus::Events::GetWorldTranslation);
            return position;
        }
        void SetBasePosition(const AZ::Vector3& position) override
        {
            AZ::TransformBus::Event(GetEntityId(), &AZ::TransformBus::Events::SetWorldTranslation, position);
        }
        AZ::Vector3 GetCenterPosition() const override { return GetBasePosition() + AZ::Vector3(0.0f, 0.0f, 0.9f); }
        float GetStepHeight() const override { return 0.3f; }
        void SetStepHeight(float) override {}
        AZ::Vector3 GetUpDirection() const override { return AZ::Vector3::CreateAxisZ(); }
        void SetUpDirection(const AZ::Vector3&) override {}
        float GetSlopeLimitDegrees() const override { return 45.0f; }
        void SetSlopeLimitDegrees(float) override {}
        float GetMaximumSpeed() const override { return 0.0f; }
        void SetMaximumSpeed(float) override {}
        AZ::Vector3 GetVelocity() const override { return m_velocity; }
        void AddVelocityForTick(const AZ::Vector3& velocity) override { m_pending += velocity; }
        void AddVelocityForPhysicsTimestep(const AZ::Vector3& velocity) override { m_pending += velocity; }
        bool IsPresent() const override { return true; }
        Physics::Character* GetCharacter() override { return nullptr; }

        // PhysX::CharacterGameplayRequests
        bool IsOnGround() const override { return m_lockVertical; }
        float GetGravityMultiplier() const override { return m_gravityMultiplier; }
        void SetGravityMultiplier(const float multiplier) override { m_gravityMultiplier = multiplier; }
        float GetGroundDetectionBoxHeight() const override { return 0.05f; }
        void SetGroundDetectionBoxHeight(float) override {}
        AZ::Vector3 GetFallingVelocity() const override { return m_fallingVelocity; }
        void SetFallingVelocity(const AZ::Vector3& velocity) override { m_fallingVelocity = velocity; }

        float m_gravityMultiplier = 1.0f;
        AZ::Vector3 m_fallingVelocity = AZ::Vector3(0.0f, 0.0f, -3.0f);
        bool m_lockVertical = false;

    private:
        AZ::Vector3 m_pending = AZ::Vector3::CreateZero();
        AZ::Vector3 m_velocity = AZ::Vector3::CreateZero();
    };
}

namespace
{
    int g_failures = 0;

    /** Writes a reflected field the way a prefab would, by its serialized name. */
    template<class Component, class Value>
    bool SetReflectedField(AZ::SerializeContext* serialize, Component* component, const char* field, const Value& value)
    {
        const AZ::SerializeContext::ClassData* data = serialize ? serialize->FindClassData(azrtti_typeid<Component>()) : nullptr;
        if (!data)
        {
            return false;
        }
        for (const AZ::SerializeContext::ClassElement& element : data->m_elements)
        {
            if (AZStd::string_view(element.m_name) == field && element.m_typeId == azrtti_typeid<Value>())
            {
                *reinterpret_cast<Value*>(reinterpret_cast<char*>(component) + element.m_offset) = value;
                return true;
            }
        }
        return false;
    }

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
             DarkArisen::OceanComponent::CreateDescriptor(), DarkArisen::SwimmerComponent::CreateDescriptor(),
             DarkArisen::WaterVolumeComponent::CreateDescriptor(), ProbeSupport::ProbeTransformComponent::CreateDescriptor(),
             ProbeSupport::ProbeCharacterComponent::CreateDescriptor()})
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


    // ---- Swimming (Chapter 1 "Undertow"): Outer Reef Current.A from ContentSource, calm sea.
    if (sea)
    {
        sea->SetWeather(2.0f, 90.0f, 0.2f);
    }
    AZ::Entity* swimmer = MakeEntity("Swimmer", AZ::Vector3(-58.0f, 0.0f, -1.4f));
    swimmer->CreateComponent<DarkArisen::CombatantComponent>();
    auto* swimmerCharacter = swimmer->CreateComponent<ProbeSupport::ProbeCharacterComponent>();
    swimmer->CreateComponent<DarkArisen::SwimmerComponent>();
    AZ::Entity* currentA = MakeEntity("Current.A", AZ::Vector3(-58.0f, 0.0f, -1.0f));
    auto* currentVolume = currentA->CreateComponent<DarkArisen::WaterVolumeComponent>();
    Check(SetReflectedField(serialize, currentVolume, "CurrentAcceleration",
              AZ::Vector3(static_cast<float>(DarkArisen::Core::CurrentFromCentimetres(130.0)),
                  static_cast<float>(DarkArisen::Core::CurrentFromCentimetres(15.0)), 0.0f)),
        "water volume current set through its reflected field");
    AZ::Entity* shallows = MakeEntity("Shallows", AZ::Vector3(-23.0f, 0.0f, 0.0f));
    auto* shallowVolume = shallows->CreateComponent<DarkArisen::WaterVolumeComponent>();
    Check(SetReflectedField(serialize, shallowVolume, "ShallowExit", true), "shallow exit set through its reflected field");
    for (AZ::Entity* entity : {swimmer, currentA, shallows})
    {
        entity->Init();
        entity->Activate();  // the volumes report the missing PhysX trigger collider (no physics scene here)
    }
    Check(swimmer->GetState() == AZ::Entity::State::Active, "swimmer active (combatant + character controller services)");
    const AZ::EntityId swimmerId = swimmer->GetId();
    auto swimmerX = [swimmerId]()
    {
        AZ::Vector3 position = AZ::Vector3::CreateZero();
        AZ::TransformBus::EventResult(position, swimmerId, &AZ::TransformBus::Events::GetWorldTranslation);
        return position;
    };
    auto isSwimming = [swimmerId]()
    {
        bool swimming = false;
        DarkArisen::SwimRequestBus::EventResult(swimming, swimmerId, &DarkArisen::SwimRequests::IsSwimming);
        return swimming;
    };
    DarkArisen::Core::Combatant* swimmerModel = nullptr;
    DarkArisen::CombatRequestBus::Event(swimmerId, [&swimmerModel](DarkArisen::CombatRequests* handler) { swimmerModel = &handler->GetCombatant(); });

    currentVolume->NotifyEntered(swimmerId);
    Check(isSwimming() && swimmerCharacter->m_gravityMultiplier == 0.0f && swimmerCharacter->m_fallingVelocity.IsZero(),
        "entering water: swimming, PhysX gravity suspended, falling velocity cleared");
    Check(swimmerModel && swimmerModel->Stamina.RegenMultiplier < 0.3f, "stamina regen slowed in water");

    DarkArisen::SwimRequestBus::Event(swimmerId, &DarkArisen::SwimRequests::SetSwimIntent, AZ::Vector2(-0.4f, 0.0f), false);
    const float startX = swimmerX().GetX();
    Tick(8.0f);
    Check(swimmerX().GetX() > startX, "casual stroke against Current.A loses ground (design: sometimes impossible)");
    const AZ::Vector3 floating = swimmerX();
    const float surface = sea ? static_cast<float>(sea->GetSurface().Sample(floating.GetX(), floating.GetY(), sea->GetOceanTime()).Height) : 0.0f;
    Check(AZ::GetAbs(floating.GetZ() - (surface + 0.12f - 1.62f)) < 0.25f, "buoyancy floats the body with the eyes above the sampled sea");
    bool submerged = true;
    DarkArisen::SwimRequestBus::EventResult(submerged, swimmerId, &DarkArisen::SwimRequests::IsHeadSubmerged);
    Check(!submerged, "floating swimmer breathes");

    DarkArisen::SwimRequestBus::Event(swimmerId, &DarkArisen::SwimRequests::SetSwimIntent, AZ::Vector2(-1.0f, 0.0f), true);
    const float sprintStartX = swimmerX().GetX();
    const float staminaBefore = swimmerModel ? swimmerModel->Stamina.CurrentStamina : 0.0f;
    Tick(2.0f);
    DarkArisen::Core::SwimPace pace = DarkArisen::Core::SwimPace::Casual;
    DarkArisen::SwimRequestBus::EventResult(pace, swimmerId, &DarkArisen::SwimRequests::GetSwimPace);
    Check(swimmerX().GetX() < sprintStartX && pace == DarkArisen::Core::SwimPace::Sprint, "sprint stroke beats Current.A");
    // Fighting the current drains even casual strokes, so the sprint runs dry within seconds.
    Tick(3.0f);
    DarkArisen::SwimRequestBus::EventResult(pace, swimmerId, &DarkArisen::SwimRequests::GetSwimPace);
    Check(swimmerModel && swimmerModel->Stamina.CurrentStamina < staminaBefore - 50.0f && pace == DarkArisen::Core::SwimPace::Casual,
        "swim sprint drains stamina through the combatant and exhausts to casual strokes");

    // Pinned under wreckage: breath runs out, drowning deals environmental damage.
    DarkArisen::SwimRequestBus::Event(swimmerId, &DarkArisen::SwimRequests::SetSwimIntent, AZ::Vector2::CreateZero(), false);
    swimmerCharacter->m_lockVertical = true;
    AZ::TransformBus::Event(swimmerId, &AZ::TransformBus::Events::SetWorldTranslation, AZ::Vector3(-50.0f, 0.0f, -6.0f));
    const float healthBefore = swimmerModel ? swimmerModel->Health.CurrentHealth : 0.0f;
    Tick(31.5f);
    float breath = 1.0f;
    DarkArisen::SwimRequestBus::EventResult(breath, swimmerId, &DarkArisen::SwimRequests::GetBreathFraction);
    Check(breath == 0.0f && swimmerModel && AZ::GetAbs(healthBefore - swimmerModel->Health.CurrentHealth - 20.0f) < 0.01f,
        "30 s bare breath, then 20 drowning damage per whole second");
    swimmerCharacter->m_lockVertical = false;
    Tick(4.0f);
    DarkArisen::SwimRequestBus::EventResult(breath, swimmerId, &DarkArisen::SwimRequests::GetBreathFraction);
    Check(breath == 1.0f, "surfacing refills breath");

    // Shallows left seaward while still floating: the swimmer keeps swimming.
    currentVolume->NotifyExited(swimmerId);
    shallowVolume->NotifyEntered(swimmerId);
    shallowVolume->NotifyExited(swimmerId);
    Tick(0.5f);
    Check(isSwimming(), "leaving the shallows in deep water keeps swimming");
    // Shallows left towards the beach at standing depth: walk out, gravity and regen restored.
    shallowVolume->NotifyEntered(swimmerId);
    swimmerCharacter->m_lockVertical = true;
    AZ::TransformBus::Event(swimmerId, &AZ::TransformBus::Events::SetWorldTranslation, AZ::Vector3(-12.0f, 0.0f, surface - 0.6f));
    shallowVolume->NotifyExited(swimmerId);
    Tick(0.1f);
    Check(!isSwimming() && swimmerCharacter->m_gravityMultiplier == 1.0f && swimmerModel && swimmerModel->Stamina.RegenMultiplier == 1.0f,
        "shallow exit at wading depth: walking, gravity and stamina regen restored");

    enemyEvents.BusDisconnect();
    for (AZ::Entity* entity : {shallows, currentA, swimmer, ship, ocean, ethan, boarder, jake})
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
