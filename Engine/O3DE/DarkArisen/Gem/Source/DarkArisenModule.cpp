#include <AzCore/Memory/SystemAllocator.h>
#include <AzCore/Module/Module.h>

#include <DarkArisen/DarkArisenTypeIds.h>

#include "Combat/CombatantComponent.h"
#include "Combat/EnemyBrainComponent.h"
#include "Player/JakeInputComponent.h"
#include "Player/SwimmerComponent.h"
#include "Ships/ShipVoyageComponent.h"
#include "Story/CampaignSystemComponent.h"
#include "Story/StoryTriggerComponent.h"
#include "World/OceanComponent.h"
#include "World/WaterVolumeComponent.h"

namespace DarkArisen
{
    class DarkArisenModule : public AZ::Module
    {
    public:
        AZ_RTTI(DarkArisenModule, DarkArisenModuleTypeId, AZ::Module);
        AZ_CLASS_ALLOCATOR(DarkArisenModule, AZ::SystemAllocator);

        DarkArisenModule()
        {
            m_descriptors.insert(m_descriptors.end(), {
                CampaignSystemComponent::CreateDescriptor(),
                CombatantComponent::CreateDescriptor(),
                ShipVoyageComponent::CreateDescriptor(),
                StoryTriggerComponent::CreateDescriptor(),
                JakeInputComponent::CreateDescriptor(),
                EnemyBrainComponent::CreateDescriptor(),
                OceanComponent::CreateDescriptor(),
                SwimmerComponent::CreateDescriptor(),
                WaterVolumeComponent::CreateDescriptor(),
            });
        }

        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList{ azrtti_typeid<CampaignSystemComponent>() };
        }
    };
}

#if defined(O3DE_GEM_NAME)
AZ_DECLARE_MODULE_CLASS(AZ_JOIN(Gem_, O3DE_GEM_NAME), DarkArisen::DarkArisenModule)
#else
AZ_DECLARE_MODULE_CLASS(Gem_DarkArisen, DarkArisen::DarkArisenModule)
#endif
