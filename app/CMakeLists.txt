# Set the minimum required CMake version
cmake_minimum_required(VERSION 3.12)

# Define the project name (you can customize this)
project(app_lib)

# Add your source files for the "app" here
file(GLOB_RECURSE APP_SRC_FILES "*.cpp" "*.h")

# Create a library for the "app" source files
add_library(app_lib ${APP_SRC_FILES})
