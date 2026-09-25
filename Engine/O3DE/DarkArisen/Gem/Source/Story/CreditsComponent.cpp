#include "CreditsComponent.h"

#include <DarkArisen/CampaignBus.h>
#include <DarkArisen/DarkArisenTypeIds.h>
#include <DarkArisen/PresentationBus.h>

#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/SerializeContext.h>

#include <DarkArisen/Core/Facts.h>

namespace DarkArisen
{
    AZ_COMPONENT_IMPL(CreditsComponent, "CreditsComponent", CreditsComponentTypeId);

    void CreditsComponent::Reflect(AZ::ReflectContext* context)
    {
        auto* serializeContext = azrtti_cast<AZ::SerializeContext*>(context);
        if (!serializeContext)
        {
            return;
        }
        serializeContext->Class<CreditsComponent, AZ::Component>()
            ->Version(1)
            ->Field("RollText", &CreditsComponent::m_rollText)
            ->Field("AllowSkipAfterSeconds", &CreditsComponent::m_allowSkipAfterSeconds);

        if (AZ::EditContext* editContext = serializeContext->GetEditContext())
        {
            editContext->Class<CreditsComponent>("Dark Arisen Credits", "Credits roll from CreditsAuthority.json.")
                ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                ->Attribute(AZ::Edit::Attributes::Category, "Dark Arisen")
                ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"))
                ->DataElement(AZ::Edit::UIHandlers::MultiLineEdit, &CreditsComponent::m_rollText, "Roll Text", "")
                ->DataElement(AZ::Edit::UIHandlers::Default, &CreditsComponent::m_allowSkipAfterSeconds, "Allow Skip After (s)", "");
        }
    }

    void CreditsComponent::Activate()
    {
        m_started = false;
        auto* campaign = CampaignInterface::Get();
        if (!campaign || !campaign->HasFact(AZStd::string(Core::Facts::CreditsReachable.data(), Core::Facts::CreditsReachable.size())))
        {
            AZ_Error("DarkArisen", false, "Credits level loaded before The Wake After completed; the roll does not start.");
            return;
        }
        if (m_rollText.empty())
        {
            AZ_Error("DarkArisen", false, "Credits roll text is empty; materialise CreditsAuthority.json.");
            return;
        }
        m_started = true;
        PresentationNotificationBus::Broadcast(&PresentationNotifications::OnCreditsStarted, m_rollText, m_allowSkipAfterSeconds);
    }
}
