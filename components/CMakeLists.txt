# Set the minimum required CMake version
cmake_minimum_required(VERSION 3.12)

project(components_lib)

file(GLOB_RECURSE COMPONENTS_SRC_FILES "*.cpp" "*.h")

add_library(${PROJECT_NAME} ${COMPONENTS_SRC_FILES})
target_compile_definitions(${PROJECT_NAME} PRIVATE GOOP_RENDERER_VULKAN)

