# Standalone build of O3DE 2605.0 AzCore plus the Dark Arisen campaign adapter.
#
# Runs Dark Arisen's CampaignSystemComponent inside a real AZ::ComponentApplication
# without O3DE's third-party package server. Evidence level: O3DE framework runtime.
# It does NOT replace Editor/GameLauncher/Atom verification on a full engine build.
#
#   cmake -S Tools/o3de/AzCoreProbe -B Build/azcore-probe -G Ninja \
#         -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Release \
#         -DO3DE_ENGINE_ROOT=/opt/dark-arisen/o3de -DDARKARISEN_PROBE_DEPS=/opt/dark-arisen/deps
cmake_minimum_required(VERSION 3.24)
project(DarkArisenAzCoreProbe C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(O3DE_ENGINE_ROOT "$ENV{O3DE_ENGINE_ROOT}" CACHE PATH "Pinned O3DE 2605.0 checkout")
set(DARKARISEN_PROBE_DEPS "" CACHE PATH "Directory with lua, cityhash, rapidjson, 3p-package-source (DEPENDENCIES.lock)")
foreach(required IN ITEMS "${O3DE_ENGINE_ROOT}/engine.json" "${DARKARISEN_PROBE_DEPS}/lua/lua.h"
        "${DARKARISEN_PROBE_DEPS}/cityhash/src/city.cc" "${DARKARISEN_PROBE_DEPS}/rapidjson/include/rapidjson/reader.h"
        "${DARKARISEN_PROBE_DEPS}/3p-package-source/package-system/RapidXML-multiplatform/RapidXML/include/rapidxml/rapidxml.h")
    if(NOT EXISTS "${required}")
        message(FATAL_ERROR "Probe prerequisite missing: ${required} (see DEPENDENCIES.lock)")
    endif()
endforeach()

get_filename_component(REPO_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../.." ABSOLUTE)
set(AZ "${O3DE_ENGINE_ROOT}/Code/Framework/AzCore")
set(AZF "${O3DE_ENGINE_ROOT}/Code/Framework/AzFramework")
set(DEPS "${DARKARISEN_PROBE_DEPS}")

# O3DE package layouts: <Lua/lua.h> and a cityhash config header.
set(SHIM "${CMAKE_CURRENT_BINARY_DIR}/shim")
file(GLOB lua_headers "${DEPS}/lua/*.h")
file(COPY ${lua_headers} DESTINATION "${SHIM}/Lua")
file(WRITE "${SHIM}/cityhash/config.h" "#pragma once\n")

function(o3de_collect out listfile)
    get_filename_component(dir ${listfile} DIRECTORY)
    unset(FILES)
    include(${listfile})
    set(result)
    foreach(f ${FILES})
        if(f MATCHES "\\.(cpp|c)$")
            get_filename_component(abs "${dir}/${f}" ABSOLUTE)
            list(APPEND result ${abs})
        endif()
    endforeach()
    set(${out} ${result} PARENT_SCOPE)
endfunction()
o3de_collect(core_src ${AZ}/AzCore/azcore_files.cmake)
o3de_collect(std_src ${AZ}/AzCore/std/azstd_files.cmake)
o3de_collect(plat_src ${AZ}/Platform/Linux/platform_linux_files.cmake)

file(GLOB lua_src "${DEPS}/lua/*.c")
list(FILTER lua_src EXCLUDE REGEX "/(lua|luac|onelua|ltests)\\.c$")
add_library(probe_lua STATIC ${lua_src})
target_compile_definitions(probe_lua PRIVATE LUA_USE_LINUX)
set_target_properties(probe_lua PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_library(probe_cityhash STATIC "${DEPS}/cityhash/src/city.cc")
target_include_directories(probe_cityhash PUBLIC "${DEPS}/cityhash/src" PRIVATE "${SHIM}/cityhash")

# Global definitions and flags O3DE applies on Linux/Clang (cmake/Platform/**/Configurations_*.cmake).
set(O3DE_DEFINES O3DE_STACK_CAPTURE_DEPTH=3 O3DE_DISABLE_CONDITIONAL_EXPLICIT=1 LINUX __linux__ LINUX64
    _HAS_EXCEPTIONS=0 _PROFILE AZ_PROFILE_BUILD NDEBUG AZ_ENABLE_TRACING AZ_ENABLE_DEBUG_TOOLS
    AZ_BUILD_CONFIGURATION_TYPE="profile")
set(O3DE_FLAGS -fno-exceptions -msse4.1 -fvisibility=hidden -fvisibility-inlines-hidden)

add_library(AzCore STATIC ${core_src} ${std_src} ${plat_src})
target_include_directories(AzCore BEFORE PUBLIC ${AZ} ${AZ}/Platform/Linux ${AZ}/Platform/Common ${SHIM}
    "${DEPS}/rapidjson/include"
    "${DEPS}/3p-package-source/package-system/RapidXML-multiplatform/RapidXML/include")
target_compile_definitions(AzCore PUBLIC ${O3DE_DEFINES} PRIVATE AZCORE_EXPORTS)
target_compile_options(AzCore PUBLIC ${O3DE_FLAGS} PRIVATE -w)
target_link_libraries(AzCore PUBLIC probe_lua probe_cityhash zstd z unwind pthread dl atomic)

# Engine-neutral core, compiled with O3DE's flags (no exceptions).
set(DARKARISEN_CORE_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory("${REPO_ROOT}/Core" "${CMAKE_CURRENT_BINARY_DIR}/DarkArisenCore")
target_compile_options(DarkArisenCore PRIVATE -fno-exceptions)

set(GEM "${REPO_ROOT}/Engine/O3DE/DarkArisen/Gem")
add_executable(DarkArisenCampaignProbe
    CampaignRuntimeProbe.cpp
    ${GEM}/Source/Story/CampaignSystemComponent.cpp
    ${GEM}/Source/Persistence/SaveSlotStore.cpp
    ${AZF}/AzFramework/API/ApplicationAPI.cpp
    ${AZF}/AzFramework/IO/LocalFileIO.cpp
    ${AZF}/Platform/Common/UnixLike/AzFramework/IO/LocalFileIO_UnixLike.cpp)
target_include_directories(DarkArisenCampaignProbe PRIVATE ${AZF} ${AZF}/Platform/Linux ${AZF}/Platform/Common
    ${GEM}/Include ${GEM}/Source)
target_link_libraries(DarkArisenCampaignProbe PRIVATE AzCore DarkArisen::Core)

enable_testing()
add_test(NAME DarkArisenCampaignProbe COMMAND DarkArisenCampaignProbe)

# Gameplay adapters (combat, enemy brain, ship) on the same AzCore runtime.
add_executable(DarkArisenGameplayProbe
    GameplayRuntimeProbe.cpp
    ${GEM}/Source/Story/CampaignSystemComponent.cpp
    ${GEM}/Source/Persistence/SaveSlotStore.cpp
    ${GEM}/Source/Combat/CombatantComponent.cpp
    ${GEM}/Source/Combat/EnemyBrainComponent.cpp
    ${GEM}/Source/Ships/ShipVoyageComponent.cpp
    ${AZF}/AzFramework/API/ApplicationAPI.cpp
    ${AZF}/AzFramework/IO/LocalFileIO.cpp
    ${AZF}/Platform/Common/UnixLike/AzFramework/IO/LocalFileIO_UnixLike.cpp
    ${AZF}/AzFramework/Physics/CharacterBus.cpp
    ${AZF}/AzFramework/Physics/Common/PhysicsSceneQueries.cpp
    ${AZF}/AzFramework/Physics/Collision/CollisionGroups.cpp
    ${AZF}/AzFramework/Physics/ShapeConfiguration.cpp
    ${AZF}/AzFramework/Physics/Collision/CollisionLayers.cpp
    ${AZF}/AzFramework/Physics/CollisionBus.cpp
    ${AZF}/AzFramework/Physics/SystemBus.cpp)
target_include_directories(DarkArisenGameplayProbe PRIVATE ${AZF} ${AZF}/Platform/Linux ${AZF}/Platform/Common
    ${GEM}/Include ${GEM}/Source)
target_link_libraries(DarkArisenGameplayProbe PRIVATE AzCore DarkArisen::Core)
add_test(NAME DarkArisenGameplayProbe COMMAND DarkArisenGameplayProbe)
