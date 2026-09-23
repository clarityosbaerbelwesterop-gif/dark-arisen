# Native O3DE operations owner for Dark Arisen (no engine dependency).
cmake_minimum_required(VERSION 3.22)
project(DarkArisenO3DEOps LANGUAGES CXX)

if(MSVC)
    # MSVC is verified in CI only; warnings stay visible but are not fatal until a Windows gate exists.
    set(DARKARISEN_TOOL_WARNINGS /W4 /permissive-)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    set(DARKARISEN_TOOL_WARNINGS -Wall -Wextra -Wpedantic -Werror)
endif()

add_library(DarkArisenAssetPipeline STATIC Ops/AssetPipeline.cpp)
target_include_directories(DarkArisenAssetPipeline PUBLIC Ops)
target_compile_features(DarkArisenAssetPipeline PUBLIC cxx_std_20)
target_compile_options(DarkArisenAssetPipeline PRIVATE ${DARKARISEN_TOOL_WARNINGS})

add_executable(DarkArisenO3DE Ops/DarkArisenO3DE.cpp)
target_link_libraries(DarkArisenO3DE PRIVATE DarkArisenAssetPipeline)
target_compile_options(DarkArisenO3DE PRIVATE ${DARKARISEN_TOOL_WARNINGS})

enable_testing()
add_executable(DarkArisenAssetPipelineTests Ops/AssetPipelineTests.cpp)
target_link_libraries(DarkArisenAssetPipelineTests PRIVATE DarkArisenAssetPipeline)
target_compile_options(DarkArisenAssetPipelineTests PRIVATE ${DARKARISEN_TOOL_WARNINGS})
add_test(NAME DarkArisenAssetPipelineTests COMMAND DarkArisenAssetPipelineTests)
