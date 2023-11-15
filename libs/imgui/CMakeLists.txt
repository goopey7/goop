# Set the minimum required CMake version
cmake_minimum_required(VERSION 3.12)

project(imgui_lib)

# Add your source files for the "app" here
file(GLOB SRC_FILES "*.cpp" "*.h", "backends/imgui_impl_glfw.cpp", "backends/imgui_impl_glfw.h", "backends/imgui_impl_vulkan.cpp", "backends/imgui_impl_vulkan.h")

# Create a library for the "app" source files
add_library(${PROJECT_NAME} ${SRC_FILES})
