#pragma once

#include <DarkArisen/CampaignBus.h>
#include <DarkArisen/ShipBus.h>

#include <AzCore/Component/Component.h>
#include <AzCore/Component/TickBus.h>

#include <DarkArisen/Core/ShipVoyage.h>

namespace DarkArisen
{
    /**
     * La Liberacion as a physical, continuously simulated vessel. Heading 0 is +Y (north),
     * clockwise positive. The ship keeps its commanded course when Jake leaves the helm and
     * moves only by its own simulated speed: no fast travel, no route teleport.
     */
    class ShipVoyageComponent
        : public AZ::Component
        , protected ShipRequestBus::Handler
        , protected CampaignNotificationBus::Handler
        , protected AZ::TickBus::Handler
    {
    public:
        AZ_COMPONENT_DECL(ShipVoyageComponent);

        static void Reflect(AZ::ReflectContext* context);
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);

    protected:
        void Activate() override;
        void Deactivate() override;

        // ShipRequests
        bool TakeHelm() override;
        void LeaveHelm() override;
        bool SetThrottle(float throttle) override;
        void SetHelmCommandDegrees(float headingDegrees) override;
        bool Embark(const AZStd::string& characterId) override;
        bool Disembark(const AZStd::string& characterId) override;
        float GetForwardSpeed() const override { return m_voyage.GetForwardSpeed(); }
        float GetHullIntegrity() const override { return m_voyage.GetHullIntegrity(); }

        // CampaignNotifications
        void OnCaptureWorldState() override;
        void OnRestoreWorldState(const AZStd::string& levelName) override;
        void OnFactChanged(const AZStd::string& fact, bool enabled) override;

        // TickBus
        void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;

    private:
        Core::ShipVoyage m_voyage;
        float m_windDirectionDegrees = 90.0f;
        float m_windStrengthMetresPerSecond = 6.0f;

        void ApplyHeadingToEntity(float travelledMetres) const;
    };
}
