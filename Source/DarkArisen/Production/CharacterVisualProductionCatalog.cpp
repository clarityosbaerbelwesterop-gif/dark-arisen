// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Production/CharacterVisualProductionCatalog.h"

namespace
{
FCharacterVisualProductionBrief Brief(
    const TCHAR* StableId,
    const TCHAR* DisplayName,
    const ECharacterVisualProductionRole Role,
    const TCHAR* Source,
    const TCHAR* Physical,
    const TCHAR* Wardrobe,
    const TCHAR* Performance,
    const TCHAR* Unknowns,
    const bool bProviderReady)
{
    FCharacterVisualProductionBrief Result;
    Result.StableId = StableId;
    Result.DisplayName = DisplayName;
    Result.Role = Role;
    Result.GoverningSource = Source;
    Result.PhysicalFacts = Physical;
    Result.WardrobeAndObjects = Wardrobe;
    Result.PerformanceRead = Performance;
    Result.ExplicitUnknowns = Unknowns;
    Result.bProviderReferenceReady = bProviderReady;
    return Result;
}
}

TArray<FCharacterVisualProductionBrief> FCharacterVisualProductionCatalog::BuildMajorCharacterBriefs()
{
    return {
        Brief(
            TEXT("character.jake-harlow"),
            TEXT("Jake Harlow"),
            ECharacterVisualProductionRole::Protagonist,
            TEXT("docs/characters/jake_harlow.md; docs/design/style_bible.md; animation system.md"),
            TEXT("Age 17 at story start, 18-19 by the end; 1.75 m; lean and sinewy; dark-brown hair falling into his face; green eyes."),
            TEXT("The character bible does not lock a complete canonical wardrobe in the visual quick reference."),
            TEXT("Quiet observer; after the trauma his revenge is still, methodical and patient. Combat posture waits, reads and counters rather than celebrating violence."),
            TEXT("Canonical clothing/costume breakdown, exact facial proportions, skin tone and other unspecified visual details remain unlocked in this source."),
            true),
        Brief(
            TEXT("character.mira"),
            TEXT("Mira"),
            ECharacterVisualProductionRole::CoreCrew,
            TEXT("docs/design/npcs/named_crew_deep_dives.md Section 2.1"),
            TEXT("Age 32; 175 cm; lean, strong, weathered; long dark braid with premature salt-gray; pale gray-blue eyes; sun-tanned skin; faint knuckle scars, deeper left-forearm scar; small scar through right eyebrow; usually-covered compass-rose tattoo on left shoulder."),
            TEXT("Practical layered sailor gear with Fjordlund touches; carved bone buttons from her mother; functional over fashionable; signature worn leather coat that belonged to her brother."),
            TEXT("Steady, reserved, practical. Direct delivery and controlled emotion; commanding only when needed. Movement should read as experienced shipboard competence without flourish."),
            TEXT("No additional face shape, cosmetic styling or costume embellishment may be invented as canon."),
            true),
        Brief(
            TEXT("character.big-tom"),
            TEXT("Big Tom"),
            ECharacterVisualProductionRole::CoreCrew,
            TEXT("docs/design/npcs/named_crew_deep_dives.md Section 3.1"),
            TEXT("Age 47; 188 cm; massive, broad-shouldered, heavily muscled; bald; full well-kept gray-brown beard; brown eyes with laugh lines; ruddy skin; forge-scarred forearms; missing left pinky; elaborate Imperial-military-origin chest tattoo."),
            TEXT("Heavy leather forge apron while working; rolled-sleeve linen shirts; heavy boots; small wedding ring on a chain; head scarf often worn while forging."),
            TEXT("Loud but gentle, physically protective and warm. Forge work has practiced rhythm; age and old wounds are visible without making him frail."),
            TEXT("Exact chest-tattoo design and unspecified costume variants remain unlocked."),
            true),
        Brief(
            TEXT("character.ines"),
            TEXT("Ines"),
            ECharacterVisualProductionRole::CoreCrew,
            TEXT("docs/design/npcs/named_crew_deep_dives.md Section 4.1"),
            TEXT("Age 38; 168 cm; wiry build and careful movement; dark-brown hair pinned up practically with gray beginning at the temples; dark-brown intense eyes; olive Rexan/Imperial mixed-heritage skin; acid scar on right hand; small dot tattoo on inner wrist."),
            TEXT("Practical protective alchemist gear; leather apron with many pockets; layered protection; small flasks carried; treasured brooch from her mother."),
            TEXT("Controlled, precise and observant. Rarely raises her voice; physical work should read as deliberate and technically competent rather than theatrical."),
            TEXT("Exact tattoo meaning/graphic beyond the source description and unspecified costume variants remain unlocked."),
            true),
        Brief(
            TEXT("character.father-salvio"),
            TEXT("Father Salvio"),
            ECharacterVisualProductionRole::CoreCrew,
            TEXT("docs/design/npcs/named_crew_deep_dives.md Section 5.1"),
            TEXT("Age 58; 172 cm; slim and monastic but not frail; short white tonsure-style hair; neatly trimmed white beard; pale-blue kind, observant eyes; pale skin; calloused working hands."),
            TEXT("Well-worn black cassock most often; simple wooden cross; wooden rosary always present; simple sandals; monastic but practical; separate battle robe exists when he fights."),
            TEXT("Warm, patient and observant with earned calm. He works with his hands and can become unexpectedly fierce without losing the character's restraint."),
            TEXT("Exact battle-robe construction and unspecified liturgical detail remain unlocked."),
            true),
        Brief(
            TEXT("character.esteban"),
            TEXT("Esteban"),
            ECharacterVisualProductionRole::CoreCrew,
            TEXT("docs/design/npcs/named_crew_deep_dives.md Section 6.1"),
            TEXT("Age 78; 170 cm; slim, deeply weathered and surprisingly agile; long white hair usually tied back; full white beard; deeply lined gentle dark-brown eyes; mahogany sun-weathered skin; multiple small meaningful tattoos; missing left ear."),
            TEXT("Mixed-cultural sailor gear with items gathered from many regions; a gifted cap; polished boots; chain with multiple charms; necklace carrying his late wife's wedding ring."),
            TEXT("Slow, deliberate, patient physicality without generic old-man frailty. Decades at sea should read in balance, hands, gaze and economical movement."),
            TEXT("Individual tattoo designs, charm identities beyond authored facts and unspecified clothing pieces remain unlocked."),
            true),
        Brief(
            TEXT("character.elowen-arion"),
            TEXT("Elowen Arion"),
            ECharacterVisualProductionRole::MajorNarrative,
            TEXT("the princess.md Sections 2-5; house arion.md; mounted travel.md"),
            TEXT("Age 19. The source locks her peaceful first impression and her exceptional riding, languages and aristocratic training, but does not supply a sufficient physical feature sheet for canonical face/body generation."),
            TEXT("House Arion aristocratic context and riding context exist, but this source does not lock a production-ready canonical costume breakdown."),
            TEXT("Peaceful in the specific sense of someone who has already made a decision; high register by training, then plain/direct underneath. At the ball the calm reads as genuine loveliness rather than performance."),
            TEXT("Hair, eyes, height, build, skin, facial structure and exact wardrobe are not sufficiently locked for a canonical provider visual. Do not synthesize them."),
            false),
        Brief(
            TEXT("character.ethan-harlow"),
            TEXT("Ethan Harlow"),
            ECharacterVisualProductionRole::MajorNarrative,
            TEXT("docs/characters/docs/characters/ethan_harlow.md"),
            TEXT("Age 22 at start, 23-24 when Jake finds him; 1.88 m; broad-shouldered and powerfully built from years of sea work; dark-brown hair shorter than Jake's and often tied back; hazel-brown eyes; scar over right eyebrow from childhood."),
            TEXT("The character bible does not lock a complete canonical costume sheet. Later identity includes a captain's coat, but exact construction is not authored here."),
            TEXT("Charismatic born leader. Combat is aggressive, forward and space-taking with a two-handed sword; emotional mask changes across the confrontation with Jake."),
            TEXT("Skin tone, exact facial proportions and full costume design remain unlocked in this source."),
            true),
        Brief(
            TEXT("character.draven-voss"),
            TEXT("Captain Draven Voss"),
            ECharacterVisualProductionRole::MajorAntagonist,
            TEXT("docs/characters/docs/characters/docs/characters/draven_voss.md"),
            TEXT("Age 45; 1.90 m; wiry rather than bulky; posture carries twenty years of naval training; black hair with silver streaks, neatly tied back; narrow well-kept beard; ice-blue eyes; old burn scar on left side of neck."),
            TEXT("Former Imperial naval officer and current Crimson Armada captain. The source does not supply a complete canonical clothing construction or color sheet."),
            TEXT("Quiet, measured authority: never needs to shout. Combat is classically trained, economical and precise with sabre and pistol before the authored final unarmed phase."),
            TEXT("Skin tone, exact facial proportions and full officer/pirate costume breakdown remain unlocked in this source."),
            true)
    };
}

