# Dark Arisen engine-neutral gameplay core.
#
# Pure C++20, no engine headers. The O3DE Gems and the Unreal reference
# implementation both sit on top of the rules defined here, so they can be
# built and tested on any machine with a C++ compiler and CMake.
cmake_minimum_required(VERSION 3.22)
project(DarkArisenCore VERSION 0.1.0 LANGUAGES CXX)

option(DARKARISEN_CORE_TESTS "Build the Dark Arisen core test executable" ON)

add_library(DarkArisenCore STATIC
    Source/MissionCatalog.cpp
    Source/CampaignState.cpp
    Source/SaveRules.cpp
    Source/SaveCodec.cpp
    Source/CampaignRuntime.cpp
    Source/OpeningRuntime.cpp
    Source/Combat.cpp
    Source/ShipVoyage.cpp
    Source/EntityPolicy.cpp
    Source/EnemyBrain.cpp
    Source/Ocean.cpp
    Source/Water.cpp
    Source/Presentation.cpp
    Source/LevelSpawn.cpp
    Source/Camera.cpp
    Source/ColonialWar.cpp
    Source/NavalCombat.cpp
    Source/StoryWorld.cpp
    Source/QuestJournal.cpp
    Source/ProgressionEconomy.cpp
    Source/LivingWorld.cpp
)
add_library(DarkArisen::Core ALIAS DarkArisenCore)
target_include_directories(DarkArisenCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include)
target_compile_features(DarkArisenCore PUBLIC cxx_std_20)
set_target_properties(DarkArisenCore PROPERTIES CXX_EXTENSIONS OFF POSITION_INDEPENDENT_CODE ON)

if(MSVC)
    # MSVC is verified in CI only; warnings stay visible but are not fatal until a Windows gate exists.
    target_compile_options(DarkArisenCore PRIVATE /W4 /permissive-)
else()
    target_compile_options(DarkArisenCore PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Werror)
endif()

if(DARKARISEN_CORE_TESTS)
    enable_testing()
    add_executable(DarkArisenCoreTests
        Tests/TestMain.cpp
        Tests/MissionCatalogTests.cpp
        Tests/CampaignRuntimeTests.cpp
        Tests/SaveTests.cpp
        Tests/OpeningRuntimeTests.cpp
        Tests/CombatTests.cpp
        Tests/ShipVoyageTests.cpp
        Tests/CanonTests.cpp
        Tests/EnemyBrainTests.cpp
        Tests/OceanTests.cpp
        Tests/WaterTests.cpp
        Tests/PresentationTests.cpp
        Tests/CameraTests.cpp
        Tests/StoryWorldTests.cpp
        Tests/LivingCampaignTests.cpp
        Tests/UnrealParityTests.cpp
    )
    target_link_libraries(DarkArisenCoreTests PRIVATE DarkArisen::Core)
    # Parity tests read the Unreal reference sources so the two implementations cannot drift.
    get_filename_component(DARKARISEN_REPO_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)
    target_compile_definitions(DarkArisenCoreTests PRIVATE DARKARISEN_REPO_ROOT="${DARKARISEN_REPO_ROOT}")
    if(MSVC)
        target_compile_options(DarkArisenCoreTests PRIVATE /W4 /permissive-)
    else()
        target_compile_options(DarkArisenCoreTests PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()
    add_test(NAME DarkArisenCoreTests COMMAND DarkArisenCoreTests)
endif()
