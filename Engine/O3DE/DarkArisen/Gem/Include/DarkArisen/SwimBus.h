#pragma once

#include <DarkArisen/DarkArisenTypeIds.h>

#include <AzCore/Component/ComponentBus.h>
#include <AzCore/Component/EntityId.h>
#include <AzCore/Math/Vector2.h>
#include <AzCore/Math/Vector3.h>
#include <AzCore/RTTI/RTTI.h>

#include <DarkArisen/Core/Water.h>

namespace DarkArisen
{
    /** What a water volume tells the swimmer that entered it. */
    struct WaterVolumeInfo
    {
        AZ::EntityId m_volume;
        /** m/s^2, world. ContentSource authors cm/s^2; see Core::CurrentFromCentimetres. */
        AZ::Vector3 m_currentAcceleration = AZ::Vector3::CreateZero();
        /** Sample the level's OceanComponent for the surface; otherwise use m_surfaceHeight. */
        bool m_useOceanSurface = true;
        float m_surfaceHeight = 0.0f;
        /** Leaving this volume towards wadeable ground returns the swimmer to walking. */
        bool m_shallowExit = false;
    };

    /** Swimming and breath for one character. Addressed by the swimmer's EntityId. */
    class SwimRequests : public AZ::ComponentBus
    {
    public:
        AZ_RTTI(SwimRequests, SwimRequestsTypeId);
        virtual ~SwimRequests() = default;

        virtual void OnEnteredWaterVolume(const WaterVolumeInfo& volume) = 0;
        virtual void OnExitedWaterVolume(AZ::EntityId volume) = 0;

        /** Stick direction in world XY (length <= 1) and whether sprint is held. */
        virtual void SetSwimIntent(const AZ::Vector2& intent, bool sprintHeld) = 0;

        virtual bool IsSwimming() const = 0;
        virtual bool IsHeadSubmerged() const = 0;
        virtual float GetBreathFraction() const = 0;
        virtual Core::SwimPace GetSwimPace() const = 0;
        virtual void SetBreathTier(Core::BreathTier tier) = 0;

        /** Checkpoint respawn: out of the water, full breath (Unreal RestoreAtCheckpoint parity). */
        virtual void ResetForRespawn() = 0;
    };

    using SwimRequestBus = AZ::EBus<SwimRequests>;
}
