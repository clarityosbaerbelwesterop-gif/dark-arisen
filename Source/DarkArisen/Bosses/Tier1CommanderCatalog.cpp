// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Bosses/Tier1CommanderCatalog.h"

namespace
{
FTier1CommanderDefinition Commander(
    const TCHAR* StableId,
    const TCHAR* DisplayName,
    const TCHAR* Colony,
    const TCHAR* Castle,
    const ETier1CommanderEncounterMode Mode,
    std::initializer_list<float> HealthFractions,
    std::initializer_list<const TCHAR*> PhaseIds,
    const TCHAR* ArenaMechanic,
    const TCHAR* AvoidedPath,
    const TCHAR* PrimaryReward)
{
    FTier1CommanderDefinition Entry;
    Entry.StableId = StableId;
    Entry.DisplayName = DisplayName;
    Entry.Colony = Colony;
    Entry.Castle = Castle;
    Entry.GoverningSource = TEXT("colonial war bosses.md");
    Entry.EncounterMode = Mode;
    for (const float Fraction : HealthFractions)
    {
        Entry.PhaseHealthFractions.Add(Fraction);
    }
    for (const TCHAR* PhaseId : PhaseIds)
    {
        Entry.PhaseIds.Add(FName(PhaseId));
    }
    Entry.ArenaMechanic = ArenaMechanic;
    Entry.AvoidedPath = AvoidedPath;
    Entry.PrimaryReward = PrimaryReward;
    return Entry;
}
}

