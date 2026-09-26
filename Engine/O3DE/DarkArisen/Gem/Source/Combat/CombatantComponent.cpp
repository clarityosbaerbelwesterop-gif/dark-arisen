#include "CombatantComponent.h"

#include <AzCore/Component/TransformBus.h>
#include <AzCore/Interface/Interface.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Time/ITime.h>
#include <AzCore/std/sort.h>
#include <AzFramework/Physics/Common/PhysicsSceneQueries.h>
#include <AzFramework/Physics/PhysicsScene.h>

#include <DarkArisen/Core/CampaignRuntime.h>
#include <DarkArisen/Core/EntityPolicy.h>
#include <DarkArisen/Core/Facts.h>
#include <DarkArisen/Core/OpeningRuntime.h>
#include <DarkArisen/Core/SaveRules.h>

#include <string>

namespace DarkArisen
{
    AZ_COMPONENT_IMPL(CombatantComponent, "CombatantComponent", CombatantComponentTypeId);

    void CombatantComponent::Reflect(AZ::ReflectContext* context)
    {
        auto* serializeContext = azrtti_cast<AZ::SerializeContext*>(context);
        if (!serializeContext)
        {
            return;
        }
        serializeContext->Class<CombatantComponent, AZ::Component>()
            ->Version(1)
            ->Field("CombatantId", &CombatantComponent::m_combatantId)
            ->Field("MaxHealth", &CombatantComponent::m_maxHealth)
            ->Field("MaxPosture", &CombatantComponent::m_maxPosture)
            ->Field("Armor", &CombatantComponent::m_armorFraction)
            ->Field("PhysicalResistance", &CombatantComponent::m_physicalResistance)
            ->Field("RallyEnabled", &CombatantComponent::m_rallyEnabled)
            ->Field("WeightClass", &CombatantComponent::m_weightClass)
            ->Field("MeleeStart", &CombatantComponent::m_meleeStartMetres)
            ->Field("MeleeEnd", &CombatantComponent::m_meleeEndMetres)
            ->Field("MeleeRadius", &CombatantComponent::m_meleeRadiusMetres)
            ->Field("DefeatEncounterId", &CombatantComponent::m_defeatEncounterId);

        if (AZ::EditContext* editContext = serializeContext->GetEditContext())
        {
            editContext->Class<CombatantComponent>("Dark Arisen Combatant", "Engine-neutral combat model on an entity.")
                ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                ->Attribute(AZ::Edit::Attributes::Category, "Dark Arisen")
                ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"))
                ->DataElement(AZ::Edit::UIHandlers::Default, &CombatantComponent::m_combatantId, "Combatant Id",
                    "Stable canon id, e.g. character.jake, enemy.boarder.harlow_deck, boss.draven_voss.")
                ->DataElement(AZ::Edit::UIHandlers::Default, &CombatantComponent::m_maxHealth, "Max Health", "")
                ->DataElement(AZ::Edit::UIHandlers::Default, &CombatantComponent::m_maxPosture, "Max Posture", "")
                ->DataElement(AZ::Edit::UIHandlers::Default, &CombatantComponent::m_armorFraction, "Armor", "0..0.9")
                ->DataElement(AZ::Edit::UIHandlers::Default, &CombatantComponent::m_physicalResistance, "Resistance", "0..0.9")
                ->DataElement(AZ::Edit::UIHandlers::Default, &CombatantComponent::m_rallyEnabled, "Rally", "")
                ->DataElement(AZ::Edit::UIHandlers::Default, &CombatantComponent::m_weightClass, "Weight Class",
                    "0 Light, 1 Medium, 2 Heavy, 3 Great, 4 Polearm")
                ->DataElement(AZ::Edit::UIHandlers::Default, &CombatantComponent::m_meleeStartMetres, "Melee Start (m)", "")
                ->DataElement(AZ::Edit::UIHandlers::Default, &CombatantComponent::m_meleeEndMetres, "Melee End (m)", "")
                ->DataElement(AZ::Edit::UIHandlers::Default, &CombatantComponent::m_meleeRadiusMetres, "Melee Radius (m)", "")
                ->DataElement(AZ::Edit::UIHandlers::Default, &CombatantComponent::m_defeatEncounterId, "Defeat Encounter",
                    "Opening boarding encounter that counts this combatant's defeat.");
        }
    }

