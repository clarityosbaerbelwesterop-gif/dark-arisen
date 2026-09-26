#pragma once

#include <AzCore/Component/Component.h>
#include <AzCore/std/string/string.h>

namespace DarkArisen
{
    /**
     * Credits roll (L_Credits). The roll text is materialised from
     * ContentSource/Story/Credits/CreditsAuthority.json; nothing here invents a name. Fails closed:
     * the roll only starts once the campaign has reached the credits after The Wake After.
     */
    class CreditsComponent : public AZ::Component
    {
    public:
        AZ_COMPONENT_DECL(CreditsComponent);

        static void Reflect(AZ::ReflectContext* context);

        bool HasStarted() const { return m_started; }

    protected:
        void Activate() override;
        void Deactivate() override {}

    private:
        AZStd::string m_rollText;
        float m_allowSkipAfterSeconds = 8.0f;
        bool m_started = false;
    };
}