TArray<FTier1CommanderDefinition> FTier1CommanderCatalog::BuildAll()
{
    TArray<FTier1CommanderDefinition> Result;
    Result.Reserve(RequiredCommanderCount);

    {
        FTier1CommanderDefinition Entry = Commander(
            TEXT("boss.herrera"), TEXT("General-Gobernador Alonso de Herrera"),
            TEXT("Puerto de la Corona"), TEXT("La Ciudadela de la Corona"),
            ETier1CommanderEncounterMode::HumanDuel,
            {0.70f, 0.35f},
            {TEXT("measure"), TEXT("pressure"), TEXT("arithmetic")},
            TEXT("Interlocking bastion fields of fire with adjacent garrison marksmen; Alliance Trust Tier 3 undercity entry starts in archive vaults."),
            TEXT("Barrio rising can take Corona while Herrera is elsewhere."),
            TEXT("Herrera rapier; Empire retaliation coordination degrades archipelago-wide; major Control swing."));
        Entry.bHasUndercityVariant = true;
        Result.Add(Entry);
    }

    Result.Add(Commander(
        TEXT("boss.reyes"), TEXT("General Baltasar Reyes"),
        TEXT("Nueva Esperanza"), TEXT("Fuerte Esperanza"),
        ETier1CommanderEncounterMode::HumanDuel,
        {0.60f, 0.25f},
        {TEXT("professional"), TEXT("arithmetic-arrives"), TEXT("overseer")},
        TEXT("Inward-facing gun positions progress toward firing on the villages while Jake duels Reyes."),
        TEXT("The uprising can take the fort before Jake reaches Reyes."),
        TEXT("Labor ledgers; Reyes sabre; Nueva Esperanza Control collapse.")));

    Result.Add(Commander(
        TEXT("boss.cruz"), TEXT("Comandante Isabel Cruz"),
        TEXT("Fuerte San Rafael"), TEXT("Fuerte San Rafael"),
        ETier1CommanderEncounterMode::HumanDuel,
        {0.65f, 0.30f},
        {TEXT("duty"), TEXT("list"), TEXT("1846")},
        TEXT("Shade galleries apply heat pressure; the cistern chamber is the authored cool recovery space."),
        TEXT("Cisterns and Mountain Paths can force formal surrender without the fight."),
        TEXT("1846 patrol reports and Cruz shrine-clearance list.")));

    {
        FTier1CommanderDefinition Entry = Commander(
            TEXT("boss.de_silva"), TEXT("Gobernador Francisco de Silva"),
            TEXT("Bahía Dorada"), TEXT("Castillo Dorado"),
            ETier1CommanderEncounterMode::NegotiationPursuit,
            {},
            {TEXT("opening-bid"), TEXT("sweetener"), TEXT("ledgers"), TEXT("truth")},
            TEXT("Captain Miguel Aldana is the actual combatant while de Silva retreats room-to-room and escalates offers."),
            TEXT("Bahía Dorada can be purchased; de Silva assists his own colony's fall for sufficient consideration."),
            TEXT("De Silva bribe ledgers and vault; Aldana may be recruitable if spared."));
        Entry.bDirectCombatant = false;
        Result.Add(Entry);
    }

    {
        FTier1CommanderDefinition Entry = Commander(
            TEXT("boss.vega"), TEXT("General Rodrigo Vega"),
            TEXT("Puesto del Norte"), TEXT("Puesto del Norte"),
            ETier1CommanderEncounterMode::HumanDuel,
            {0.60f, 0.25f},
            {TEXT("line"), TEXT("scaffolding"), TEXT("name")},
            TEXT("Icy wall walk and unfinished bastion scaffolding affect Jake; Vega's eleven winters make him authoredly sure-footed."),
            TEXT("Cut the Northern Run before the freeze; Jake may also leave after Vega refuses mercy."),
            TEXT("Vega correspondence paired with Thorne dispatches; cold-forged broadsword."));
        Entry.ResolutionRule = ETier1CommanderResolutionRule::SpareRefusedLeaveOnly;
        Result.Add(Entry);
    }

    {
        FTier1CommanderDefinition Entry = Commander(
            TEXT("boss.blackwood"), TEXT("Lord-Director Edmund Blackwood"),
            TEXT("New Coventry"), TEXT("Coventry House"),
            ETier1CommanderEncounterMode::VerticalPursuit,
            {},
            {TEXT("warehouse"), TEXT("counting-halls"), TEXT("offices"), TEXT("archives"), TEXT("roof-or-dock")},
            TEXT("Vertical pursuit through Coventry House; Regulators buy floors, clerks remain present, archive fire can destroy the Master Ledger."),
            TEXT("Drive New Coventry below profitability and shareholders recall Blackwood."),
            TEXT("Master Ledger; Regulator coordination office; Company treasury."));
        Entry.bDirectCombatant = false;
        Entry.bObjectiveCanBeDestroyedByFire = true;
        Result.Add(Entry);
    }

    {
        FTier1CommanderDefinition Entry = Commander(
            TEXT("boss.sterling"), TEXT("Admiral Katherine Sterling"),
            TEXT("Port Sterling"), TEXT("The Sterling Bastion"),
            ETier1CommanderEncounterMode::NavalEngagement,
            {},
            {TEXT("engagement"), TEXT("squadron"), TEXT("intractable")},
            TEXT("HMS Intractable, two escort frigates and harbour batteries fight as a coordinated naval problem; optional boarding creates the only melee variant."),
            TEXT("Alliance/common-cause route can preserve Sterling and her squadron; neutral standoff also avoids combat."),
            TEXT("Sterling protest correspondence; Bastion naval infrastructure; alliance route adds her squadron to endgame."));
        Entry.bHasBoardingVariant = true;
        Entry.bHasAllianceAvoidancePath = true;
        Result.Add(Entry);
    }

    Result.Add(Commander(
        TEXT("boss.ashcroft"), TEXT("General Percival Ashcroft"),
        TEXT("Blackwood Landing"), TEXT("Ashcroft Hall"),
        ETier1CommanderEncounterMode::HumanDuel,
        {0.65f, 0.30f},
        {TEXT("instrument"), TEXT("bailiffs-thin"), TEXT("astonishment")},
        TEXT("Bailiffs individually put down weapons and leave as villages rise, emptying the domestic arena around Ashcroft."),
        TEXT("Heist the four thousand original contracts; the colony ends without the encounter."),
        TEXT("Four thousand original contracts; season revenue; largest single castle LS gain.")));

    {
        FTier1CommanderDefinition Entry = Commander(
            TEXT("boss.thorne"), TEXT("Colonel Marcus Thorne"),
            TEXT("Fort Resolute"), TEXT("Fort Resolute"),
            ETier1CommanderEncounterMode::HumanDuel,
            {0.60f, 0.25f},
            {TEXT("command"), TEXT("garrison-falters"), TEXT("bewilderment")},
            TEXT("Cold and short troop rotations make Thorne's garrison deteriorate as the encounter continues while Thorne remains technically strong."),
            TEXT("Expose six years of dispatches for shareholder withdrawal, or complete the education path and have Thorne write the honest recommendation himself."),
            TEXT("Thorne dispatch copies paired with Vega correspondence; Northern Front Control swing."));
        Entry.bGarrisonWeakensOverTime = true;
        Result.Add(Entry);
    }

    return Result;
}

