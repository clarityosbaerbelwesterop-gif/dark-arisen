// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Production/Tier1BossVisualReadinessCatalog.h"

namespace
{
FTier1BossVisualReadiness Tier1(
    const TCHAR* StableId,
    const TCHAR* DisplayName,
    const TCHAR* Source,
    const TCHAR* AgeRole,
    const TCHAR* Weapon,
    const TCHAR* Performance,
    const TCHAR* Arena,
    const TCHAR* Missing)
{
    FTier1BossVisualReadiness Result;
    Result.BossStableId = StableId;
    Result.DisplayName = DisplayName;
    Result.GoverningSource = Source;
    Result.AuthoredAgeRole = AgeRole;
    Result.AuthoredWeaponSilhouette = Weapon;
    Result.AuthoredPerformanceRead = Performance;
    Result.AuthoredArenaRead = Arena;
    Result.MissingPhysicalAuthority = Missing;
    Result.bCurrentTier1Authority = true;
    Result.bProviderFullLookReady = false;
    return Result;
}
}

TArray<FTier1BossVisualReadiness> FTier1BossVisualReadinessCatalog::BuildAll()
{
    return {
        Tier1(
            TEXT("boss.herrera"),
            TEXT("General-Gobernador Alonso de Herrera"),
            TEXT("Docs/M7_TIER1_BOSS_REGISTER.md; colonial war bosses.md Section 4"),
            TEXT("54; twenty-six years of colonial service; apex human boss of Phase 7."),
            TEXT("Imperial officer rapier and main-gauche, later one cavalry-pistol shot; light cuirass under a uniform coat."),
            TEXT("Precise, patient and punishing; conservative assessment becomes pressure and finally silent maximum aggression."),
            TEXT("Interlocking bastion faces/curtain walls with marksmen, or cramped archive-vault undercity opening if Alliance work changes the approach."),
            TEXT("Final height, build, face, hair, eyes, skin, uniform cut/colors beyond the generic Imperial coat/cuirass language, scars and personal visual identifiers are not authored by the current Tier-1 source.")),
        Tier1(
            TEXT("boss.reyes"),
            TEXT("General Baltasar Reyes"),
            TEXT("Docs/M7_TIER1_BOSS_REGISTER.md; colonial war bosses.md Section 5"),
            TEXT("47; risen from nothing; administrator who has held a hostile colony for twenty years."),
            TEXT("Plain well-maintained cavalry sabre and a brace of pistols."),
            TEXT("Practical and brutal; initially defensive to buy time for gun crews, then increasingly desperate and ugly as the fort falls."),
            TEXT("Inner yard, firing steps and inward-facing gun positions during a race to disable artillery aimed at the villages."),
            TEXT("Current authority does not define height, build, face, hair, eyes, skin, scars, exact uniform/armor, insignia treatment or distinctive personal silhouette.")),
        Tier1(
            TEXT("boss.cruz"),
            TEXT("Comandante Isabel Cruz"),
            TEXT("Docs/M7_TIER1_BOSS_REGISTER.md; colonial war bosses.md Section 6"),
            TEXT("43; long-serving mining-post commander who twice refused orders she considered atrocities."),
            TEXT("Plain worn infantry sword; shield added in Phase 2; the only commander whose doctrine is explicitly defensive."),
            TEXT("Disciplined, economical, deliberately gives ground and does not press every advantage."),
            TEXT("Close hot shade galleries leading to the cool cistern complex; heat pressure defines the visual/combat rhythm."),
            TEXT("Current authority does not define height, body type, face, hair, eyes, skin, scars, uniform silhouette, shield design, ethnicity-specific visual treatment or personal accessories.")),
        Tier1(
            TEXT("boss.de_silva"),
            TEXT("Gobernador Francisco de Silva"),
            TEXT("Docs/M7_TIER1_BOSS_REGISTER.md; colonial war bosses.md Section 7"),
            TEXT("61; warm, funny, cultured, systemically corrupt and a coward; the boss who tries hardest not to be one."),
            TEXT("No earnest personal weapon is authored; Captain Miguel Aldana is the actual fighter while de Silva negotiates and flees."),
            TEXT("Transactional performance: escalating offers and movement between rooms until cornered in the gardens."),
            TEXT("Reception wing, ballroom, terrace and seaward gardens with orchids and roses."),
            TEXT("Current authority does not define height, build, face, hair, eyes, skin, clothing, jewelry, grooming, scars or a final gubernatorial silhouette.")),
        Tier1(
            TEXT("boss.vega"),
            TEXT("General Rodrigo Vega"),
            TEXT("Docs/M7_TIER1_BOSS_REGISTER.md; colonial war bosses.md Section 8"),
            TEXT("58; eleven years abandoned on the northern line and obsessed with not being the general who lost it."),
            TEXT("Heavy two-handed Imperial broadsword, cold-climate pattern, practised with for more than a decade."),
            TEXT("Relentless, heavy and without self-preservation; stands ground like a wall and is uniquely experienced on the ice."),
            TEXT("Winter wall walk and skeletal unfinished bastion with frozen scaffolding; Vega remains sure-footed where Jake slips."),
            TEXT("Current authority does not define height, build, face, hair, eyes, skin, cold-weather uniform, beard, scars, gloves/boots or personal identifiers beyond the broadsword.")),
        Tier1(
            TEXT("boss.blackwood"),
            TEXT("Lord-Director Edmund Blackwood"),
            TEXT("Docs/M7_TIER1_BOSS_REGISTER.md; colonial war bosses.md Section 9"),
            TEXT("51; true believer in the Company system; pursuit boss rather than a conventional fighter."),
            TEXT("Carries the Master Ledger; no personal combat weapon is authored because fighting is not a tool he considers relevant."),
            TEXT("Warm educated argument continues while retreating floor by floor; expects persuasion and process to remain meaningful."),
            TEXT("Coventry House vertically: warehouse, counting halls, offices, archives and roof/dock, with clerks, Regulators and dangerous paper fire."),
            TEXT("Current authority does not define height, build, face, hair, eyes, skin, Company clothing, hat, grooming, spectacles, jewelry or the physical Master Ledger design.")),
        Tier1(
            TEXT("boss.sterling"),
            TEXT("Admiral Katherine Sterling"),
            TEXT("Docs/M7_TIER1_BOSS_REGISTER.md; colonial war bosses.md Section 10"),
            TEXT("46; finest officer in the archipelago, honest and brilliant, openly opposed to the terms of her own commission."),
            TEXT("Primary silhouette is command of HMS Intractable and two escort frigates; only the boarding variant gives her a naval cutlass."),
            TEXT("Formal, professional and tactically excellent; offers terms before firing and handles the Intractable better than any other AI ship."),
            TEXT("Three-dimensional naval approach to Port Sterling: Intractable, escort frigates and harbour batteries; boarding variant ends on her quarterdeck."),
            TEXT("Current authority does not define height, build, face, hair, eyes, skin, exact admiral uniform, hat, scars, age-detail, cutlass design or personal accessories. The final HMS Intractable visual package is also not defined here.")),
        Tier1(
            TEXT("boss.ashcroft"),
            TEXT("General Percival Ashcroft"),
            TEXT("Docs/M7_TIER1_BOSS_REGISTER.md; colonial war bosses.md Section 11"),
            TEXT("55; foreclosure enforcer and least self-aware Phase-7 antagonist."),
            TEXT("Duelling sabre, then a bailiff's cudgel when the sabre proves insufficient."),
            TEXT("Competent gentleman's fencing degrades into something uglier as contracted bailiffs abandon him."),
            TEXT("Ashcroft Hall corridors, dining room, study and terrace above the fields while bailiffs leave one by one."),
            TEXT("Current authority does not define height, build, face, hair, eyes, skin, clothing, legal-office accessories, scars, jewelry or a distinctive personal silhouette.")),
        Tier1(
            TEXT("boss.thorne"),
            TEXT("Colonel Marcus Thorne"),
            TEXT("Docs/M7_TIER1_BOSS_REGISTER.md; colonial war bosses.md Section 12"),
            TEXT("29; genuinely capable, ambitious and has never lost anything."),
            TEXT("Modern Albion officer's sabre; excellent, expensively taught technique."),
            TEXT("Textbook and fast, but unprepared for an opponent who does not follow the textbook; confidence becomes bewilderment as his rotating garrison fails."),
            TEXT("Fort Resolute batteries and wall walk in the cold, with a garrison whose effectiveness decays because nobody stays long enough to learn the place."),
            TEXT("Current authority does not define height, build, face, hair, eyes, skin, exact Albion uniform, age-specific grooming, scars, insignia details or personal accessories."))
    };
}

