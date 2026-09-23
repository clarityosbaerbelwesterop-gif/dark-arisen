#pragma once

#include <DarkArisen/DarkArisenTypeIds.h>

#include <AzCore/Component/ComponentBus.h>
#include <AzCore/RTTI/RTTI.h>
#include <AzCore/std/string/string.h>

namespace DarkArisen
{
    /** Helm and access requests for a physical ship entity (La Liberacion). */
    class ShipRequests : public AZ::ComponentBus
    {
    public:
        AZ_RTTI(ShipRequests, ShipRequestsTypeId);
        virtual ~ShipRequests() = default;

        virtual bool TakeHelm() = 0;
        virtual void LeaveHelm() = 0;
        virtual bool SetThrottle(float throttle) = 0;
        virtual void SetHelmCommandDegrees(float headingDegrees) = 0;
        virtual bool Embark(const AZStd::string& characterId) = 0;
        virtual bool Disembark(const AZStd::string& characterId) = 0;
        virtual float GetForwardSpeed() const = 0;
        virtual float GetHullIntegrity() const = 0;
    };

    using ShipRequestBus = AZ::EBus<ShipRequests>;
}
