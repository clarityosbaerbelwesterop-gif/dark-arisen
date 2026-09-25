#include "SwimmerComponent.h"

#include "Story/StoryTriggerComponent.h"

#include <DarkArisen/CombatBus.h>
#include <DarkArisen/OceanBus.h>

#include <AzCore/Component/TransformBus.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/std/algorithm.h>
#include <AzCore/Math/MathUtils.h>
#include <AzFramework/Physics/CharacterBus.h>
#include <PhysX/CharacterGameplayBus.h>

#include <DarkArisen/Core/Combat.h>
#include <DarkArisen/Core/Ocean.h>

namespace DarkArisen
{
    AZ_COMPONENT_IMPL(SwimmerComponent, "SwimmerComponent", SwimmerComponentTypeId);

    void SwimmerComponent::Reflect(AZ::ReflectContext* context)
    {
        auto* serializeContext = azrtti_cast<AZ::SerializeContext*>(context);
        if (!serializeContext)
        {
            return;
        }
        serializeContext->Class<SwimmerComponent, AZ::Component>()
            ->Version(1)
            ->Field("EyeHeight", &SwimmerComponent::m_eyeHeight)
            ->Field("HeadClearance", &SwimmerComponent::m_headClearance)
            ->Field("WadeDepth", &SwimmerComponent::m_wadeDepth)
            ->Field("BuoyancyResponse", &SwimmerComponent::m_buoyancyResponseSeconds)
            ->Field("MaxVerticalSpeed", &SwimmerComponent::m_maxVerticalSpeed);
        if (AZ::EditContext* editContext = serializeContext->GetEditContext())
        {
            editContext->Class<SwimmerComponent>("Dark Arisen Swimmer", "Surface swimming, currents and breath.")
                ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                ->Attribute(AZ::Edit::Attributes::Category, "Dark Arisen")
                ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"))
                ->DataElement(AZ::Edit::UIHandlers::Default, &SwimmerComponent::m_eyeHeight, "Eye Height (m)", "Above the controller base")
                ->DataElement(AZ::Edit::UIHandlers::Default, &SwimmerComponent::m_headClearance, "Head Clearance (m)",
                    "Eyes above the surface when floating")
                ->DataElement(AZ::Edit::UIHandlers::Default, &SwimmerComponent::m_wadeDepth, "Wade Depth (m)",
                    "Base this close below the surface can stand")
                ->DataElement(AZ::Edit::UIHandlers::Default, &SwimmerComponent::m_buoyancyResponseSeconds, "Buoyancy Response (s)", "")
                ->DataElement(AZ::Edit::UIHandlers::Default, &SwimmerComponent::m_maxVerticalSpeed, "Max Vertical Speed (m/s)", "");
        }
    }

