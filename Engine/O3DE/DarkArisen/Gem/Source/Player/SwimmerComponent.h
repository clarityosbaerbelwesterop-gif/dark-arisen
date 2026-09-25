#pragma once

#include <DarkArisen/SwimBus.h>

#include <AzCore/Component/Component.h>
#include <AzCore/Component/TickBus.h>
#include <AzCore/std/containers/vector.h>

#include <DarkArisen/Core/Water.h>

namespace DarkArisen
{
    /**
     * Surface swimming, currents, buoyancy and breath for a character with a PhysX character
     * controller and a CombatantComponent (Unreal: MOVE_Swimming + UWaterBreathComponent).
     * While swimming, gravity from the PhysX character gameplay component is suspended and the
     * swimmer floats at the sampled sea surface; stamina drain and drowning go through the
     * combatant's Core models, so rally, death and respawn stay in one place.
     */
    class SwimmerComponent
        : public AZ::Component
        , public SwimRequestBus::Handler
        , protected AZ::TickBus::Handler
    {
    public:
        AZ_COMPONENT_DECL(SwimmerComponent);

        static void Reflect(AZ::ReflectContext* context);
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);

        // SwimRequests
        void OnEnteredWaterVolume(const WaterVolumeInfo& volume) override;
        void OnExitedWaterVolume(AZ::EntityId volume) override;
        void SetSwimIntent(const AZ::Vector2& intent, bool sprintHeld) override;
        bool IsSwimming() const override { return m_swim.IsSwimming(); }
        bool IsHeadSubmerged() const override { return m_breath.IsHeadSubmerged(); }
        float GetBreathFraction() const override { return m_breath.GetRemainingFraction(); }
        Core::SwimPace GetSwimPace() const override { return m_lastPace; }
        void SetBreathTier(Core::BreathTier tier) override { m_breath.SetTier(tier); }
        void ResetForRespawn() override;

    protected:
        void Activate() override;
        void Deactivate() override;
        void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;

    private:
        // Character proportions (metres above the controller base).
        float m_eyeHeight = 1.62f;
        /** Eyes this far above the surface when floating calmly. */
        float m_headClearance = 0.12f;
        /** Standing depth: base no deeper than this below the surface means the ground is wadeable. */
        float m_wadeDepth = 1.1f;
        /** Time constant with which buoyancy pulls the body back to floating height. */
        float m_buoyancyResponseSeconds = 0.35f;
        float m_maxVerticalSpeed = 2.0f;

        Core::SwimModel m_swim;
        Core::BreathModel m_breath;
        AZStd::vector<WaterVolumeInfo> m_volumes;
        WaterVolumeInfo m_lastVolume;
        bool m_lastExitWasShallow = false;
        AZ::Vector2 m_intent = AZ::Vector2::CreateZero();
        bool m_sprintHeld = false;
        Core::SwimPace m_lastPace = Core::SwimPace::Casual;

        float m_savedGravityMultiplier = 1.0f;
        float m_savedRegenMultiplier = 1.0f;

        void EnterWater();
        void LeaveWater();
        void ApplyCurrent();
        float SampleSurface(const AZ::Vector3& position) const;
    };
}
