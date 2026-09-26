#include "EnemyBrainComponent.h"

#include <DarkArisen/CampaignBus.h>
#include <DarkArisen/EnemyBus.h>

#include <AzCore/Component/TransformBus.h>
#include <AzCore/std/algorithm.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzFramework/Physics/CharacterBus.h>
#include <AzFramework/Physics/Common/PhysicsSceneQueries.h>
#include <AzFramework/Physics/PhysicsScene.h>

#include <DarkArisen/Core/Combat.h>
#include <DarkArisen/Core/EntityPolicy.h>

namespace DarkArisen
{
    AZ_COMPONENT_IMPL(EnemyBrainComponent, "EnemyBrainComponent", EnemyBrainComponentTypeId);

    void EnemyBrainComponent::Reflect(AZ::ReflectContext* context)
    {
        auto* serializeContext = azrtti_cast<AZ::SerializeContext*>(context);
        if (!serializeContext)
        {
            return;
        }
        serializeContext->Class<EnemyBrainComponent, AZ::Component>()
            ->Version(1)
            ->Field("Profile", &EnemyBrainComponent::m_profileKind)
            ->Field("HolderBossId", &EnemyBrainComponent::m_holderBossId)
            ->Field("HolderMissionId", &EnemyBrainComponent::m_holderMissionId)
            ->Field("EyeHeight", &EnemyBrainComponent::m_eyeHeightMetres)
            ->Field("OverrideResolution", &EnemyBrainComponent::m_overrideResolution)
            ->Field("OutcomeKey", &EnemyBrainComponent::m_outcomeKey)
            ->Field("OutcomeValue", &EnemyBrainComponent::m_outcomeValue)
            ->Field("CompleteMissionOnDefeat", &EnemyBrainComponent::m_completeMissionOnDefeat);

        if (AZ::EditContext* editContext = serializeContext->GetEditContext())
        {
            editContext->Enum<EnemyProfileKind>("Enemy Profile", "Authored enemy data from the engine-neutral core.")
                ->Value("Boarder", EnemyProfileKind::Boarder)
                ->Value("Holder (Nine Who Hold)", EnemyProfileKind::Holder)
                ->Value("Dream Ethan (Chapter 9 memory figure)", EnemyProfileKind::DreamEthan)
                ->Value("Draven Voss", EnemyProfileKind::DravenVoss);
            editContext->Class<EnemyBrainComponent>("Dark Arisen Enemy Brain", "Native enemy decision authority.")
                ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                ->Attribute(AZ::Edit::Attributes::Category, "Dark Arisen")
                ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"))
                ->DataElement(AZ::Edit::UIHandlers::ComboBox, &EnemyBrainComponent::m_profileKind, "Profile", "")
                ->DataElement(AZ::Edit::UIHandlers::Default, &EnemyBrainComponent::m_holderBossId, "Holder Boss Id",
                    "Holder only, e.g. boss.herrera")
                ->DataElement(AZ::Edit::UIHandlers::Default, &EnemyBrainComponent::m_holderMissionId, "Holder Mission Id",
                    "Holder only, e.g. Main.C04.03.HerrerasFall")
                ->DataElement(AZ::Edit::UIHandlers::Default, &EnemyBrainComponent::m_eyeHeightMetres, "Eye Height (m)", "")
                ->DataElement(AZ::Edit::UIHandlers::Default, &EnemyBrainComponent::m_overrideResolution, "Override Resolution",
                    "Use the story contract's outcome and completion instead of the profile's")
                ->DataElement(AZ::Edit::UIHandlers::Default, &EnemyBrainComponent::m_outcomeKey, "Outcome Key", "")
                ->DataElement(AZ::Edit::UIHandlers::Default, &EnemyBrainComponent::m_outcomeValue, "Outcome Value", "")
                ->DataElement(AZ::Edit::UIHandlers::Default, &EnemyBrainComponent::m_completeMissionOnDefeat,
                    "Complete Mission On Defeat", "");
        }
    }

