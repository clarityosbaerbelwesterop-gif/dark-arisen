#pragma once

#include <DarkArisen/CombatBus.h>
#include <DarkArisen/EnemyBus.h>

#include <DarkArisen/AuthoringEnums.h>

#include <AzCore/Component/Component.h>
#include <AzCore/Component/TickBus.h>
#include <AzCore/Math/Vector3.h>
#include <AzCore/RTTI/TypeInfo.h>
#include <AzCore/std/smart_ptr/unique_ptr.h>
#include <AzCore/std/string/string.h>

#include <DarkArisen/Core/EnemyBrain.h>

namespace DarkArisen
{
    /**
     * Drives an enemy's CombatantComponent with the engine-neutral Core::EnemyBrain.
     * Perception: distance to Jake plus a line-of-sight ray. Movement: PhysX character velocity.
     * Attacks: telegraph notification, then CombatRequestBus. Boss defeat resolves its mission.
     */
    class EnemyBrainComponent
        : public AZ::Component
        , protected AZ::TickBus::Handler
        , protected EnemyPopulationRequestBus::Handler
    {
    public:
        AZ_COMPONENT_DECL(EnemyBrainComponent);

        static void Reflect(AZ::ReflectContext* context);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);

        /** UE ADuelingEnemyCharacter MaxWalkSpeed 280 cm/s. */
        static constexpr float WalkSpeedMetresPerSecond = 2.8f;

    protected:
        void Activate() override;
        void Deactivate() override;
        void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;
        void CountLivingRankAndFile(int& count) const override;

    private:
        EnemyProfileKind m_profileKind = EnemyProfileKind::Boarder;
        AZStd::string m_holderBossId;
        AZStd::string m_holderMissionId;
        float m_eyeHeightMetres = 1.6f;
        /** Story contracts author each Holder's resolution; off keeps the Core profile's. */
        bool m_overrideResolution = false;
        AZStd::string m_outcomeKey;
        AZStd::string m_outcomeValue;
        bool m_completeMissionOnDefeat = true;

        AZStd::unique_ptr<Core::EnemyBrain> m_brain;
        AZ::EntityId m_target;
        bool m_defeatResolved = false;

        Core::EnemyProfile BuildProfile() const;
        Core::EnemyPerception Perceive();
        bool HasLineOfSight(const AZ::Vector3& from, const AZ::Vector3& to) const;
    };
}

namespace AZ
{
    AZ_TYPE_INFO_SPECIALIZE(DarkArisen::EnemyProfileKind, "{9B3F6D12-5A4C-4E8B-8D71-2C6E0F9A4B57}");
}
