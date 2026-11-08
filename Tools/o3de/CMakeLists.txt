# Native O3DE operations owner for Dark Arisen (no engine dependency).
cmake_minimum_required(VERSION 3.22)
project(DarkArisenO3DEOps LANGUAGES CXX)

if(MSVC)
    # MSVC is verified in CI only; warnings stay visible but are not fatal until a Windows gate exists.
    set(DARKARISEN_TOOL_WARNINGS /W4 /permissive-)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    set(DARKARISEN_TOOL_WARNINGS -Wall -Wextra -Wpedantic -Werror)
    # Materialised content is checked byte for byte, so arithmetic must not depend on the target ISA:
    # compilers that default to an FMA-capable ISA (GCC 15 on ubuntu 26.04) would otherwise fuse a*b+c
    # and round differently. Applies to the core added below as well. MSVC does not contract under /fp:precise.
    add_compile_options(-ffp-contract=off)
endif()

# The engine-neutral core supplies the rules the materialiser validates against (mission levels,
# presentation timelines, spawn rules); the Gem's plain headers supply type ids and authoring enums.
set(DARKARISEN_CORE_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(../../Core ${CMAKE_CURRENT_BINARY_DIR}/Core)

add_library(DarkArisenAssetPipeline STATIC Ops/AssetPipeline.cpp Ops/Gltf.cpp Ops/Prefab.cpp Ops/Materializer.cpp Ops/StoryContracts.cpp
    Ops/ArtKit.cpp Ops/ArtKitArchipelago.cpp Ops/ArtKitAuthored.cpp Ops/ArtKitCore.cpp Ops/ArtKitShips.cpp Ops/ArtKitTerrain.cpp Ops/ArtKitTextures.cpp Ops/ArtKitWorld.cpp
    Ops/ArtKitCast.cpp Ops/ArtKitHumanActor.cpp Ops/ArtKitHumanBody.cpp Ops/ArtKitHumanDress.cpp Ops/ArtKitHumanMotion.cpp)
target_include_directories(DarkArisenAssetPipeline PUBLIC Ops ../../Engine/O3DE/DarkArisen/Gem/Include)
target_link_libraries(DarkArisenAssetPipeline PUBLIC DarkArisen::Core)
target_compile_features(DarkArisenAssetPipeline PUBLIC cxx_std_20)
target_compile_options(DarkArisenAssetPipeline PRIVATE ${DARKARISEN_TOOL_WARNINGS})

# decimate-art: game-ready versions of heavy scans (native quadric-error simplifier, no third-party code)
add_library(DarkArisenArtDecimate STATIC Ops/ArtKitDecimate.cpp)
target_link_libraries(DarkArisenArtDecimate PUBLIC DarkArisenAssetPipeline)
target_compile_options(DarkArisenArtDecimate PRIVATE ${DARKARISEN_TOOL_WARNINGS})

add_executable(DarkArisenO3DE Ops/DarkArisenO3DE.cpp)
target_link_libraries(DarkArisenO3DE PRIVATE DarkArisenAssetPipeline DarkArisenArtDecimate)
target_compile_options(DarkArisenO3DE PRIVATE ${DARKARISEN_TOOL_WARNINGS})

enable_testing()
add_executable(DarkArisenAssetPipelineTests Ops/AssetPipelineTests.cpp)
target_link_libraries(DarkArisenAssetPipelineTests PRIVATE DarkArisenAssetPipeline)
target_compile_options(DarkArisenAssetPipelineTests PRIVATE ${DARKARISEN_TOOL_WARNINGS})
add_test(NAME DarkArisenAssetPipelineTests COMMAND DarkArisenAssetPipelineTests)

add_executable(DarkArisenConversionTests Ops/ConversionTests.cpp)
target_link_libraries(DarkArisenConversionTests PRIVATE DarkArisenAssetPipeline)
target_compile_options(DarkArisenConversionTests PRIVATE ${DARKARISEN_TOOL_WARNINGS})
target_compile_definitions(DarkArisenConversionTests PRIVATE DARKARISEN_REPO_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/../..")
add_test(NAME DarkArisenConversionTests COMMAND DarkArisenConversionTests)
