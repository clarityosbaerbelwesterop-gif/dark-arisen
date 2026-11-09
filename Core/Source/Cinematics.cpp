#include "DarkArisen/Core/Cinematics.h"

#include <algorithm>
#include <array>

namespace DarkArisen::Core
{
    namespace
    {
        using C = CinematicCamera;
        using M = CinematicMusic;
        using P = Performance;
        using K = CinematicKind;

        constexpr float Push = MaxPushMetresPerSecond;

        Cinematic Scene(std::string_view Id, int Number, std::string_view Title, std::string_view Summary, std::string_view Source, C Camera,
            float PushSpeed, M Music, P Animation, float Length, float Control)
        {
            Cinematic Out;
            Out.Id = Id;
            Out.Number = Number;
            Out.Kind = K::Cutscene;
            Out.Title = Title;
            Out.Summary = Summary;
            Out.Source = Source;
            Out.Camera = Camera;
            Out.PushMetresPerSecond = PushSpeed;
            Out.Music = Music;
            Out.Animation = Animation;
            Out.LengthSeconds = Length;
            Out.ControlTakenSeconds = Control;
            return Out;
        }

        Cinematic Endgame(std::string_view Id, int Number)
        {
            Cinematic Out = Scene(Id, Number, "Endgame sequence", "Specified in bosses/crimson_armada.md and the main-story documents; it never references Highmoore.",
                "cutscene catalog.md 3.4", C::Authored, 0.0f, M::None, P::None, 0.0f, 0.0f);
            Out.Specified = false;
            return Out;
        }

        Cinematic Insert(std::string_view Id, int Number, std::string_view Subject, std::string_view Source)
        {
            Cinematic Out = Scene(Id, Number, Subject, "Two seconds, control taken, one subject, control returned.", Source, C::StaticHold, 0.0f, M::None, P::P3,
                InsertSeconds, InsertSeconds);
            Out.Kind = K::Insert;
            return Out;
        }

        Cinematic StandalonePush(std::string_view Id, int Number, std::string_view Subject, std::string_view Summary, std::string_view Source,
            std::string_view Fact, bool NewGamePlus)
        {
            // the catalog gives these two pushes no length: they run as long as #12's push onto the pillow (8 s)
            Cinematic Out = Scene(Id, Number, Subject, Summary, Source, C::SlowPush, Push, M::None, P::None, 8.0f, 8.0f);
            Out.Kind = K::SlowPush;
            Out.RequiresFact = Fact;
            Out.NewGamePlusOnly = NewGamePlus;
            return Out;
        }

