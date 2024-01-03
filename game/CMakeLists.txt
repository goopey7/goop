# Set the minimum required CMake version
cmake_minimum_required(VERSION 3.12)

project(game_lib)

file(GLOB_RECURSE GAME_SRC_FILES "*.cpp" "*.h")

add_library(${PROJECT_NAME} ${GAME_SRC_FILES})
include_directories("../libs/json")
target_compile_definitions(${PROJECT_NAME} PRIVATE GOOP_RENDERER_VULKAN)
