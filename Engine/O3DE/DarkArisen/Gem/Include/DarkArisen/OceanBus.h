#pragma once

#include <DarkArisen/DarkArisenTypeIds.h>

#include <AzCore/Interface/Interface.h>
#include <AzCore/RTTI/RTTI.h>

namespace DarkArisen::Core
{
    class OceanSurface;
}

namespace DarkArisen
{
    /**
     * The one sea of the loaded level. Gameplay (ships, swimming, boarding) and the Atom ocean
     * shader read the same Core::OceanSurface, so what the player sees is what the hull rides on.
     */
    class OceanRequests
    {
    public:
        AZ_RTTI(OceanRequests, OceanRequestsTypeId);
        virtual ~OceanRequests() = default;

        virtual const Core::OceanSurface& GetSurface() const = 0;
        /** Seconds of ocean time; the shader must use the same clock. */
        virtual double GetOceanTime() const = 0;
        virtual void SetWeather(float windSpeedMetresPerSecond, float windDirectionDegrees, float exposure) = 0;
    };

    using OceanInterface = AZ::Interface<OceanRequests>;
}
