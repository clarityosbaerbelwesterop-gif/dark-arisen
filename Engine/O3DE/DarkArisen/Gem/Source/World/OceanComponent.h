#pragma once

#include <DarkArisen/OceanBus.h>

#include <AzCore/Component/Component.h>
#include <AzCore/Component/TickBus.h>

#include <DarkArisen/Core/Ocean.h>

namespace DarkArisen
{
    /** Level entity that owns the sea state and the ocean clock. Exactly one per level. */
    class OceanComponent
        : public AZ::Component
        , public OceanRequests
        , protected AZ::TickBus::Handler
    {
    public:
        AZ_COMPONENT_DECL(OceanComponent);

        static void Reflect(AZ::ReflectContext* context);
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);

        // OceanRequests
        const Core::OceanSurface& GetSurface() const override { return m_surface; }
        double GetOceanTime() const override { return m_time; }
        void SetWeather(float windSpeedMetresPerSecond, float windDirectionDegrees, float exposure) override;

    protected:
        void Activate() override;
        void Deactivate() override;
        void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;

    private:
        float m_windSpeed = 6.0f;
        float m_windDirection = 90.0f;
        float m_exposure = 1.0f;
        Core::OceanSurface m_surface;
        double m_time = 0.0;
    };
}