    void CombatantComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("DarkArisenCombatantService"));
    }

    void CombatantComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("DarkArisenCombatantService"));
    }

    void CombatantComponent::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        required.push_back(AZ_CRC_CE("TransformService"));
    }

    bool CombatantComponent::IsPlayer() const
    {
        return m_combatantId == Core::EntityPolicy::JakeId.data();
    }

    void CombatantComponent::ApplyConfiguration()
    {
        m_combatant = Core::Combatant{};
        m_combatant.EntityId = std::string(m_combatantId.c_str());
        m_combatant.Health.MaxHealth = m_maxHealth;
        m_combatant.Health.CurrentHealth = m_maxHealth;
        m_combatant.Health.RallyEnabled = m_rallyEnabled;
        m_combatant.Combat.MaxPosture = m_maxPosture;
        m_combatant.Combat.ArmorFraction = m_armorFraction;
        m_combatant.Combat.PhysicalResistance = m_physicalResistance;
        m_combatant.Combat.SetWeightClass(static_cast<Core::WeaponWeightClass>(AZ::GetClamp(m_weightClass, 0, 4)));
    }

    void CombatantComponent::Activate()
    {
        if (m_combatantId.empty() || m_maxHealth <= 0.0f || m_maxPosture <= 0.0f)
        {
            AZ_Error("DarkArisen", false, "Combatant on entity %s has no valid id/health/posture; refusing to activate.",
                GetEntityId().ToString().c_str());
            return;
        }
        ApplyConfiguration();
        CombatRequestBus::Handler::BusConnect(GetEntityId());
        AZ::TickBus::Handler::BusConnect();
        if (IsPlayer())
        {
            CampaignNotificationBus::Handler::BusConnect();
            if (auto* campaign = CampaignInterface::Get())
            {
                m_combatant.Combat.RacheUnlocked = campaign->HasFact(Core::Facts::RacheUnlocked.data());
                OnRestoreWorldState(campaign->GetCurrentLevelName());
            }
            else
            {
                AZ_Error("DarkArisen", false, "Jake activated without the campaign system component.");
            }
        }
    }

    void CombatantComponent::Deactivate()
    {
        if (IsPlayer())
        {
            m_combatant.Combat.StopRache();
            ApplyTimeScale();
        }
        CampaignNotificationBus::Handler::BusDisconnect();
        AZ::TickBus::Handler::BusDisconnect();
        CombatRequestBus::Handler::BusDisconnect();
    }

    bool CombatantComponent::LightAttack() { return m_combatant.Combat.PerformLightAttack(m_combatant.Stamina); }
    bool CombatantComponent::HeavyAttack() { return m_combatant.Combat.PerformHeavyAttack(m_combatant.Stamina); }
    bool CombatantComponent::Parry() { return m_combatant.Combat.PerformParry(m_combatant.Stamina); }
    bool CombatantComponent::Dodge(const bool backstep) { return m_combatant.Combat.PerformDodge(m_combatant.Stamina, backstep); }
    void CombatantComponent::SetSprinting(const bool sprinting) { m_combatant.Stamina.SetSprinting(sprinting); }

    bool CombatantComponent::StartRache()
    {
        const bool started = m_combatant.Combat.StartRache();
        ApplyTimeScale();
        return started;
    }

    float CombatantComponent::GetHealthFraction() const { return m_combatant.Health.GetHealthFraction(); }

    float CombatantComponent::GetStaminaFraction() const
    {
        const Core::StaminaModel& stamina = m_combatant.Stamina;
        return stamina.MaxStamina > 0.0f ? stamina.CurrentStamina / stamina.MaxStamina : 0.0f;
    }

    float CombatantComponent::GetPostureRemainingFraction() const { return m_combatant.Combat.GetPostureRemainingFraction(); }
    bool CombatantComponent::IsDead() const { return m_combatant.Health.IsDead(); }

    void CombatantComponent::RecoverAtCheckpoint(const float healthFraction, const float staminaFraction)
    {
        m_combatant.Health.ResetForRespawn(healthFraction);
        m_combatant.Stamina.ResetForRespawn(staminaFraction);
        m_combatant.Combat.ResetAfterRespawn();
        ApplyTimeScale();
    }

    void CombatantComponent::ApplyTimeScale() const
    {
        if (auto* time = AZ::Interface<AZ::ITime>::Get(); time && IsPlayer())
        {
            time->SetSimulationTickScale(m_combatant.Combat.GetTimeScale());
        }
    }

    void CombatantComponent::OnTick(const float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
    {
        // deltaTime is simulation time; Rache drains in real time.
        const float scale = m_combatant.Combat.GetTimeScale();
        const float realDelta = scale > 0.0f ? deltaTime / scale : deltaTime;
        const Core::CombatTickEvents events = m_combatant.Combat.Tick(deltaTime, realDelta);
        m_combatant.Stamina.Tick(deltaTime);
        m_combatant.Health.Tick(deltaTime);
        if (events.MeleeHitDue)
        {
            ResolveMeleeSweep(*events.MeleeHitDue);
        }
        if (events.RacheEnded)
        {
            ApplyTimeScale();
        }
        ReportDefeatIfNeeded();
    }

    void CombatantComponent::ResolveMeleeSweep(const Core::HitKind kind)
    {
        auto* sceneInterface = AZ::Interface<AzPhysics::SceneInterface>::Get();
        if (!sceneInterface)
        {
            AZ_Error("DarkArisen", false, "Melee hit frame without a physics scene.");
            return;
        }
        AZ::Transform worldTM = AZ::Transform::CreateIdentity();
        AZ::TransformBus::EventResult(worldTM, GetEntityId(), &AZ::TransformBus::Events::GetWorldTM);
        const AZ::Vector3 forward = worldTM.GetBasisY().GetNormalizedSafe();
        AZ::Transform start = worldTM;
        start.SetTranslation(worldTM.GetTranslation() + forward * m_meleeStartMetres);

        AzPhysics::ShapeCastRequest request = AzPhysics::ShapeCastRequestHelpers::CreateSphereCastRequest(
            m_meleeRadiusMetres, start, forward, AZ::GetMax(0.0f, m_meleeEndMetres - m_meleeStartMetres));
        request.m_reportMultipleHits = true;
        const AzPhysics::SceneHandle sceneHandle = sceneInterface->GetSceneHandle(AzPhysics::DefaultPhysicsSceneName);
        AzPhysics::SceneQueryHits hits = sceneInterface->QueryScene(sceneHandle, &request);
        AZStd::sort(hits.m_hits.begin(), hits.m_hits.end(),
            [](const AzPhysics::SceneQueryHit& left, const AzPhysics::SceneQueryHit& right) { return left.m_distance < right.m_distance; });

        for (const AzPhysics::SceneQueryHit& hit : hits.m_hits)
        {
            if (!hit.m_entityId.IsValid() || hit.m_entityId == GetEntityId())
            {
                continue;
            }
            Core::Combatant* target = nullptr;
            CombatRequestBus::Event(hit.m_entityId, [&target](CombatRequests* handler) { target = &handler->GetCombatant(); });
            if (!target)
            {
                continue;
            }
            const Core::DamageResult result = Core::ResolveMeleeHit(m_combatant, *target, kind);
            if (result.Resolved)
            {
                CombatNotificationBus::Event(GetEntityId(), &CombatNotifications::OnMeleeResolved, hit.m_entityId, result.Deflected);
                return; // One authored contact per swing.
            }
        }
    }

    void CombatantComponent::ReportDefeatIfNeeded()
    {
        if (m_defeatReported || !m_combatant.Health.IsDead() || m_defeatEncounterId.empty())
        {
            return;
        }
        m_defeatReported = true;
        if (auto* campaign = CampaignInterface::Get())
        {
            if (!campaign->GetOpening().SignalBoarderDefeated(m_defeatEncounterId.c_str()))
            {
                AZ_Error("DarkArisen", false, "Defeat of %s rejected by encounter %s.", m_combatantId.c_str(),
                    m_defeatEncounterId.c_str());
            }
        }
    }

    void CombatantComponent::OnCaptureWorldState()
    {
        auto* campaign = CampaignInterface::Get();
        if (!campaign || m_combatant.Health.IsDead())
        {
            return; // Death never becomes the saved state; the checkpoint owns recovery.
        }
        AZ::Transform worldTM = AZ::Transform::CreateIdentity();
        AZ::TransformBus::EventResult(worldTM, GetEntityId(), &AZ::TransformBus::Events::GetWorldTM);
        Core::PlayerRuntimeSnapshot snapshot;
        snapshot.Valid = true;
        snapshot.SourceLevel = campaign->GetCurrentLevelName().c_str();
        const AZ::Vector3 position = worldTM.GetTranslation();
        const AZ::Quaternion rotation = worldTM.GetRotation();
        snapshot.WorldTransform.Translation = {position.GetX(), position.GetY(), position.GetZ()};
        snapshot.WorldTransform.Rotation = {rotation.GetX(), rotation.GetY(), rotation.GetZ(), rotation.GetW()};
        snapshot.WorldTransform.Scale = {worldTM.GetUniformScale(), worldTM.GetUniformScale(), worldTM.GetUniformScale()};
        snapshot.HealthFraction = AZ::GetClamp(GetHealthFraction(), 0.01f, 1.0f);
        snapshot.StaminaFraction = AZ::GetClamp(GetStaminaFraction(), 0.0f, 1.0f);
        if (!campaign->GetCampaign().CapturePlayerRuntime(snapshot))
        {
            AZ_Error("DarkArisen", false, "Player snapshot rejected by campaign validation.");
        }
    }

    void CombatantComponent::OnRestoreWorldState(const AZStd::string& levelName)
    {
        auto* campaign = CampaignInterface::Get();
        if (!campaign)
        {
            return;
        }
        const Core::PlayerRuntimeSnapshot& saved = campaign->GetCampaign().State().PlayerRuntime;
        if (!saved.Valid)
        {
            return; // Fresh game or legacy save: the authored spawn stands.
        }
        m_combatant.Health.ResetForRespawn(saved.HealthFraction);
        m_combatant.Stamina.ResetForRespawn(saved.StaminaFraction);
        if (Core::ShouldRestoreMapLocalTransform(saved.SourceLevel, levelName.c_str()))
        {
            const Core::Transform& t = saved.WorldTransform;
            const AZ::Transform worldTM = AZ::Transform::CreateFromQuaternionAndTranslation(
                AZ::Quaternion(static_cast<float>(t.Rotation.X), static_cast<float>(t.Rotation.Y),
                    static_cast<float>(t.Rotation.Z), static_cast<float>(t.Rotation.W)).GetNormalized(),
                AZ::Vector3(static_cast<float>(t.Translation.X), static_cast<float>(t.Translation.Y),
                    static_cast<float>(t.Translation.Z)));
            AZ::TransformBus::Event(GetEntityId(), &AZ::TransformBus::Events::SetWorldTM, worldTM);
        }
    }

    void CombatantComponent::OnFactChanged(const AZStd::string& fact, const bool enabled)
    {
        if (fact == Core::Facts::RacheUnlocked.data())
        {
            m_combatant.Combat.RacheUnlocked = enabled;
            if (enabled)
            {
                m_combatant.Combat.AddRacheFuel(100.0f); // The unlock primes a full meter (URacheStoryUnlock).
            }
        }
    }
}
