#pragma once

#include <DarkArisen/DarkArisenTypeIds.h>

#include <AzCore/Component/ComponentBus.h>
#include <AzCore/Component/EntityId.h>
#include <AzCore/EBus/EBus.h>
#include <AzCore/Math/Vector2.h>
#include <AzCore/RTTI/RTTI.h>

namespace DarkArisen
{
    /** The player's follow camera (one per level). Broadcast. */
    class CameraRigRequests : public AZ::EBusTraits
    {
    public:
        AZ_RTTI(CameraRigRequests, CameraRigRequestsTypeId);
        static constexpr AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
        virtual ~CameraRigRequests() = default;

        virtual void AddMouseLook(float deltaXPixels, float deltaYPixels) = 0;
        /** Right stick position, applied every tick until changed. */
        virtual void SetStickLook(const AZ::Vector2& stick) = 0;
        virtual float GetYawDegrees() const = 0;
        /** Ground-plane unit vectors for camera-relative movement. */
        virtual AZ::Vector2 GetForward() const = 0;
        virtual AZ::Vector2 GetRight() const = 0;
    };

    using CameraRigRequestBus = AZ::EBus<CameraRigRequests>;

    /** Lock-on for a combatant (the player). Addressed by entity id. */
    class LockOnRequests : public AZ::ComponentBus
    {
    public:
        AZ_RTTI(LockOnRequests, LockOnRequestsTypeId);
        virtual ~LockOnRequests() = default;

        /** Locks the best target in front, or releases the current one. Returns whether locked after. */
        virtual bool ToggleLockOn() = 0;
        virtual void ReleaseLockOn() = 0;
        virtual bool IsLockedOn() const = 0;
        virtual AZ::EntityId GetLockTarget() const = 0;
    };

    using LockOnRequestBus = AZ::EBus<LockOnRequests>;
}
