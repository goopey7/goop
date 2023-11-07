# Set the minimum required CMake version
cmake_minimum_required(VERSION 3.12)

project(app_lib)

# Add your source files for the "app" here
file(GLOB_RECURSE APP_SRC_FILES "*.cpp" "*.h")

# Create a library for the "app" source files
add_library(${PROJECT_NAME} ${APP_SRC_FILES})