TArray<FTier1BossVisualDesignGap> FTier1BossVisualReadinessCatalog::BuildDesignGaps()
{
    TArray<FTier1BossVisualDesignGap> Result;
    for (const FTier1BossVisualReadiness& Boss : BuildAll())
    {
        Result.Add({
            FName(*FString::Printf(TEXT("design-gap.tier1-boss-visual.%s"), *Boss.BossStableId.ToString().Replace(TEXT("boss."), TEXT("")))),
            FString::Printf(
                TEXT("%s has current combat/arena/performance authority but no complete physical look authority: %s"),
                *Boss.DisplayName,
                *Boss.MissingPhysicalAuthority),
            Boss.GoverningSource
        });
    }
    return Result;
}

bool FTier1BossVisualReadinessCatalog::Validate(TArray<FString>& OutErrors)
{
    OutErrors.Reset();
    const TArray<FTier1BossVisualReadiness> Bosses = BuildAll();
    if (Bosses.Num() != RequiredTier1BossCount)
    {
        OutErrors.Add(FString::Printf(TEXT("Tier-1 visual readiness requires exactly %d bosses; found %d."), RequiredTier1BossCount, Bosses.Num()));
    }

    const TSet<FName> RequiredIds = {
        TEXT("boss.herrera"), TEXT("boss.reyes"), TEXT("boss.cruz"), TEXT("boss.de_silva"), TEXT("boss.vega"),
        TEXT("boss.blackwood"), TEXT("boss.sterling"), TEXT("boss.ashcroft"), TEXT("boss.thorne")
    };
    TSet<FName> Seen;
    int32 Ready = 0;
    for (const FTier1BossVisualReadiness& Boss : Bosses)
    {
        if (!RequiredIds.Contains(Boss.BossStableId))
        {
            OutErrors.Add(FString::Printf(TEXT("Unknown Tier-1 boss visual id: %s"), *Boss.BossStableId.ToString()));
        }
        if (Seen.Contains(Boss.BossStableId))
        {
            OutErrors.Add(FString::Printf(TEXT("Duplicate Tier-1 boss visual id: %s"), *Boss.BossStableId.ToString()));
        }
        Seen.Add(Boss.BossStableId);

        if (Boss.DisplayName.IsEmpty()
            || Boss.GoverningSource.IsEmpty()
            || Boss.AuthoredAgeRole.IsEmpty()
            || Boss.AuthoredWeaponSilhouette.IsEmpty()
            || Boss.AuthoredPerformanceRead.IsEmpty()
            || Boss.AuthoredArenaRead.IsEmpty()
            || Boss.MissingPhysicalAuthority.IsEmpty())
        {
            OutErrors.Add(FString::Printf(TEXT("Tier-1 boss %s has an incomplete visual-readiness record."), *Boss.BossStableId.ToString()));
        }
        if (!Boss.bCurrentTier1Authority)
        {
            OutErrors.Add(FString::Printf(TEXT("Tier-1 boss %s lost current-authority classification."), *Boss.BossStableId.ToString()));
        }
        if (Boss.bProviderFullLookReady)
        {
            ++Ready;
        }
    }

    if (Seen.Num() != RequiredTier1BossCount || Ready != ProviderFullLookReadyCount)
    {
        OutErrors.Add(TEXT("All nine current Tier-1 bosses must be tracked and remain full-look provider-blocked until physical authority is authored."));
    }
    if (BuildDesignGaps().Num() != ProviderBlockedFullLookCount)
    {
        OutErrors.Add(TEXT("Tier-1 visual design gaps must remain one-per-current-boss while all nine full looks are blocked."));
    }

    return OutErrors.IsEmpty();
}