    void SwimmerComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("DarkArisenSwimmerService"));
    }

    void SwimmerComponent::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        required.push_back(AZ_CRC_CE("DarkArisenCombatantService"));
        required.push_back(AZ_CRC_CE("PhysicsCharacterControllerService"));
    }

    void SwimmerComponent::Activate()
    {
        m_volumes.clear();
        m_breath.ResetForRespawn();
        SwimRequestBus::Handler::BusConnect(GetEntityId());
        AZ::TickBus::Handler::BusConnect();
    }

    void SwimmerComponent::Deactivate()
    {
        AZ::TickBus::Handler::BusDisconnect();
        SwimRequestBus::Handler::BusDisconnect();
        if (m_swim.IsSwimming())
        {
            LeaveWater();
        }
        m_volumes.clear();
    }

    void SwimmerComponent::OnEnteredWaterVolume(const WaterVolumeInfo& volume)
    {
        if (!volume.m_currentAcceleration.IsFinite() || !AZ::IsFiniteFloat(volume.m_surfaceHeight))
        {
            AZ_Error("DarkArisen", false, "Water volume %s has a non-finite current or surface.", volume.m_volume.ToString().c_str());
            return;
        }
        auto existing = AZStd::find_if(m_volumes.begin(), m_volumes.end(),
            [&volume](const WaterVolumeInfo& other) { return other.m_volume == volume.m_volume; });
        if (existing != m_volumes.end())
        {
            m_volumes.erase(existing);
        }
        m_volumes.push_back(volume);
        m_lastVolume = volume;
        if (!m_swim.IsSwimming())
        {
            EnterWater();
        }
        ApplyCurrent();
    }

    void SwimmerComponent::OnExitedWaterVolume(const AZ::EntityId volume)
    {
        auto existing = AZStd::find_if(m_volumes.begin(), m_volumes.end(),
            [volume](const WaterVolumeInfo& other) { return other.m_volume == volume; });
        if (existing == m_volumes.end())
        {
            return;
        }
        m_lastExitWasShallow = existing->m_shallowExit;
        m_volumes.erase(existing);
        if (!m_volumes.empty())
        {
            m_lastVolume = m_volumes.back();
        }
        // Outside every authored volume the sea still holds the swimmer, without a current.
        ApplyCurrent();
    }

    void SwimmerComponent::SetSwimIntent(const AZ::Vector2& intent, const bool sprintHeld)
    {
        m_intent = intent.IsFinite() ? intent : AZ::Vector2::CreateZero();
        m_sprintHeld = sprintHeld;
    }

    void SwimmerComponent::ResetForRespawn()
    {
        if (m_swim.IsSwimming())
        {
            LeaveWater();
        }
        m_volumes.clear();
        m_breath.ResetForRespawn();
        m_intent = AZ::Vector2::CreateZero();
    }

    void SwimmerComponent::RefreshWaterVolumes()
    {
        AZ::Vector3 position = AZ::Vector3::CreateZero();
        AZ::TransformBus::EventResult(position, GetEntityId(), &AZ::TransformBus::Events::GetWorldTranslation);
        AZStd::vector<WaterVolumeInfo> containing;
        WaterVolumeQueryBus::Broadcast(&WaterVolumeQueries::AppendIfContains, position, containing);
        // Volumes no longer containing the body are left; new ones are entered (duplicates are merged).
        AZStd::vector<AZ::EntityId> stale;
        for (const WaterVolumeInfo& volume : m_volumes)
        {
            const bool still = AZStd::any_of(containing.begin(), containing.end(),
                [&volume](const WaterVolumeInfo& other) { return other.m_volume == volume.m_volume; });
            if (!still)
            {
                stale.push_back(volume.m_volume);
            }
        }
        for (const AZ::EntityId& volume : stale)
        {
            OnExitedWaterVolume(volume);
        }
        for (const WaterVolumeInfo& volume : containing)
        {
            const bool known = AZStd::any_of(m_volumes.begin(), m_volumes.end(),
                [&volume](const WaterVolumeInfo& other) { return other.m_volume == volume.m_volume; });
            OnEnteredWaterVolume(volume);
            if (!known)
            {
                // The volume's story beat (e.g. entering the water) is gated and one-shot, so telling it is safe.
                StoryTriggerRequestBus::Event(volume.m_volume, &StoryTriggerRequests::NotifyBodyEntered, GetEntityId());
            }
        }
    }

    void SwimmerComponent::EnterWater()
    {
        AZ::Vector3 velocity = AZ::Vector3::CreateZero();
        Physics::CharacterRequestBus::EventResult(velocity, GetEntityId(), &Physics::CharacterRequests::GetVelocity);
        m_swim.Enter(velocity.GetX(), velocity.GetY());
        m_lastExitWasShallow = false;

        // Buoyancy replaces gravity while afloat.
        PhysX::CharacterGameplayRequestBus::EventResult(
            m_savedGravityMultiplier, GetEntityId(), &PhysX::CharacterGameplayRequests::GetGravityMultiplier);
        PhysX::CharacterGameplayRequestBus::Event(GetEntityId(), &PhysX::CharacterGameplayRequests::SetGravityMultiplier, 0.0f);
        PhysX::CharacterGameplayRequestBus::Event(
            GetEntityId(), &PhysX::CharacterGameplayRequests::SetFallingVelocity, AZ::Vector3::CreateZero());

        const float regen = m_swim.Tuning.InWaterRegenMultiplier;
        CombatRequestBus::Event(GetEntityId(), [this, regen](CombatRequests* handler)
        {
            Core::StaminaModel& stamina = handler->GetCombatant().Stamina;
            m_savedRegenMultiplier = stamina.RegenMultiplier;
            stamina.RegenMultiplier = regen;
            stamina.SetSprinting(false);
        });
    }

    void SwimmerComponent::LeaveWater()
    {
        m_swim.Exit();
        m_breath.SetHeadSubmerged(false);
        m_lastPace = Core::SwimPace::Casual;
        PhysX::CharacterGameplayRequestBus::Event(
            GetEntityId(), &PhysX::CharacterGameplayRequests::SetGravityMultiplier, m_savedGravityMultiplier);
        const float regen = m_savedRegenMultiplier;
        CombatRequestBus::Event(GetEntityId(), [regen](CombatRequests* handler)
        {
            handler->GetCombatant().Stamina.RegenMultiplier = regen;
        });
    }

    void SwimmerComponent::ApplyCurrent()
    {
        const AZ::Vector3 current = m_volumes.empty() ? AZ::Vector3::CreateZero() : m_volumes.back().m_currentAcceleration;
        m_swim.SetCurrentAcceleration(current.GetX(), current.GetY());
    }

    float SwimmerComponent::SampleSurface(const AZ::Vector3& position) const
    {
        const WaterVolumeInfo& volume = m_volumes.empty() ? m_lastVolume : m_volumes.back();
        if (volume.m_useOceanSurface)
        {
            if (const OceanRequests* ocean = OceanInterface::Get())
            {
                return static_cast<float>(ocean->GetSurface().Sample(position.GetX(), position.GetY(), ocean->GetOceanTime()).Height);
            }
        }
        return volume.m_surfaceHeight;
    }

    void SwimmerComponent::OnTick(const float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
    {
        if (!m_swim.IsSwimming() || deltaTime <= 0.0f)
        {
            return;
        }
        Core::Combatant* combatant = nullptr;
        CombatRequestBus::Event(GetEntityId(), [&combatant](CombatRequests* handler) { combatant = &handler->GetCombatant(); });
        if (!combatant)
        {
            AZ_Error("DarkArisen", false, "Swimmer %s lost its combatant.", GetEntityId().ToString().c_str());
            return;
        }

        AZ::Vector3 position = AZ::Vector3::CreateZero();
        AZ::TransformBus::EventResult(position, GetEntityId(), &AZ::TransformBus::Events::GetWorldTranslation);
        const float surface = SampleSurface(position);

        // Out of the last shallow volume and standing depth reached: walk out.
        if (m_volumes.empty() && m_lastExitWasShallow && position.GetZ() >= surface - m_wadeDepth)
        {
            LeaveWater();
            return;
        }

        m_breath.SetHeadSubmerged(position.GetZ() + m_eyeHeight < surface);
        m_breath.TickAndApply(deltaTime, combatant->Health);
        if (combatant->Health.IsDead())
        {
            return;  // the body stays where it is; respawn goes through ResetForRespawn.
        }

        const bool stroke = m_intent.GetLengthSq() > 0.0001f;
        const float intentLength = m_intent.GetLength();
        const Core::SwimPace requested = m_sprintHeld
            ? Core::SwimPace::Sprint
            : (intentLength > 0.5f ? Core::SwimPace::Active : Core::SwimPace::Casual);
        const Core::SwimStep step = m_swim.Tick(m_intent.GetX(), m_intent.GetY(), requested, combatant->Stamina, deltaTime);
        m_lastPace = step.EffectivePace;

        const float target = static_cast<float>(Core::FloatingBaseHeight(surface, m_eyeHeight, m_headClearance));
        const float response = AZ::GetMax(m_buoyancyResponseSeconds, 0.01f);
        const float vertical = AZ::GetClamp((target - position.GetZ()) / response, -m_maxVerticalSpeed, m_maxVerticalSpeed);
        const AZ::Vector3 velocity(static_cast<float>(step.VelocityX), static_cast<float>(step.VelocityY), vertical);
        Physics::CharacterRequestBus::Event(GetEntityId(), &Physics::CharacterRequests::AddVelocityForTick, velocity);

        if (stroke)
        {
            AZ::TransformBus::Event(GetEntityId(), &AZ::TransformBus::Events::SetWorldRotationQuaternion,
                AZ::Quaternion::CreateRotationZ(AZ::Atan2(-m_intent.GetX(), m_intent.GetY())));
        }
    }
}
