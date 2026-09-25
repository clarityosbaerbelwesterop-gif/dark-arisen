#include "ShipVoyageComponent.h"

#include <AzCore/Component/TransformBus.h>
#include <AzCore/Math/MathUtils.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/SerializeContext.h>

#include <DarkArisen/OceanBus.h>

#include <DarkArisen/Core/CampaignRuntime.h>
#include <DarkArisen/Core/Facts.h>
#include <DarkArisen/Core/Ocean.h>

namespace DarkArisen
{
    AZ_COMPONENT_IMPL(ShipVoyageComponent, "ShipVoyageComponent", ShipVoyageComponentTypeId);

    void ShipVoyageComponent::Reflect(AZ::ReflectContext* context)
    {
        auto* serializeContext = azrtti_cast<AZ::SerializeContext*>(context);
        if (!serializeContext)
        {
            return;
        }
        serializeContext->Class<ShipVoyageComponent, AZ::Component>()
            ->Version(1)
            ->Field("WindDirection", &ShipVoyageComponent::m_windDirectionDegrees)
            ->Field("WindStrength", &ShipVoyageComponent::m_windStrengthMetresPerSecond)
            ->Field("Waterline", &ShipVoyageComponent::m_waterlineHeightMetres);
        if (AZ::EditContext* editContext = serializeContext->GetEditContext())
        {
            editContext->Class<ShipVoyageComponent>("Dark Arisen Ship Voyage", "La Liberacion helm, wind and crew model.")
                ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                ->Attribute(AZ::Edit::Attributes::Category, "Dark Arisen")
                ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"))
                ->DataElement(AZ::Edit::UIHandlers::Default, &ShipVoyageComponent::m_windDirectionDegrees,
                    "Initial Wind Direction", "Degrees, 0 = north (+Y).")
                ->DataElement(AZ::Edit::UIHandlers::Default, &ShipVoyageComponent::m_windStrengthMetresPerSecond,
                    "Initial Wind Strength", "m/s (the level ocean overrides both wind values)")
                ->DataElement(AZ::Edit::UIHandlers::Default, &ShipVoyageComponent::m_waterlineHeightMetres, "Waterline Height",
                    "Entity origin height above still water in metres.");
        }
    }

