#include "MainMenuComponent.h"

#include <DarkArisen/FrontEndBus.h>

#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/SerializeContext.h>

namespace DarkArisen
{
    AZ_COMPONENT_IMPL(MainMenuComponent, "MainMenuComponent", MainMenuComponentTypeId);

    void MainMenuComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<MainMenuComponent, AZ::Component>()->Version(1);
            if (auto* editContext = serialize->GetEditContext())
            {
                editContext->Class<MainMenuComponent>("Main Menu", "Shows the Dark Arisen main menu while this level is loaded.")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Category, "Dark Arisen")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"));
            }
        }
    }

    void MainMenuComponent::Activate()
    {
        FrontEndRequestBus::Broadcast(&FrontEndRequests::OpenMainMenu);
    }

    void MainMenuComponent::Deactivate()
    {
        FrontEndRequestBus::Broadcast(&FrontEndRequests::CloseMainMenu);
    }
}
