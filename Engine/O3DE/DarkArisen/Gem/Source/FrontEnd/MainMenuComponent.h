#pragma once

#include <AzCore/Component/Component.h>

namespace DarkArisen
{
    /** Placed in L_FrontEnd: while the level is loaded, the front end shows the main menu. */
    class MainMenuComponent : public AZ::Component
    {
    public:
        AZ_COMPONENT_DECL(MainMenuComponent);

        static void Reflect(AZ::ReflectContext* context);

    protected:
        void Activate() override;
        void Deactivate() override;
    };
}
