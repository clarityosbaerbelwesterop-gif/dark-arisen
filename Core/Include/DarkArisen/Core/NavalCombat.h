#pragma once

#include <array>
#include <cstdint>
#include <vector>

namespace DarkArisen::Core
{
    enum class Broadside : std::uint8_t { Port, Starboard };

    /** Ship gunnery and hull data (UNavalCombatComponent defaults and AHostileNavalShip). */
    struct NavalCombatProfile
    {
        float MaxHull = 1200.0f;
        int CannonsPerSide = 8;
        float DamagePerCannon = 24.0f;
        float BroadsideRangeMetres = 90.0f;
        float ReloadSeconds = 7.5f;
        bool PlayerAligned = false;

        /** La Liberacion under Jake (UE component defaults). */
        static NavalCombatProfile LaLiberacion();
        /** Crimson escorts and gunships (UE AHostileNavalShip). */
        static NavalCombatProfile HostileEscort();
    };

    /** One ship's guns and hull. The engine positions ships and picks the target. */
    class NavalCombatant
    {
    public:
        explicit NavalCombatant(NavalCombatProfile Profile = NavalCombatProfile::LaLiberacion());

        const NavalCombatProfile& Profile() const { return Data; }
        bool IsSunk() const { return Hull <= 0.0f; }
        float GetHull() const { return Hull; }
        float GetHullFraction() const { return Data.MaxHull > 0.0f ? Hull / Data.MaxHull : 0.0f; }
        float GetReloadRemaining(Broadside Side) const;
        bool CanFire(Broadside Side) const;

        /**
         * Fires one broadside into Target (already chosen by SelectBroadsideTarget). Returns the
         * damage dealt, 0 when the side is reloading, the ship is sunk or the target is invalid.
         */
        float FireBroadside(Broadside Side, NavalCombatant& Target);
        void ApplyHullDamage(float Damage);
        /** Hull repair at a harbor or rest; never raises a sunk ship. */
        void Repair(float Amount);
        void Tick(float DeltaSeconds);

    private:
        NavalCombatProfile Data;
        float Hull = 0.0f;
        std::array<float, 2> Reload{0.0f, 0.0f};
    };

    /** What the engine knows about another ship for broadside targeting. */
    struct NavalContact
    {
        double X = 0.0;           // world metres
        double Y = 0.0;
        double Z = 0.0;
        bool PlayerAligned = false;
        bool Sunk = false;
    };

    /**
     * UNavalCombatComponent::FindBroadsideTarget: the nearest hostile, afloat ship inside range
     * that lies clearly to the given side (|cos| >= 0.25 against the ship's right vector).
     * Returns the index into Contacts, or -1.
     */
    int SelectBroadsideTarget(const std::array<double, 3>& Position, const std::array<double, 2>& RightVector, bool PlayerAligned,
        Broadside Side, float RangeMetres, const std::vector<NavalContact>& Contacts);
}
