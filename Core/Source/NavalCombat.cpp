#include "DarkArisen/Core/NavalCombat.h"

#include <algorithm>
#include <cmath>

namespace DarkArisen::Core
{
    NavalCombatProfile NavalCombatProfile::LaLiberacion()
    {
        NavalCombatProfile Profile;
        Profile.PlayerAligned = true;
        return Profile;
    }

    NavalCombatProfile NavalCombatProfile::HostileEscort()
    {
        NavalCombatProfile Profile;
        Profile.MaxHull = 760.0f;
        Profile.CannonsPerSide = 6;
        Profile.DamagePerCannon = 18.0f;
        Profile.ReloadSeconds = 9.0f;
        Profile.PlayerAligned = false;
        return Profile;
    }

    NavalCombatant::NavalCombatant(NavalCombatProfile Profile)
        : Data(Profile)
        , Hull(std::max(0.0f, Profile.MaxHull))
    {
    }

    float NavalCombatant::GetReloadRemaining(const Broadside Side) const
    {
        return Reload[static_cast<std::size_t>(Side)];
    }

    bool NavalCombatant::CanFire(const Broadside Side) const
    {
        return !IsSunk() && GetReloadRemaining(Side) <= 0.0f;
    }

    float NavalCombatant::FireBroadside(const Broadside Side, NavalCombatant& Target)
    {
        if (!CanFire(Side) || &Target == this || Target.IsSunk() || Target.Data.PlayerAligned == Data.PlayerAligned)
        {
            return 0.0f;
        }
        const float Damage = static_cast<float>(std::max(1, Data.CannonsPerSide)) * std::max(0.0f, Data.DamagePerCannon);
        Target.ApplyHullDamage(Damage);
        Reload[static_cast<std::size_t>(Side)] = Data.ReloadSeconds;
        return Damage;
    }

    void NavalCombatant::ApplyHullDamage(const float Damage)
    {
        if (!(Damage > 0.0f) || IsSunk())
        {
            return;
        }
        Hull = std::clamp(Hull - Damage, 0.0f, Data.MaxHull);
    }

    void NavalCombatant::Repair(const float Amount)
    {
        if (!(Amount > 0.0f) || IsSunk())
        {
            return;
        }
        Hull = std::min(Data.MaxHull, Hull + Amount);
    }

    void NavalCombatant::Tick(const float DeltaSeconds)
    {
        if (!(DeltaSeconds > 0.0f))
        {
            return;
        }
        for (float& Remaining : Reload)
        {
            Remaining = std::max(0.0f, Remaining - DeltaSeconds);
        }
    }

    int SelectBroadsideTarget(const std::array<double, 3>& Position, const std::array<double, 2>& RightVector, const bool PlayerAligned,
        const Broadside Side, const float RangeMetres, const std::vector<NavalContact>& Contacts)
    {
        const double RightLength = std::hypot(RightVector[0], RightVector[1]);
        if (!(RightLength > 1e-9) || !(RangeMetres > 0.0f))
        {
            return -1;
        }
        int Best = -1;
        double BestSquared = static_cast<double>(RangeMetres) * RangeMetres;
        for (std::size_t Index = 0; Index < Contacts.size(); ++Index)
        {
            const NavalContact& Contact = Contacts[Index];
            if (Contact.Sunk || Contact.PlayerAligned == PlayerAligned)
            {
                continue;
            }
            const double Dx = Contact.X - Position[0];
            const double Dy = Contact.Y - Position[1];
            const double Dz = Contact.Z - Position[2];
            const double Flat = std::hypot(Dx, Dy);
            if (!(Flat > 1e-9))
            {
                continue;
            }
            const double Dot = (Dx * RightVector[0] + Dy * RightVector[1]) / (Flat * RightLength);
            if ((Side == Broadside::Port && Dot > -0.25) || (Side == Broadside::Starboard && Dot < 0.25))
            {
                continue;
            }
            const double Squared = Dx * Dx + Dy * Dy + Dz * Dz;
            if (Squared < BestSquared)
            {
                BestSquared = Squared;
                Best = static_cast<int>(Index);
            }
        }
        return Best;
    }
}
