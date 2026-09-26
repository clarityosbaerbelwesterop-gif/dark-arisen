#include "NavalCombatComponent.h"

#include <AzCore/Component/TransformBus.h>
#include <AzCore/Math/Transform.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/std/containers/vector.h>

namespace DarkArisen
{
    AZ_COMPONENT_IMPL(NavalCombatComponent, "NavalCombatComponent", NavalCombatComponentTypeId);

    void NavalCombatComponent::Reflect(AZ::ReflectContext* context)
    {
        auto* serializeContext = azrtti_cast<AZ::SerializeContext*>(context);
        if (!serializeContext)
        {
            return;
        }
        serializeContext->Class<NavalCombatComponent, AZ::Component>()
            ->Version(1)
            ->Field("PlayerAligned", &NavalCombatComponent::m_playerAligned)
            ->Field("FireDecisionSeconds", &NavalCombatComponent::m_fireDecisionSeconds);

        if (AZ::EditContext* editContext = serializeContext->GetEditContext())
        {
            editContext->Class<NavalCombatComponent>("Dark Arisen Naval Combat", "Broadsides and hull of one ship.")
                ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                ->Attribute(AZ::Edit::Attributes::Category, "Dark Arisen")
                ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"))
                ->DataElement(AZ::Edit::UIHandlers::Default, &NavalCombatComponent::m_playerAligned, "Player Aligned",
                    "On: La Liberacion (Jake fires at the helm). Off: hostile ship with its own fire loop.")
                ->DataElement(AZ::Edit::UIHandlers::Default, &NavalCombatComponent::m_fireDecisionSeconds, "Fire Decision (s)", "");
        }
    }

    void NavalCombatComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("DarkArisenNavalCombatService"));
    }

    void NavalCombatComponent::Activate()
    {
        m_combatant = Core::NavalCombatant(m_playerAligned ? Core::NavalCombatProfile::LaLiberacion() : Core::NavalCombatProfile::HostileEscort());
        m_fireDecisionRemaining = m_fireDecisionSeconds;
        m_sunkReported = false;
        NavalCombatRequestBus::Handler::BusConnect(GetEntityId());
        AZ::TickBus::Handler::BusConnect();
    }

    void NavalCombatComponent::Deactivate()
    {
        AZ::TickBus::Handler::BusDisconnect();
        NavalCombatRequestBus::Handler::BusDisconnect();
    }

    float NavalCombatComponent::FireBroadside(const int side)
    {
        if (side < 0 || side > static_cast<int>(Core::Broadside::Starboard) || m_combatant.IsSunk())
        {
            return 0.0f;
        }
        const auto broadside = static_cast<Core::Broadside>(side);
        if (!m_combatant.CanFire(broadside))
        {
            return 0.0f;
        }
        AZ::Transform self = AZ::Transform::CreateIdentity();
        AZ::TransformBus::EventResult(self, GetEntityId(), &AZ::TransformBus::Events::GetWorldTM);
        const AZ::Vector3 right = self.TransformVector(AZ::Vector3(0.0f, -1.0f, 0.0f));
        const AZ::Vector3 position = self.GetTranslation();

        AZStd::vector<NavalCombatRequests*> ships;
        std::vector<Core::NavalContact> contacts;
        NavalCombatRequestBus::EnumerateHandlers([&](NavalCombatRequests* ship)
        {
            if (ship == this)
            {
                return true;
            }
            AZ::Vector3 where = AZ::Vector3::CreateZero();
            AZ::TransformBus::EventResult(where, ship->GetShipEntityId(), &AZ::TransformBus::Events::GetWorldTranslation);
            ships.push_back(ship);
            contacts.push_back({where.GetX(), where.GetY(), where.GetZ(), ship->IsPlayerAligned(), ship->IsSunk()});
            return true;
        });
        const int target = Core::SelectBroadsideTarget({position.GetX(), position.GetY(), position.GetZ()}, {right.GetX(), right.GetY()},
            m_playerAligned, broadside, m_combatant.Profile().BroadsideRangeMetres, contacts);
        if (target < 0)
        {
            return 0.0f;
        }
        NavalCombatRequests* victim = ships[static_cast<size_t>(target)];
        const bool wasSunk = victim->IsSunk();
        const float damage = m_combatant.FireBroadside(broadside, victim->GetNavalCombatant());
        if (damage > 0.0f)
        {
            NavalCombatNotificationBus::Event(GetEntityId(), &NavalCombatNotifications::OnBroadsideFired, side, damage);
            if (!wasSunk && victim->IsSunk())
            {
                victim->ApplyHullDamage(0.0f); // lets the victim report its sinking immediately
            }
        }
        return damage;
    }

    void NavalCombatComponent::ApplyHullDamage(const float damage)
    {
        m_combatant.ApplyHullDamage(damage);
        if (m_combatant.IsSunk() && !m_sunkReported)
        {
            m_sunkReported = true;
            NavalCombatNotificationBus::Event(GetEntityId(), &NavalCombatNotifications::OnSunk);
        }
    }

    void NavalCombatComponent::OnTick(const float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
    {
        m_combatant.Tick(deltaTime);
        if (m_combatant.IsSunk())
        {
            ApplyHullDamage(0.0f);
            return;
        }
        if (m_playerAligned)
        {
            return; // Jake fires from the helm.
        }
        m_fireDecisionRemaining -= deltaTime;
        if (m_fireDecisionRemaining > 0.0f)
        {
            return;
        }
        m_fireDecisionRemaining = AZStd::max(0.25f, m_fireDecisionSeconds);
        if (FireBroadside(static_cast<int>(Core::Broadside::Port)) <= 0.0f)
        {
            FireBroadside(static_cast<int>(Core::Broadside::Starboard));
        }
    }
}