bool FTier1CommanderCatalog::TryGet(const FName StableId, FTier1CommanderDefinition& OutDefinition)
{
    if (StableId.IsNone())
    {
        return false;
    }
    for (const FTier1CommanderDefinition& Definition : BuildAll())
    {
        if (Definition.StableId == StableId)
        {
            OutDefinition = Definition;
            return true;
        }
    }
    return false;
}

bool FTier1CommanderCatalog::Validate(TArray<FString>& OutErrors)
{
    OutErrors.Reset();
    const TArray<FTier1CommanderDefinition> Definitions = BuildAll();
    if (Definitions.Num() != RequiredCommanderCount)
    {
        OutErrors.Add(FString::Printf(TEXT("Tier-1 commander catalog requires exactly %d entries; found %d."), RequiredCommanderCount, Definitions.Num()));
    }

    const TSet<FName> CanonicalIds = {
        TEXT("boss.herrera"), TEXT("boss.reyes"), TEXT("boss.cruz"), TEXT("boss.de_silva"),
        TEXT("boss.vega"), TEXT("boss.blackwood"), TEXT("boss.sterling"), TEXT("boss.ashcroft"), TEXT("boss.thorne")
    };

    TSet<FName> Seen;
    for (const FTier1CommanderDefinition& Definition : Definitions)
    {
        if (Definition.StableId.IsNone()
            || Definition.DisplayName.IsEmpty()
            || Definition.Colony.IsEmpty()
            || Definition.Castle.IsEmpty()
            || Definition.GoverningSource.IsEmpty()
            || Definition.ArenaMechanic.IsEmpty()
            || Definition.AvoidedPath.IsEmpty()
            || Definition.PrimaryReward.IsEmpty())
        {
            OutErrors.Add(TEXT("Every Tier-1 commander requires identity, location, source, arena, avoided path and reward."));
        }
        if (!CanonicalIds.Contains(Definition.StableId))
        {
            OutErrors.Add(FString::Printf(TEXT("Non-canonical Tier-1 commander id: %s"), *Definition.StableId.ToString()));
        }
        if (Seen.Contains(Definition.StableId))
        {
            OutErrors.Add(FString::Printf(TEXT("Duplicate Tier-1 commander id: %s"), *Definition.StableId.ToString()));
        }
        Seen.Add(Definition.StableId);

        if (!Definition.PhaseHealthFractions.IsEmpty())
        {
            if (Definition.PhaseHealthFractions.Num() != 2 || Definition.PhaseIds.Num() != 3)
            {
                OutErrors.Add(FString::Printf(TEXT("Health-band commander %s must use exactly two thresholds and three phases."), *Definition.StableId.ToString()));
            }
            else if (!(Definition.PhaseHealthFractions[0] > Definition.PhaseHealthFractions[1]
                && Definition.PhaseHealthFractions[0] < 1.0f
                && Definition.PhaseHealthFractions[1] > 0.0f))
            {
                OutErrors.Add(FString::Printf(TEXT("Commander %s has invalid descending phase thresholds."), *Definition.StableId.ToString()));
            }
        }
        else if (Definition.PhaseIds.IsEmpty())
        {
            OutErrors.Add(FString::Printf(TEXT("Non-health encounter %s still requires authored phase/stage identities."), *Definition.StableId.ToString()));
        }
    }

    if (Seen.Num() != CanonicalIds.Num())
    {
        OutErrors.Add(TEXT("Tier-1 commander set must contain exactly The Nine Who Hold."));
    }
    for (const FName CanonicalId : CanonicalIds)
    {
        if (!Seen.Contains(CanonicalId))
        {
            OutErrors.Add(FString::Printf(TEXT("Missing Tier-1 commander: %s"), *CanonicalId.ToString()));
        }
    }

    FTier1CommanderDefinition Vega;
    if (!TryGet(TEXT("boss.vega"), Vega) || Vega.ResolutionRule != ETier1CommanderResolutionRule::SpareRefusedLeaveOnly)
    {
        OutErrors.Add(TEXT("Vega must retain his refusal-to-be-spared resolution rule."));
    }

    return OutErrors.IsEmpty();
}
