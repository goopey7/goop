# Set the minimum required CMake version
cmake_minimum_required(VERSION 3.12)

project(editor_lib)

# Add your source files for the "app" here
file(GLOB_RECURSE APP_SRC_FILES "*.cpp" "*.h")

# Create a library for the "app" source files
add_library(${PROJECT_NAME} ${APP_SRC_FILES})

include_directories("../libs/json/")
target_compile_definitions(${PROJECT_NAME} PRIVATE GOOP_RENDERER_VULKAN)