        const std::array<Cinematic, 26>& Register()
        {
            static const std::array<Cinematic, 26> Entries = {
                Scene("Cutscene.01.TheBrother", 1, "The Brother",
                    "The game's opening: Draven's attack on the Harlow home. Marc and Denise die; Ethan is taken alive (2026-09 canon lock).",
                    "cutscene catalog.md 3.1 #1; Docs/MAIN_STORY_AUTHORITY_2026_09.md", C::StaticHold, 0.0f, M::NoneUntilLastEightSeconds, P::P1, 130.0f, 130.0f),
                Scene("Cutscene.02.TheLiberacion", 2, "The Liberacion", "Jake takes the ship; the crew assembles. A slow push onto the ship's name, then release.",
                    "cutscene catalog.md 3.1 #2; mechanics/ship_management.md", C::SlowPush, Push, M::MainThemeFirstStatement, P::P2, 80.0f, 80.0f),
                Scene("Cutscene.03.TheFirstLetter", 3, "The First Letter", "Stage 1 retaliation: the Imperial letter arrives in the great cabin.",
                    "cutscene catalog.md 3.2 #3; colonial_war/retaliation_system.md 3.2", C::SlowPush, Push, M::None, P::P3, 35.0f, 35.0f),
                Scene("Cutscene.04.TheGrove", 4, "The Grove", "Ethan's Grove found. Anchored wide on entry for six seconds, then free.",
                    "cutscene catalog.md 3.2 #4; fauna/cannabis_system.md", C::AnchoredWide, 0.0f, M::EthanMotifUnresolved, P::P2, 50.0f, 6.0f),
                Scene("Cutscene.05.TheDreamFightEntry", 5, "The Dream Fight Entry",
                    "The ceremony and the transition into the dream; the only sequence permitted a non-realist visual grammar.",
                    "cutscene catalog.md 3.2 #5; fauna/cannabis_system.md 6", C::Authored, 0.0f, M::SungDiegetic, P::P1, 100.0f, 100.0f),
                Scene("Cutscene.06.DonAlejandroAndTheStrain", 6, "Don Alejandro and the Strain",
                    "He is given Ethan's Strain and understands what it is. A slow push, then a static hold on his hands.",
                    "cutscene catalog.md 3.2 #6; npcs/cigar_collector_don_alejandro.md", C::SlowPush, Push, M::None, P::P1, 65.0f, 65.0f),
                Scene("Cutscene.07.TheEmergence", 7, "The Emergence", "Anchored wide for six seconds facing the basin, then release. Not a flythrough.",
                    "cutscene catalog.md 3.3 #7; highmoore_region.md 1.2", C::AnchoredWide, 0.0f, M::BasinWarmMinor, P::None, 6.0f, 6.0f),
                Scene("Cutscene.08.TheVoiceFromBehind", 8, "The Voice from Behind", "\"I've never seen you before.\" Over the shoulder for the turn only, then free.",
                    "cutscene catalog.md 3.3 #8; princess_quest_the_ball.md 4.4", C::OverShoulderTurn, 0.0f, M::ArionMusiciansDiegetic, P::P1, 18.0f, 18.0f),
                Scene("Cutscene.09.TheEjection", 9, "The Ejection", "Cassian at the gate; the grey.",
                    "cutscene catalog.md 3.3 #9; princess_quest_the_ball.md 8.4", C::StaticTwoShot, 0.0f, M::None, P::P1, 75.0f, 75.0f),
                Scene("Cutscene.10.TheLetterFalse", 10, "The Letter (False)", "Ellis hands it over on the fell. Wind, wide.",
                    "cutscene catalog.md 3.3 #10; princess_quest_the_lake.md 2.1", C::StaticTwoShot, 0.0f, M::None, P::P2, 40.0f, 40.0f),
                Scene("Cutscene.11.TheArrow", 11, "The Arrow",
                    "The camera does not move. Control is taken for the catch only, and the catch is deliberately bad; the rain-dance cue runs on unchanged.",
                    "cutscene catalog.md 3.3 #11; princess_quest_the_lake.md 8", C::Unmoving, 0.0f, M::RunningCueContinues, P::P1, 1.5f, 1.5f),
                Scene("Cutscene.12.TheLetterReal", 12, "The Letter (Real)",
                    "A slow push onto the pillow on entry; the reading itself is player-paced with full control. Rain on glass.",
                    "cutscene catalog.md 3.3 #12; princess_quest_arion_falls.md 4.4", C::SlowPush, Push, M::None, P::P3, 8.0f, 8.0f),
                Scene("Cutscene.13.SheWroteTwo", 13, "She Wrote Two",
                    "Cassian in the courtyard, in rain; Maerwyn on the stair in frame the entire time and never cut to.",
                    "cutscene catalog.md 3.3 #13; princess_quest_arion_falls.md 6.3-6.5", C::StaticTwoShot, 0.0f, M::None, P::P1, 150.0f, 150.0f),
                Scene("Cutscene.14.TheWizardsQuestion", 14, "The Wizard's Question", "Ellis is simply in frame in a shot that was already running: no entrance, no reveal.",
                    "cutscene catalog.md 3.3 #14; princess_quest_belos.md 9.2", C::Authored, 0.0f, M::None, P::P1, 110.0f, 110.0f),
                Endgame("Cutscene.15.Endgame", 15),
                Endgame("Cutscene.16.Endgame", 16),
                Endgame("Cutscene.17.Endgame", 17),
                Endgame("Cutscene.18.Endgame", 18),
                Endgame("Cutscene.19.Endgame", 19),
                Insert("Insert.1.JakesHands", 1, "Jake's hands, shaking, holding a sword that is not", "princess_quest_belos.md 5.3"),
                Insert("Insert.2.TheNineTreasures", 2, "The nine state treasures laid out together", "colonial_war/castle_catalog.md 13.2"),
                Insert("Insert.3.TheBlackCandle", 3, "The black candle, lit, on the cabin table", "colonial_war/assassin_network.md 2.3"),
                Insert("Insert.4.TheNoticeOfEngagement", 4, "The Notice of Engagement, its settlement figure", "colonial_war/assassin_network.md 3.3"),
                Insert("Insert.5.ElowensSignatures", 5, "Elowen's two signatures, side by side in inventory", "princess_quest_the_truth.md 6.3"),
                StandalonePush("SlowPush.5.TheBoundaryStone", 5, "The boundary stone at the lake",
                    "Only on a later playthrough, once the real letter has been read: the only new-game-plus camera event in Dark Arisen.",
                    "cutscene catalog.md 5; princess_quest_the_lake.md 4.4", "Highmoore.RealLetterRead", true),
                StandalonePush("SlowPush.6.AnEmptyRabbitHutch", 6, "An empty rabbit hutch", "Only if the player fills the hutches.",
                    "cutscene catalog.md 5; highmoore_reconstruction.md 5.5", "Highmoore.RabbitHutchesFilled", false),
            };
            return Entries;
        }

