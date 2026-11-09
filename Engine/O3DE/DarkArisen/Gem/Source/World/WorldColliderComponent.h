#pragma once

#include <DarkArisen/WorldBus.h>

#include <AzCore/Component/Component.h>
#include <AzCore/std/containers/unordered_map.h>
#include <AzFramework/Physics/Common/PhysicsTypes.h>

namespace DarkArisen
{
    /**
     * The open world's ground for physics: each tile whose plantings are resident (within the streamer's foliage
     * radius, so everything Jake can reach) gets a static PhysX heightfield from the same 5 m heights its mesh was
     * built from, with the mesh's diagonal, so what he stands on is what he sees. Removed when the tile leaves.
     */
    class WorldColliderComponent
        : public AZ::Component
        , protected WorldStreamingNotificationBus::Handler
    {
    public:
        AZ_COMPONENT_DECL(WorldColliderComponent);

        static void Reflect(AZ::ReflectContext* context);
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);

    protected:
        void Activate() override;
        void Deactivate() override;

        // WorldStreamingNotifications
        void OnFoliageLoaded(const WorldFoliage& foliage) override;
        void OnFoliageUnloaded(int tile) override;

    private:
        AzPhysics::SceneHandle m_scene = AzPhysics::InvalidSceneHandle;
        AZStd::unordered_map<int, AzPhysics::SimulatedBodyHandle> m_bodies;
    };
}
