#include "OceanComponent.h"

#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/SerializeContext.h>

namespace DarkArisen
{
    AZ_COMPONENT_IMPL(OceanComponent, "OceanComponent", OceanComponentTypeId);

    void OceanComponent::Reflect(AZ::ReflectContext* context)
    {
        auto* serializeContext = azrtti_cast<AZ::SerializeContext*>(context);
        if (!serializeContext)
        {
            return;
        }
        serializeContext->Class<OceanComponent, AZ::Component>()
            ->Version(1)
            ->Field("WindSpeed", &OceanComponent::m_windSpeed)
            ->Field("WindDirection", &OceanComponent::m_windDirection)
            ->Field("Exposure", &OceanComponent::m_exposure);
        if (AZ::EditContext* editContext = serializeContext->GetEditContext())
        {
            editContext->Class<OceanComponent>("Dark Arisen Ocean", "Sea state shared by gameplay and the ocean shader.")
                ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                ->Attribute(AZ::Edit::Attributes::Category, "Dark Arisen")
                ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Level"))
                ->DataElement(AZ::Edit::UIHandlers::Default, &OceanComponent::m_windSpeed, "Wind (m/s)", "")
                ->DataElement(AZ::Edit::UIHandlers::Default, &OceanComponent::m_windDirection, "Wind Direction", "Degrees, 0 = north (+Y)")
                ->DataElement(AZ::Edit::UIHandlers::Default, &OceanComponent::m_exposure, "Exposure", "0 sheltered harbour ... 1 open sea");
        }
    }

    void OceanComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("DarkArisenOceanService"));
    }

    void OceanComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("DarkArisenOceanService"));
    }

    void OceanComponent::SetWeather(const float windSpeedMetresPerSecond, const float windDirectionDegrees, const float exposure)
    {
        m_windSpeed = windSpeedMetresPerSecond;
        m_windDirection = windDirectionDegrees;
        m_exposure = exposure;
        Core::SeaState state;
        state.WindSpeedMetresPerSecond = m_windSpeed;
        state.WindDirectionDegrees = m_windDirection;
        state.Exposure = m_exposure;
        m_surface.SetSeaState(state);
    }

    void OceanComponent::Activate()
    {
        if (OceanInterface::Get() != nullptr)
        {
            AZ_Error("DarkArisen", false, "A second ocean on %s; each level owns exactly one sea.", GetEntityId().ToString().c_str());
            return;
        }
        SetWeather(m_windSpeed, m_windDirection, m_exposure);
        m_time = 0.0;
        OceanInterface::Register(this);
        AZ::TickBus::Handler::BusConnect();
    }

    void OceanComponent::Deactivate()
    {
        AZ::TickBus::Handler::BusDisconnect();
        if (OceanInterface::Get() == this)
        {
            OceanInterface::Unregister(this);
        }
    }

    void OceanComponent::OnTick(const float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
    {
        m_time += static_cast<double>(deltaTime);
    }
}
