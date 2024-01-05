cmake_minimum_required(VERSION 3.12)
project(goop)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

include_directories(".")

# Find GLFW package
find_package(GLFW3 REQUIRED)
include_directories(${GLFW3_INCLUDE_DIR})

# Find SDL package
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

# Find GLM package
find_package(GLM REQUIRED)
include_directories(${GLM_INCLUDE_DIR})

# Find Assimp package
find_package(ASSIMP REQUIRED)
include_directories(${ASSIMP_INCLUDE_DIR})

# Include soloud headers
add_subdirectory(libs/soloud)
include_directories("libs/soloud/include")

# Include STB headers
include_directories("libs/stb/")

# include vulkan headers
include_directories("libs/vulkan/include")

# include volk
include_directories("libs/volk/")
add_subdirectory(libs/volk)

# include imgui
include_directories("libs/imgui/")
include_directories("libs/imgui/backends")
add_subdirectory(libs/imgui)

# include bullet
include_directories("libs/bullet3/src")
add_subdirectory(libs/bullet3)

# include sha256
include_directories("libs/sha256/")

# include entt headers
include_directories("libs/entt/")

# include json headers
include_directories("libs/json/")

# Add engine source files
file(GLOB_RECURSE ENGINE_SRC_FILES "goop/*.cpp" "goop/*.h")
file(GLOB STB_IMAGE_HEADER "libs/stb/stb_image.h")

# Define the C++ executable
add_executable(${PROJECT_NAME} ${ENGINE_SRC_FILES} ${STB_VORBIS_SOURCE})

# define app type
target_compile_definitions(${PROJECT_NAME} PRIVATE GOOP_APPTYPE_EDITOR)

# define systems
target_compile_definitions(${PROJECT_NAME} PRIVATE GOOP_RENDERER_VULKAN)
target_compile_definitions(${PROJECT_NAME} PRIVATE GOOP_WINDOW_GLFW)
target_compile_definitions(${PROJECT_NAME} PRIVATE GOOP_AUDIO_SOLOUD)
target_compile_definitions(${PROJECT_NAME} PRIVATE GOOP_MESHLOADER_ASSIMP)
target_compile_definitions(${PROJECT_NAME} PRIVATE GOOP_IMGLOADER_STB)
target_compile_definitions(${PROJECT_NAME} PRIVATE GOOP_PHYSICS_BULLET)

# ##################### Build Soloud Add a custom target to build Soloud using
# CMake
add_custom_target(
  build_soloud
  SOURCES ${SOLOUD_SOURCES}
  COMMENT "Building SoLoud...")

# Add a dependency to ensure that Soloud is built before engine code
add_dependencies(${PROJECT_NAME} build_soloud)

# Link the C++ executable
add_subdirectory(editor)
add_subdirectory(game)
add_subdirectory(components)
target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE volk
          ${SDL2_LIBRARY}
          ${GLFW3_LIBRARY}
          ${ASSIMP_LIBRARY}
          SoLoud
          imgui_lib
          editor_lib
          game_lib
          components_lib
          BulletDynamics
          BulletCollision
          LinearMath)
# ##############################################################################

# ##################### Build shaders

if(WIN32)
  find_program(
    GLSLC_EXECUTABLE
    NAMES glslc
    PATHS ${CMAKE_SOURCE_DIR}/windows/glslc)
else()
  find_program(GLSLC_EXECUTABLE glslc)
endif()
if(NOT GLSLC_EXECUTABLE)
  message(
    FATAL_ERROR "glslc not found. Make sure it is installed and in your PATH.")
endif()

# Create the "shaders" directory before compiling shaders
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/shaders)

# Add a custom target to compile GLSL shaders
add_custom_target(
  compile_shaders
  COMMAND ${GLSLC_EXECUTABLE} -o ${CMAKE_BINARY_DIR}/shaders/shader.vert.spv
          ${CMAKE_SOURCE_DIR}/shaders/shader.vert
  COMMAND
    ${GLSLC_EXECUTABLE} -o ${CMAKE_BINARY_DIR}/shaders/shader_editor.frag.spv
    ${CMAKE_SOURCE_DIR}/shaders/shader_editor.frag
  COMMAND
    ${GLSLC_EXECUTABLE} -o ${CMAKE_BINARY_DIR}/shaders/shader_game.frag.spv
    ${CMAKE_SOURCE_DIR}/shaders/shader_game.frag
  COMMAND ${GLSLC_EXECUTABLE} -o ${CMAKE_SOURCE_DIR}/shaders/shader.vert.spv
          ${CMAKE_SOURCE_DIR}/shaders/shader.vert
  COMMAND
    ${GLSLC_EXECUTABLE} -o ${CMAKE_SOURCE_DIR}/shaders/shader_editor.frag.spv
    ${CMAKE_SOURCE_DIR}/shaders/shader_editor.frag
  COMMAND
    ${GLSLC_EXECUTABLE} -o ${CMAKE_SOURCE_DIR}/shaders/shader_game.frag.spv
    ${CMAKE_SOURCE_DIR}/shaders/shader_game.frag
  COMMENT "Compiling GLSL shaders...")

# Specify the output directory for shader files
set(SHADER_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/shaders)

# Add the custom shader target as a dependency for your C++ project
add_dependencies(${PROJECT_NAME} compile_shaders)
# ##############################################################################

# copy SDL2.dll
if(WIN32)
  add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND
      ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/windows/sdl2/lib/SDL2.dll
      $<TARGET_FILE_DIR:${PROJECT_NAME}>)
  add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND
      ${CMAKE_COMMAND} -E copy
      ${CMAKE_SOURCE_DIR}/windows/assimp/lib/assimp-vc143-mt.dll
      $<TARGET_FILE_DIR:${PROJECT_NAME}>)
  add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/res)
  add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res
            ${CMAKE_BINARY_DIR}/res)
  add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/shaders
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders)

endif()

add_custom_target(update_components
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/components/updateComponents.sh
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Running updateComponents.sh script"
)
add_dependencies(${PROJECT_NAME} update_components)