    void ShipVoyageComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("DarkArisenShipService"));
    }

    void ShipVoyageComponent::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        required.push_back(AZ_CRC_CE("TransformService"));
    }

    void ShipVoyageComponent::Activate()
    {
        m_voyage = Core::ShipVoyage{};
        m_voyage.SetWind(m_windDirectionDegrees, m_windStrengthMetresPerSecond);
        ShipRequestBus::Handler::BusConnect(GetEntityId());
        CampaignNotificationBus::Handler::BusConnect();
        AZ::TickBus::Handler::BusConnect();
        if (auto* campaign = CampaignInterface::Get())
        {
            OnRestoreWorldState(campaign->GetCurrentLevelName());
        }
        else
        {
            AZ_Error("DarkArisen", false, "Ship activated without the campaign system component.");
        }
    }

    void ShipVoyageComponent::Deactivate()
    {
        AZ::TickBus::Handler::BusDisconnect();
        CampaignNotificationBus::Handler::BusDisconnect();
        ShipRequestBus::Handler::BusDisconnect();
    }

    bool ShipVoyageComponent::TakeHelm()
    {
        m_voyage.SetJakeAtHelm(true);
        return m_voyage.IsJakeAtHelm();
    }

    void ShipVoyageComponent::LeaveHelm() { m_voyage.SetJakeAtHelm(false); }
    bool ShipVoyageComponent::SetThrottle(const float throttle) { return m_voyage.SetThrottle(throttle); }
    void ShipVoyageComponent::SetHelmCommandDegrees(const float headingDegrees) { m_voyage.SetHelmCommandDegrees(headingDegrees); }
    bool ShipVoyageComponent::Embark(const AZStd::string& characterId) { return m_voyage.Embark(characterId.c_str()); }
    bool ShipVoyageComponent::Disembark(const AZStd::string& characterId) { return m_voyage.Disembark(characterId.c_str()); }

    void ShipVoyageComponent::OnTick(const float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
    {
        // One weather source: the level's sea drives both the sails and the waves the hull rides.
        if (const OceanRequests* ocean = OceanInterface::Get())
        {
            const Core::SeaState& sea = ocean->GetSurface().GetSeaState();
            m_voyage.SetWind(static_cast<float>(sea.WindDirectionDegrees), static_cast<float>(sea.WindSpeedMetresPerSecond));
        }
        ApplyHeadingToEntity(m_voyage.Tick(deltaTime));
    }

    void ShipVoyageComponent::ApplyHeadingToEntity(const float travelledMetres) const
    {
        const float headingRadians = AZ::DegToRad(m_voyage.GetHeadingDegrees());
        const AZ::Vector3 forward(AZ::Sin(headingRadians), AZ::Cos(headingRadians), 0.0f);
        AZ::Vector3 position = AZ::Vector3::CreateZero();
        AZ::TransformBus::EventResult(position, GetEntityId(), &AZ::TransformBus::Events::GetWorldTranslation);
        AZ::Quaternion rotation = AZ::Quaternion::CreateRotationZ(-headingRadians);
        position += forward * travelledMetres;
        if (const OceanRequests* ocean = OceanInterface::Get())
        {
            // Heave, pitch (bow up) and roll (starboard down) from the shared sea; the hull filters chop.
            const Core::HullPose pose = Core::ComputeHullPose(ocean->GetSurface(), Core::HullShape{}, position.GetX(),
                position.GetY(), m_voyage.GetHeadingDegrees(), ocean->GetOceanTime());
            position.SetZ(m_waterlineHeightMetres + static_cast<float>(pose.HeaveMetres));
            rotation = rotation * AZ::Quaternion::CreateRotationX(static_cast<float>(pose.PitchRadians)) *
                AZ::Quaternion::CreateRotationY(static_cast<float>(pose.RollRadians));
        }
        AZ::TransformBus::Event(GetEntityId(), &AZ::TransformBus::Events::SetWorldRotationQuaternion, rotation);
        AZ::TransformBus::Event(GetEntityId(), &AZ::TransformBus::Events::SetWorldTranslation, position);
    }

    void ShipVoyageComponent::OnCaptureWorldState()
    {
        auto* campaign = CampaignInterface::Get();
        if (!campaign)
        {
            return;
        }
        AZ::Transform worldTM = AZ::Transform::CreateIdentity();
        AZ::TransformBus::EventResult(worldTM, GetEntityId(), &AZ::TransformBus::Events::GetWorldTM);
        Core::Transform transform;
        const AZ::Vector3 position = worldTM.GetTranslation();
        const AZ::Quaternion rotation = worldTM.GetRotation();
        transform.Translation = {position.GetX(), position.GetY(), position.GetZ()};
        transform.Rotation = {rotation.GetX(), rotation.GetY(), rotation.GetZ(), rotation.GetW()};
        if (!campaign->GetCampaign().CaptureShipVoyage(m_voyage.Capture(campaign->GetCurrentLevelName().c_str(), transform)))
        {
            AZ_Error("DarkArisen", false, "Ship snapshot rejected by campaign validation.");
        }
    }

    void ShipVoyageComponent::OnRestoreWorldState(const AZStd::string& levelName)
    {
        auto* campaign = CampaignInterface::Get();
        if (!campaign)
        {
            return;
        }
        const Core::CampaignState& state = campaign->GetCampaign().State();
        if (state.LaLiberacionVoyage.Valid && m_voyage.Restore(state.LaLiberacionVoyage, levelName.c_str()))
        {
            const Core::Transform& t = state.LaLiberacionVoyage.WorldTransform;
            AZ::TransformBus::Event(GetEntityId(), &AZ::TransformBus::Events::SetWorldTranslation,
                AZ::Vector3(static_cast<float>(t.Translation.X), static_cast<float>(t.Translation.Y), static_cast<float>(t.Translation.Z)));
        }
        if (state.WorldFacts.count(std::string(Core::Facts::LaLiberacionOwned)) != 0)
        {
            m_voyage.SetOwnedAndUnlocked();
        }
        ApplyHeadingToEntity(0.0f);
    }

    void ShipVoyageComponent::OnFactChanged(const AZStd::string& fact, const bool enabled)
    {
        if (enabled && fact == Core::Facts::LaLiberacionOwned.data())
        {
            m_voyage.SetOwnedAndUnlocked();
        }
    }
}