    void EnemyBrainComponent::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        required.push_back(AZ_CRC_CE("DarkArisenCombatantService"));
        required.push_back(AZ_CRC_CE("TransformService"));
    }

    Core::EnemyProfile EnemyBrainComponent::BuildProfile() const
    {
        switch (m_profileKind)
        {
        case EnemyProfileKind::Holder:
            return Core::EnemyProfile::HolderBoss(m_holderBossId.c_str(), m_holderMissionId.c_str());
        case EnemyProfileKind::DreamEthan:
            return Core::EnemyProfile::DreamEthan();
        case EnemyProfileKind::DravenVoss:
            return Core::EnemyProfile::DravenVoss();
        case EnemyProfileKind::Boarder:
            break;
        }
        return Core::EnemyProfile::Boarder();
    }

    void EnemyBrainComponent::Activate()
    {
        Core::EnemyProfile profile = BuildProfile();
        if (m_overrideResolution)
        {
            profile.OutcomeKey = m_outcomeKey.c_str();
            profile.OutcomeValue = m_outcomeValue.c_str();
            profile.CompleteMissionOnDefeat = m_completeMissionOnDefeat;
        }
        std::string error;
        if (!profile.Validate(error))
        {
            AZ_Error("DarkArisen", false, "Enemy brain on %s rejected: %s", GetEntityId().ToString().c_str(), error.c_str());
            return;
        }
        m_brain = AZStd::make_unique<Core::EnemyBrain>(AZStd::move(profile));
        CombatRequestBus::Event(GetEntityId(), [this](CombatRequests* combat) { m_brain->Configure(combat->GetCombatant()); });
        m_defeatResolved = false;
        m_bombs.clear();
        AZ::TickBus::Handler::BusConnect();
        EnemyPopulationRequestBus::Handler::BusConnect();
        CombatNotificationBus::Handler::BusConnect(GetEntityId());
    }

    void EnemyBrainComponent::Deactivate()
    {
        CombatNotificationBus::Handler::BusDisconnect();
        EnemyPopulationRequestBus::Handler::BusDisconnect();
        AZ::TickBus::Handler::BusDisconnect();
        m_brain.reset();
    }

    bool EnemyBrainComponent::HasLineOfSight(const AZ::Vector3& from, const AZ::Vector3& to) const
    {
        auto* sceneInterface = AZ::Interface<AzPhysics::SceneInterface>::Get();
        if (!sceneInterface)
        {
            return true; // No physics scene: perception falls back to range only.
        }
        const AZ::Vector3 delta = to - from;
        AzPhysics::RayCastRequest request;
        request.m_start = from;
        request.m_distance = delta.GetLength();
        request.m_direction = delta.GetNormalizedSafe();
        const AZ::EntityId self = GetEntityId();
        const AZ::EntityId target = m_target;
        const AzPhysics::SceneQueryHits hits =
            sceneInterface->QueryScene(sceneInterface->GetSceneHandle(AzPhysics::DefaultPhysicsSceneName), &request);
        for (const AzPhysics::SceneQueryHit& hit : hits.m_hits)
        {
            if (hit.m_entityId != self)
            {
                return hit.m_entityId == target;
            }
        }
        return true;
    }

    Core::EnemyPerception EnemyBrainComponent::Perceive()
    {
        Core::EnemyPerception perception;
        if (!m_target.IsValid())
        {
            CombatRequestBus::EnumerateHandlers([this](CombatRequests* handler)
            {
                if (handler->GetCombatantId() == Core::EntityPolicy::JakeId.data())
                {
                    m_target = handler->GetCombatantEntityId();
                    return false;
                }
                return true;
            });
        }
        if (!m_target.IsValid())
        {
            return perception;
        }
        CombatRequestBus::Event(m_target, [&perception](CombatRequests* handler)
        {
            perception.TargetId = handler->GetCombatantId().c_str();
            perception.TargetDead = handler->IsDead();
            const Core::CombatState state = handler->GetCombatant().Combat.GetState();
            perception.TargetAttacking = state == Core::CombatState::LightAttacking || state == Core::CombatState::HeavyAttacking;
        });
        // TargetArmed stays at its default: Jake's sword has no dropped-weapon runtime yet, so a
        // disarm lands its damage and presentation but never leaves Draven waiting forever.
        AZ::Vector3 self = AZ::Vector3::CreateZero();
        AZ::Vector3 target = AZ::Vector3::CreateZero();
        AZ::TransformBus::EventResult(self, GetEntityId(), &AZ::TransformBus::Events::GetWorldTranslation);
        AZ::TransformBus::EventResult(target, m_target, &AZ::TransformBus::Events::GetWorldTranslation);
        perception.DistanceMetres = self.GetDistance(target);
        const AZ::Vector3 eye(0.0f, 0.0f, m_eyeHeightMetres);
        perception.TargetVisible = HasLineOfSight(self + eye, target + eye);
        return perception;
    }

    void EnemyBrainComponent::OnTick(const float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
    {
        Core::Combatant* self = nullptr;
        CombatRequestBus::Event(GetEntityId(), [&self](CombatRequests* handler) { self = &handler->GetCombatant(); });
        if (!self || !m_brain)
        {
            return;
        }
        TickPowderBombs(deltaTime);
        const Core::EnemyState previous = m_brain->GetState();
        const Core::EnemyIntent intent = m_brain->Tick(deltaTime, Perceive(), *self);
        const AZ::EntityId entity = GetEntityId();

        if (intent.StanceChanged)
        {
            EnemyNotificationBus::Event(entity, &EnemyNotifications::OnStanceChanged, static_cast<int>(*intent.StanceChanged));
        }
        if (!intent.MoveId.empty())
        {
            EnemyNotificationBus::Event(entity, &EnemyNotifications::OnMoveStarted, AZStd::string(intent.MoveId.c_str()));
        }
        if (intent.FeintCancelled)
        {
            EnemyNotificationBus::Event(entity, &EnemyNotifications::OnFeint);
        }

        if (intent.State != previous)
        {
            EnemyNotificationBus::Event(entity, &EnemyNotifications::OnEnemyStateChanged, static_cast<int>(intent.State));
        }
        if (intent.PhaseChanged)
        {
            EnemyNotificationBus::Event(entity, &EnemyNotifications::OnBossPhaseChanged, *intent.PhaseChanged);
        }
        if (intent.BeginTelegraph)
        {
            EnemyNotificationBus::Event(entity, &EnemyNotifications::OnTelegraph, static_cast<int>(*intent.BeginTelegraph));
        }
        if (intent.CommitAttack && intent.Delivery != Core::MoveDelivery::Melee)
        {
            EnemyNotificationBus::Event(entity, &EnemyNotifications::OnAttackCommitted, static_cast<int>(*intent.CommitAttack));
            Deliver(intent.Delivery, *intent.CommitAttack);
        }
        else if (intent.CommitAttack)
        {
            bool accepted = false;
            if (*intent.CommitAttack == Core::HitKind::Heavy)
                CombatRequestBus::EventResult(accepted, entity, &CombatRequests::HeavyAttack);
            else
                CombatRequestBus::EventResult(accepted, entity, &CombatRequests::LightAttack);
            if (accepted)
            {
                EnemyNotificationBus::Event(entity, &EnemyNotifications::OnAttackCommitted, static_cast<int>(*intent.CommitAttack));
            }
        }
        if (intent.FaceTarget && m_target.IsValid())
        {
            AZ::Vector3 from = AZ::Vector3::CreateZero();
            AZ::Vector3 to = AZ::Vector3::CreateZero();
            AZ::TransformBus::EventResult(from, entity, &AZ::TransformBus::Events::GetWorldTranslation);
            AZ::TransformBus::EventResult(to, m_target, &AZ::TransformBus::Events::GetWorldTranslation);
            const AZ::Vector3 flat(to.GetX() - from.GetX(), to.GetY() - from.GetY(), 0.0f);
            if (flat.GetLengthSq() > 0.0001f)
            {
                AZ::TransformBus::Event(entity, &AZ::TransformBus::Events::SetWorldRotationQuaternion,
                    AZ::Quaternion::CreateRotationZ(AZ::Atan2(-flat.GetX(), flat.GetY())));
                if (intent.MoveTowardTarget)
                {
                    Physics::CharacterRequestBus::Event(entity, &Physics::CharacterRequests::AddVelocityForTick,
                        flat.GetNormalized() * WalkSpeedMetresPerSecond);
                }
            }
        }
        if (intent.State == Core::EnemyState::Dead && !m_defeatResolved && !m_brain->Profile().BossId.empty())
        {
            m_defeatResolved = true;
            auto* campaign = CampaignInterface::Get();
            if (!campaign || !Core::ResolveBossDefeat(campaign->GetCampaign(), m_brain->Profile()))
            {
                AZ_Error("DarkArisen", false, "Boss %s defeated outside its active mission %s; story unchanged.",
                    m_brain->Profile().BossId.c_str(), m_brain->Profile().MissionId.c_str());
            }
        }
    }

    void EnemyBrainComponent::OnMeleeResolved([[maybe_unused]] const AZ::EntityId& target, const bool deflected)
    {
        if (m_brain)
        {
            Core::DamageResult result;
            result.Resolved = true;
            result.Deflected = deflected;
            m_brain->NotifyHitResolved(result);
        }
    }

    Core::Combatant* EnemyBrainComponent::TargetCombatant() const
    {
        Core::Combatant* target = nullptr;
        if (m_target.IsValid())
        {
            CombatRequestBus::Event(m_target, [&target](CombatRequests* handler) { target = &handler->GetCombatant(); });
        }
        return target;
    }

    AZ::Vector3 EnemyBrainComponent::Position(const AZ::EntityId entity) const
    {
        AZ::Vector3 position = AZ::Vector3::CreateZero();
        AZ::TransformBus::EventResult(position, entity, &AZ::TransformBus::Events::GetWorldTranslation);
        return position;
    }

    void EnemyBrainComponent::Deliver(const Core::MoveDelivery delivery, const Core::HitKind weight)
    {
        const AZ::EntityId entity = GetEntityId();
        Core::Combatant* self = nullptr;
        CombatRequestBus::Event(entity, [&self](CombatRequests* handler) { self = &handler->GetCombatant(); });
        Core::Combatant* target = TargetCombatant();
        if (!self || !target || !m_brain)
        {
            return;
        }
        const AZ::Vector3 from = Position(entity);
        const AZ::Vector3 to = Position(m_target);
        const float distance = from.GetDistance(to);
        switch (delivery)
        {
        case Core::MoveDelivery::PowderBomb:
            // Thrown at where Jake stands now; the marked zone gives him the fuse to leave it.
            m_bombs.push_back({ to, Core::EnemyProfile::PowderBombFuseSeconds });
            EnemyNotificationBus::Event(entity, &EnemyNotifications::OnPowderBombThrown, to, Core::EnemyProfile::PowderBombFuseSeconds);
            return;
        case Core::MoveDelivery::PistolShot:
        {
            const AZ::Vector3 eye(0.0f, 0.0f, m_eyeHeightMetres);
            bool hit = false;
            if (distance <= Core::EnemyProfile::PistolRangeMetres && HasLineOfSight(from + eye, to + eye))
            {
                const Core::DamageResult result = Core::ResolveBossDelivery(*self, *target, delivery, weight);
                m_brain->NotifyHitResolved(result);
                hit = result.Resolved && !result.Deflected && !result.Invulnerable;
            }
            EnemyNotificationBus::Event(entity, &EnemyNotifications::OnPistolFired, hit);
            return;
        }
        case Core::MoveDelivery::Grab:
        case Core::MoveDelivery::Disarm:
        {
            if (distance > m_brain->Profile().AttackRangeMetres)
            {
                return; // Jake stepped out of the clinch.
            }
            const Core::DamageResult result = Core::ResolveBossDelivery(*self, *target, delivery, weight);
            m_brain->NotifyHitResolved(result);
            if (delivery == Core::MoveDelivery::Disarm && result.Resolved && !result.Invulnerable)
            {
                EnemyNotificationBus::Event(entity, &EnemyNotifications::OnTargetDisarmed);
            }
            return;
        }
        case Core::MoveDelivery::Melee:
            return;
        }
    }

    void EnemyBrainComponent::TickPowderBombs(const float deltaTime)
    {
        for (PowderBomb& bomb : m_bombs)
        {
            bomb.FuseRemaining -= deltaTime;
            if (bomb.FuseRemaining > 0.0f)
            {
                continue;
            }
            Core::Combatant* self = nullptr;
            CombatRequestBus::Event(GetEntityId(), [&self](CombatRequests* handler) { self = &handler->GetCombatant(); });
            Core::Combatant* target = TargetCombatant();
            if (self && target && Position(m_target).GetDistance(bomb.Position) <= Core::EnemyProfile::PowderBombRadiusMetres)
            {
                Core::ResolveBossDelivery(*self, *target, Core::MoveDelivery::PowderBomb, Core::HitKind::Heavy);
            }
            EnemyNotificationBus::Event(GetEntityId(), &EnemyNotifications::OnPowderBombDetonated, bomb.Position);
        }
        m_bombs.erase(AZStd::remove_if(m_bombs.begin(), m_bombs.end(), [](const PowderBomb& bomb) { return bomb.FuseRemaining <= 0.0f; }),
            m_bombs.end());
    }

    void EnemyBrainComponent::CountLivingRankAndFile(int& count) const
    {
        if (!m_brain || !m_brain->Profile().BossId.empty())
        {
            return;
        }
        bool dead = true;
        CombatRequestBus::Event(GetEntityId(), [&dead](CombatRequests* combat) { dead = combat->IsDead(); });
        if (!dead)
        {
            ++count;
        }
    }
}
