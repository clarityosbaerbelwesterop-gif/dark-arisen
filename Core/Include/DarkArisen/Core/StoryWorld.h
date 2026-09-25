#pragma once

#include "DarkArisen/Core/ColonialWar.h"

#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace DarkArisen::Core
{
    class CampaignRuntime;

    /**
     * Physical story actors authored in ContentSource/Story (chapters 3-10). Ordinals are an
     * authoring contract shared with the O3DE materialiser: append only.
     */
    enum class StoryActorKind : std::uint8_t
    {
        LocationTrigger,
        Contact,
        Evidence,
        RouteResolution,
        WarAction,
        HolderBoss,
        DuelingEnemy,
        EthanHarlow,
        PlayerShip,
        NavalEnemy,
        NavalEncounterGate
    };

    enum class LocationAction : std::uint8_t { ActivateMission, CompleteMission };

    /** Real Ethan's physical state in a scene. Never hostile in any state. */
    enum class EthanPresence : std::uint8_t { Captive, Aboard, Rescued };

    std::optional<StoryActorKind> ParseStoryActorKind(std::string_view Name);
    std::string_view ToString(StoryActorKind Kind);

    /**
     * One authored actor with every field any kind uses. The mission's checkpoint/spawn are
     * already folded in where the actor has none of its own (the Unreal materialiser rule).
     */
    struct StoryActorSpec
    {
        StoryActorKind Kind = StoryActorKind::LocationTrigger;
        std::string Name;
        std::string Anchor;
        std::string MissionId;
        std::string CheckpointId;
        std::string SpawnId;

        LocationAction Action = LocationAction::ActivateMission;   // LocationTrigger

        std::string ContactId;                                     // Contact
        std::string DialogueId;
        std::string DisplayName;
        std::string Text;

        std::string Title;                                         // Evidence
        std::string Body;
        std::string WorldFactOnRead;
        std::string RequiredBossId;
        bool CompletesMission = false;                             // Evidence, holding claim

        std::string OutcomeKey;                                    // RouteResolution, WarAction, HolderBoss
        std::string OutcomeValue;
        std::string Label;

        std::string RegionId;                                      // WarAction (regional action)
        ColonialFaction InitialController = ColonialFaction::Imperial;
        WarActionVerb Verb = WarActionVerb::Support;
        ColonialFaction TargetFaction = ColonialFaction::Imperial;
        int ControlDelta = 0;
        int LiberationDelta = 0;
        int CrimsonDelta = 0;
        std::string HoldingId;                                     // WarAction (holding claim variant)

        std::string BossId;                                        // HolderBoss
        bool CompletesMissionOnDefeat = true;

        EthanPresence Ethan = EthanPresence::Captive;              // EthanHarlow

        int RequiredDefeats = 1;                                   // NavalEncounterGate
        std::string RequiredWorldFact;
        /** Naval mission gates also need the player's ship within this radius (metres); 0 = none. */
        double CompletionRadiusMetres = 0.0;

        /** WarAction with actionId/locationId: Jake claims a holding instead of moving a region. */
        bool IsHoldingClaim() const { return Kind == StoryActorKind::WarAction && !HoldingId.empty(); }
    };

    /** Unreal layout centimetres, +X forward, +Y right, +Z up (ContentSource frame). */
    using LayoutPoint = std::array<double, 3>;

    struct StoryGeometrySpec
    {
        std::string Name;
        std::string Anchor;
        /** Greybox glTF object path ("/Game/.../SM_X.SM_X"); empty for a box. */
        std::string ObjectPath;
        /** Box size in centimetres (physicalGeometry), centred on the anchor. */
        LayoutPoint Size{};
        bool IsBox() const { return ObjectPath.empty(); }
    };

    /** A ContentSource/Story mission contract (plus its ContentSource/Naval companion). */
    struct StoryMissionContract
    {
        std::string SourcePath;
        std::string MissionId;
        std::string RegionId;
        std::string MapId;
        std::string EntryAnchor;
        std::string CheckpointId;
        std::string SpawnId;
        std::string NextMission;
        std::string NavigationLaw;
        /** Authored order is the route order through the space. */
        std::vector<std::pair<std::string, LayoutPoint>> Anchors;
        std::vector<StoryGeometrySpec> Geometry;
        std::vector<StoryActorSpec> Actors;
        std::vector<std::string> CompletionFacts;
        std::vector<std::string> EncounterIds;
        bool CreditsHandoff = false;

        const LayoutPoint* FindAnchor(std::string_view Name) const;
        /** Ship-borne missions: the player sails La Liberacion here (PlayerShip or naval actors). */
        bool IsNaval() const;
    };

    /**
     * Fails closed on anything that would leave a mission unplayable or non-canonical: unknown
     * or mis-mapped mission, missing anchors, actors without their required data, no way to
     * activate or complete the mission, a broken next route, completion facts the campaign does
     * not author, or a boss/Ethan identity that breaks canon.
     */
    bool ValidateStoryContract(const StoryMissionContract& Contract, std::vector<std::string>& OutErrors);

    /** What an interaction produced for presentation and the journal. */
    struct StoryInteraction
    {
        bool Accepted = false;
        bool MissionCompleted = false;
        std::string Speaker;
        std::string Line;
        std::string DocumentTitle;
        std::string DocumentBody;
    };

    /**
     * Runtime semantics of the story actors, ported from the Unreal actors
     * (AMainStoryLocationTriggerActor, AMainStoryContactActor, AMainStoryEvidenceActor,
     * AMainStoryRouteResolutionActor, AMainStoryWarActionActor, ANavalEncounterGateActor,
     * ANavalMissionGateActor). Every mutation goes through the campaign's validated API.
     */
    namespace StoryActions
    {
        /**
         * Jake entered a location trigger. Completion is refused while living dueling enemies
         * remain in the space: authored fights cannot be bypassed by running through them.
         */
        bool OnLocationEntered(CampaignRuntime& Campaign, const StoryActorSpec& Actor, bool LivingDuelistsRemain);

        bool CanInteract(const CampaignRuntime& Campaign, const StoryActorSpec& Actor);
        StoryInteraction Interact(CampaignRuntime& Campaign, const StoryActorSpec& Actor);

        /**
         * Naval gates: completes the mission once at least RequiredDefeats hostiles exist and all
         * of them are sunk, the required world fact holds and (for mission gates) the player's
         * ship is inside the completion radius.
         */
        bool UpdateNavalGate(CampaignRuntime& Campaign, const StoryActorSpec& Gate, int HostileShips, int SunkShips,
            double PlayerShipDistanceMetres);
    }
}
