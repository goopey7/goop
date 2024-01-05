# Set the minimum required CMake version
cmake_minimum_required(VERSION 3.12)

project(game_lib)

file(GLOB_RECURSE GAME_SRC_FILES "*.cpp" "*.h")

add_library(${PROJECT_NAME} ${GAME_SRC_FILES})

if(WIN32)
add_custom_target(game_update_components
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/../components/updateComponents.bat
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Running updateComponents.bat script"
)
else()
add_custom_target(game_update_components
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/../components/updateComponents.sh
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Running updateComponents.sh script"
)
endif()

add_dependencies(${PROJECT_NAME} game_update_components)

include_directories("../libs/json")
target_compile_definitions(${PROJECT_NAME} PRIVATE GOOP_RENDERER_VULKAN)