TArray<FCharacterVisualProductionDesignGap> FCharacterVisualProductionCatalog::BuildDesignGaps()
{
    return {
        {
            TEXT("design-gap.character-visual.elowen-physical-sheet"),
            TEXT("Elowen is deeply authored as a person, but the current character source does not provide enough concrete face/body facts for a canonical visual generation. Keep provider reference generation blocked until design authority supplies or approves those facts/reference."),
            TEXT("the princess.md Sections 2-5")
        },
        {
            TEXT("design-gap.character-visual.primary-costume-sheets"),
            TEXT("Jake, Ethan and Draven have strong physical descriptions but incomplete production costume sheets. Provider reference work must not silently promote invented clothing into canon."),
            TEXT("docs/characters/jake_harlow.md; docs/characters/docs/characters/ethan_harlow.md; docs/characters/docs/characters/docs/characters/draven_voss.md")
        }
    };
}

bool FCharacterVisualProductionCatalog::Validate(TArray<FString>& OutErrors)
{
    OutErrors.Reset();
    const TArray<FCharacterVisualProductionBrief> Briefs = BuildMajorCharacterBriefs();

    if (Briefs.Num() != MajorCharacterBriefCount)
    {
        OutErrors.Add(FString::Printf(TEXT("Major-character visual catalog requires %d entries; found %d."), MajorCharacterBriefCount, Briefs.Num()));
    }

    int32 ReadyCount = 0;
    int32 BlockedCount = 0;
    TSet<FName> Seen;

    for (const FCharacterVisualProductionBrief& Character : Briefs)
    {
        if (Character.StableId.IsNone()
            || Character.DisplayName.IsEmpty()
            || Character.GoverningSource.IsEmpty()
            || Character.PhysicalFacts.IsEmpty()
            || Character.PerformanceRead.IsEmpty()
            || Character.ExplicitUnknowns.IsEmpty())
        {
            OutErrors.Add(TEXT("Every major-character visual brief needs identity, source, physical facts, performance read and explicit unknowns."));
        }

        if (Seen.Contains(Character.StableId))
        {
            OutErrors.Add(FString::Printf(TEXT("Duplicate major-character visual id: %s"), *Character.StableId.ToString()));
        }
        Seen.Add(Character.StableId);

        if (Character.bProviderReferenceReady) ++ReadyCount;
        else ++BlockedCount;

        if (Character.bApprovedReferenceExists || !Character.ApprovedReferencePath.IsEmpty())
        {
            OutErrors.Add(FString::Printf(TEXT("Character %s falsely claims an approved external reference."), *Character.StableId.ToString()));
        }
    }

    if (ReadyCount != ProviderReadyCharacterCount || BlockedCount != ExplicitlyBlockedCharacterCount)
    {
        OutErrors.Add(TEXT("Major-character provider-ready/blocked split drifted from source-backed visual completeness."));
    }

    if (BuildDesignGaps().Num() != 2)
    {
        OutErrors.Add(TEXT("Character visual production must retain the Elowen physical-sheet and primary-costume gaps."));
    }

    return OutErrors.IsEmpty();
}