        constexpr std::array<std::string_view, 22> Prohibited = {
            "Moment.AfterTheArrow", "Moment.StandingAtTheLake", "Moment.KillingTheArcher", "Moment.ReadingTheRealLetter",
            "Moment.KillingCassian", "Moment.NineMinutesAtBelos", "Moment.WalkOutOfBelos", "Moment.GoingToTheStair",
            "Moment.ReturnThroughTheMountain", "Moment.TheChurchyard", "Moment.EveryBossDeath", "Moment.IbarraInTheGarden",
            "Moment.TheGuardianSettling", "Moment.TheLightElfEnding", "Moment.VidalsConfession", "Moment.KettlesOffice",
            "Moment.AldricsNinetySeconds", "Moment.RowlandsOffer", "Moment.LiberationAftermath", "Moment.CrewSceneInTheGreatCabin",
            "Moment.MaerwynAtBreakfast", "Moment.FirstSightOfAStoppedShaft"};
    }

    std::span<const Cinematic> CinematicRegister() { return Register(); }

    const Cinematic* FindCinematic(const std::string_view Id)
    {
        for (const Cinematic& Entry : Register())
            if (Entry.Id == Id) return &Entry;
        return nullptr;
    }

    std::span<const std::string_view> ProhibitedCinematicMoments() { return Prohibited; }

    bool IsProhibitedCinematicMoment(const std::string_view Id)
    {
        return std::find(Prohibited.begin(), Prohibited.end(), Id) != Prohibited.end();
    }

    CinematicStart CinematicPlayer::Start(const std::string_view Id, const FactQuery& HasFact, const bool NewGamePlus)
    {
        if (Current) return CinematicStart::AlreadyPlaying;
        if (IsProhibitedCinematicMoment(Id)) return CinematicStart::Prohibited;
        const Cinematic* Entry = FindCinematic(Id);
        if (!Entry) return CinematicStart::Unknown;
        if (!Entry->Specified) return CinematicStart::NotSpecified;
        if (Entry->NewGamePlusOnly && !NewGamePlus) return CinematicStart::ConditionUnmet;
        if (!Entry->RequiresFact.empty() && !(HasFact && HasFact(Entry->RequiresFact))) return CinematicStart::ConditionUnmet;
        Current = Entry;
        Elapsed = 0.0f;
        StartedMusic = Entry->Music;
        return CinematicStart::Started;
    }

    CinematicFrame CinematicPlayer::Tick(const float RealDeltaSeconds)
    {
        CinematicFrame Frame;
        if (!Current) return Frame;
        Elapsed += std::max(0.0f, RealDeltaSeconds);
        Frame.Camera = Current->Camera;
        Frame.Elapsed = std::min(Elapsed, Current->LengthSeconds);
        Frame.PushDistance = Current->PushMetresPerSecond * Frame.Elapsed;
        if (Elapsed >= Current->LengthSeconds)
        {
            Frame.Finished = true;
            Current = nullptr;
            return Frame;
        }
        Frame.Playing = true;
        Frame.PlayerHasControl = Elapsed >= Current->ControlTakenSeconds;
        return Frame;
    }
}
