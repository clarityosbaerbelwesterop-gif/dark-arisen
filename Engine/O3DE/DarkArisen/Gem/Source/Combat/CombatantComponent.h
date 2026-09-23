#pragma once

#include <DarkArisen/CampaignBus.h>
#include <DarkArisen/CombatBus.h>

#include <AzCore/Component/Component.h>
#include <AzCore/Component/TickBus.h>
#include <AzCore/std/string/string.h>

#include <DarkArisen/Core/Combat.h>

namespace DarkArisen
{
    /**
     * Puts one engine-neutral Core::Combatant on an entity: Jake, boarders, Holders,
     * the dream figure or Draven. The model owns every rule; this component ticks it,
     * performs the melee sweep on the hit frame and routes the result to the target.
     * The player combatant ("character.jake") also persists its vitals and transform.
     */
    class CombatantComponent
        : public AZ::Component
        , protected CombatRequestBus::Handler
        , protected CampaignNotificationBus::Handler
        , protected AZ::TickBus::Handler
    {
    public:
        AZ_COMPONENT_DECL(CombatantComponent);

        static void Reflect(AZ::ReflectContext* context);
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);

    protected:
        void Activate() override;
        void Deactivate() override;

        // CombatRequests
        bool LightAttack() override;
        bool HeavyAttack() override;
        bool Parry() override;
        bool Dodge(bool backstep) override;
        void SetSprinting(bool sprinting) override;
        bool StartRache() override;
        float GetHealthFraction() const override;
        float GetStaminaFraction() const override;
        float GetPostureRemainingFraction() const override;
        bool IsDead() const override;
        AZStd::string GetCombatantId() const override { return m_combatantId; }
        AZ::EntityId GetCombatantEntityId() const override { return GetEntityId(); }
        Core::Combatant& GetCombatant() override { return m_combatant; }
        void RecoverAtCheckpoint(float healthFraction, float staminaFraction) override;

        // CampaignNotifications (player only)
        void OnCaptureWorldState() override;
        void OnRestoreWorldState(const AZStd::string& levelName) override;
        void OnFactChanged(const AZStd::string& fact, bool enabled) override;

        // TickBus
        void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;

    private:
        // Authored configuration.
        AZStd::string m_combatantId = "enemy.unnamed";
        float m_maxHealth = 200.0f;
        float m_maxPosture = 100.0f;
        float m_armorFraction = 0.0f;
        float m_physicalResistance = 0.0f;
        bool m_rallyEnabled = false;
        int m_weightClass = static_cast<int>(Core::WeaponWeightClass::Medium);
        /** Melee sweep (UCombatComponent: 70 cm start, 220 cm end, 42 cm radius). */
        float m_meleeStartMetres = 0.70f;
        float m_meleeEndMetres = 2.20f;
        float m_meleeRadiusMetres = 0.42f;
        /** Opening boarding encounter this combatant reports its defeat to (empty = none). */
        AZStd::string m_defeatEncounterId;

        Core::Combatant m_combatant;
        bool m_defeatReported = false;

        bool IsPlayer() const;
        void ApplyConfiguration();
        void ResolveMeleeSweep(Core::HitKind kind);
        void ReportDefeatIfNeeded();
        void ApplyTimeScale() const;
    };
}
