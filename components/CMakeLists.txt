cmake_minimum_required(VERSION 3.12)

project(components_lib)

file(GLOB_RECURSE COMPONENTS_CPP_FILES "*.cpp")
file(GLOB_RECURSE COMPONENTS_H_FILES "*.h")

if (COMPONENTS_CPP_FILES)
    add_library(${PROJECT_NAME} ${COMPONENTS_CPP_FILES} ${COMPONENTS_H_FILES})
    target_compile_definitions(${PROJECT_NAME} PRIVATE GOOP_RENDERER_VULKAN)
else()
    add_library(${PROJECT_NAME} INTERFACE)
    target_sources(${PROJECT_NAME} INTERFACE ${COMPONENTS_H_FILES})
endif()

