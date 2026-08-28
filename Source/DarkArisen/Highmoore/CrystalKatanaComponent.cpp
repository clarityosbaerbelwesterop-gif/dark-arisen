// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Highmoore/CrystalKatanaComponent.h"

UCrystalKatanaComponent::UCrystalKatanaComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool UCrystalKatanaComponent::AcquireFromDefeatedGuardian(const bool bGuardianDefeated)
{
    if (!bGuardianDefeated || bAcquired)
    {
        return false;
    }

    bAcquired = true;
    return true;
}

float UCrystalKatanaComponent::ResolveFlatAttackStaminaCost(const float AuthoredBaseCost) const
{
    if (!bAcquired || !FMath::IsFinite(AuthoredBaseCost) || AuthoredBaseCost < 0.0f)
    {
        return 0.0f;
    }
    return AuthoredBaseCost;
}

void UCrystalKatanaComponent::SetWeaponInMotion(const bool bInMotion)
{
    if (bAcquired)
    {
        LightRead = bInMotion ? ECrystalKatanaLightRead::Moving : ECrystalKatanaLightRead::Resting;
    }
}
