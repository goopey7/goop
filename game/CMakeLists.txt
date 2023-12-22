# Set the minimum required CMake version
cmake_minimum_required(VERSION 3.12)

project(game_lib)

file(GLOB_RECURSE GAME_SRC_FILES "*.cpp" "*.h")
add_library(${PROJECT_NAME} ${GAME_SRC_FILES})
