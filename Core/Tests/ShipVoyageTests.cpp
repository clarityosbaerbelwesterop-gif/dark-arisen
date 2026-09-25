#include "TestHarness.h"

#include "DarkArisen/Core/ShipVoyage.h"

using namespace DarkArisen::Core;

TEST_CASE("Ship: point of sail follows the relative wind angle")
{
    ShipVoyage Ship;
    const std::pair<float, PointOfSail> Cases[] = {{10.0f, PointOfSail::InIrons}, {45.0f, PointOfSail::CloseHauled},
        {70.0f, PointOfSail::CloseReach}, {95.0f, PointOfSail::BeamReach}, {130.0f, PointOfSail::BroadReach},
        {170.0f, PointOfSail::Running}, {-170.0f, PointOfSail::Running}};
    for (const auto& [Wind, Expected] : Cases)
    {
        Ship.SetWind(Wind, 6.0f);
        CHECK(Ship.GetPointOfSail() == Expected);
    }
}

TEST_CASE("Ship: the helm needs ownership and Jake at the wheel")
{
    ShipVoyage Ship;
    CHECK(!Ship.SetThrottle(1.0f));
    Ship.SetJakeAtHelm(true);
    CHECK(!Ship.IsJakeAtHelm());
    CHECK(!Ship.Embark("character.jake"));
    Ship.SetOwnedAndUnlocked();
    CHECK(Ship.Embark("character.jake"));
    Ship.SetJakeAtHelm(true);
    CHECK(Ship.SetThrottle(2.0f));
    CHECK_NEAR(Ship.GetThrottle(), 1.0, 1e-6);
    CHECK(!Ship.Disembark("character.jake"));
    Ship.SetJakeAtHelm(false);
    CHECK(Ship.Disembark("character.jake"));
}

TEST_CASE("Ship: turning is rate-limited and the ship keeps sailing without Jake at the helm")
{
    ShipVoyage Ship;
    Ship.SetOwnedAndUnlocked();
    Ship.SetJakeAtHelm(true);
    Ship.SetWind(90.0f, 8.0f);
    CHECK(Ship.SetThrottle(1.0f));
    Ship.SetHelmCommandDegrees(90.0f);
    Ship.Tick(1.0f);
    CHECK_NEAR(Ship.GetHeadingDegrees(), 8.0, 1e-3);
    Ship.SetJakeAtHelm(false);
    float Travelled = 0.0f;
    for (int Step = 0; Step < 600; ++Step)
    {
        Travelled += Ship.Tick(0.1f);
    }
    CHECK_NEAR(Ship.GetHeadingDegrees(), 90.0, 1e-3);
    CHECK(Travelled > 0.0f);
    // Heading 90 into wind 90 is in irons: the sails stop driving the hull.
    CHECK(Ship.GetPointOfSail() == PointOfSail::InIrons);
    CHECK(Ship.GetForwardSpeed() < 0.5f);
}

TEST_CASE("Ship: losing Mira degrades handling but never stops the vessel")
{
    ShipVoyage Ship;
    CHECK_NEAR(Ship.GetCrewHandlingFactor(), 1.0, 1e-6);
    Ship.SetCrewMemberAlive("crew.mira", false);
    CHECK_NEAR(Ship.GetCrewHandlingFactor(), 0.6 * 0.35 + 0.65, 1e-6);
    Ship.SetActiveHands(0);
    CHECK_NEAR(Ship.GetCrewHandlingFactor(), 0.6 * 0.35 + 0.15 * 0.65, 1e-6);
    Ship.SetActiveHands(500);
    CHECK(Ship.Capture("Levels/OpenSea", {}).ActiveHands == 90);
}

TEST_CASE("Ship: physical charts must be acquired before Jake annotates them")
{
    ShipVoyage Ship;
    CHECK(!Ship.AnnotatePhysicalChart("chart.harlow.family"));
    CHECK(Ship.AcquirePhysicalChart("chart.harlow.family"));
    CHECK(Ship.AnnotatePhysicalChart("chart.harlow.family"));
    CHECK(Ship.HasPhysicalChart("chart.harlow.family"));
    CHECK(!Ship.AcquirePhysicalChart(""));
}

TEST_CASE("Ship: hull damage clamps at zero and ignores invalid input")
{
    ShipVoyage Ship;
    Ship.ApplyHullDamage(-50.0f);
    CHECK_NEAR(Ship.GetHullIntegrity(), 1000.0, 1e-6);
    Ship.ApplyHullDamage(400.0f);
    CHECK_NEAR(Ship.GetHullIntegrity(), 600.0, 1e-6);
    Ship.ApplyHullDamage(5000.0f);
    CHECK_NEAR(Ship.GetHullIntegrity(), 0.0, 1e-6);
}
