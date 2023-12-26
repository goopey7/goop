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

# include volk headers
include_directories("libs/volk/")

# include vulkan headers
include_directories("libs/vulkan/include")

# include imgui headers
include_directories("libs/imgui/")
include_directories("libs/imgui/backends")

# include entt headers
include_directories("libs/entt/")

# Add engine source files
file(GLOB_RECURSE ENGINE_SRC_FILES "goop/*.cpp" "goop/*.h")
file(GLOB STB_IMAGE_HEADER "libs/stb/stb_image.h")

# Define the C++ executable
add_executable(${PROJECT_NAME} ${ENGINE_SRC_FILES} ${STB_VORBIS_SOURCE})

#define app type
target_compile_definitions(${PROJECT_NAME} PRIVATE GOOP_APPTYPE_EDITOR=1)

# define systems
target_compile_definitions(${PROJECT_NAME} PRIVATE GOOP_RENDERER_VULKAN=1)
target_compile_definitions(${PROJECT_NAME} PRIVATE GOOP_WINDOW_GLFW=1)
target_compile_definitions(${PROJECT_NAME} PRIVATE GOOP_AUDIO_SOLOUD=1)
target_compile_definitions(${PROJECT_NAME} PRIVATE GOOP_MESHLOADER_ASSIMP=1)

# ##################### Build Soloud
# Add a custom target to build Soloud using CMake
add_custom_target(
  build_soloud
  SOURCES ${SOLOUD_SOURCES}
  COMMENT "Building SoLoud...")

# Add a dependency to ensure that Soloud is built before engine code
add_dependencies(${PROJECT_NAME} build_soloud)

# ##################### Build rust library Find the cargo executable
find_program(CARGO_EXECUTABLE cargo)

if(NOT CARGO_EXECUTABLE)
  message(
    FATAL_ERROR "Cargo not found. Make sure it is installed and in your PATH.")
endif()

# Add a custom target to build your Rust project using cargo
add_custom_target(
  build_rust_project
  COMMAND ${CARGO_EXECUTABLE} build --release
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/rust_lib
  COMMENT "Building Rust project...")

include_directories("rust_lib/include")

# Add a dependency to ensure that the Rust project is built before your C++
# project
add_dependencies(${PROJECT_NAME} build_rust_project)

if(WIN32)
  set(RUST_LIB
      ${CMAKE_SOURCE_DIR}/rust_lib/target/release/rust_hello_lib.dll.lib)
else()
  set(RUST_LIB
      ${CMAKE_SOURCE_DIR}/rust_lib/target/release/librust_hello_lib.so)
endif()

# Link the C++ executable
add_subdirectory(libs/volk)
add_subdirectory(editor)
add_subdirectory(game)
add_subdirectory(libs/imgui)
target_link_libraries(${PROJECT_NAME} PRIVATE volk ${SDL2_LIBRARY} ${RUST_LIB} ${GLFW3_LIBRARY}
	${ASSIMP_LIBRARY} SoLoud imgui_lib app_lib game_lib)
# ##############################################################################

# ##################### Build shaders

if(WIN32)
	find_program(GLSLC_EXECUTABLE NAMES glslc
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
  COMMAND ${GLSLC_EXECUTABLE} -o ${CMAKE_BINARY_DIR}/shaders/shader_editor.frag.spv
          ${CMAKE_SOURCE_DIR}/shaders/shader_editor.frag
  COMMAND ${GLSLC_EXECUTABLE} -o ${CMAKE_BINARY_DIR}/shaders/shader_game.frag.spv
          ${CMAKE_SOURCE_DIR}/shaders/shader_game.frag
  COMMAND ${GLSLC_EXECUTABLE} -o ${CMAKE_SOURCE_DIR}/shaders/shader.vert.spv
          ${CMAKE_SOURCE_DIR}/shaders/shader.vert
  COMMAND ${GLSLC_EXECUTABLE} -o ${CMAKE_SOURCE_DIR}/shaders/shader_editor.frag.spv
          ${CMAKE_SOURCE_DIR}/shaders/shader_editor.frag
  COMMAND ${GLSLC_EXECUTABLE} -o ${CMAKE_SOURCE_DIR}/shaders/shader_game.frag.spv
          ${CMAKE_SOURCE_DIR}/shaders/shader_game.frag
  COMMENT "Compiling GLSL shaders...")

# Specify the output directory for shader files
set(SHADER_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/shaders)

# Add the custom shader target as a dependency for your C++ project
add_dependencies(${PROJECT_NAME} compile_shaders)
# ##############################################################################

# Copy the rust_hello_lib.dll to the build directory where the .exe goes
# And copy SDL2.dll
if(WIN32)
  add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND
      ${CMAKE_COMMAND} -E copy
      ${CMAKE_SOURCE_DIR}/rust_lib/target/release/rust_hello_lib.dll
      $<TARGET_FILE_DIR:${PROJECT_NAME}>)
  add_custom_command(
	  TARGET ${PROJECT_NAME}
	  POST_BUILD
	  COMMAND
	  ${CMAKE_COMMAND} -E copy
	  ${CMAKE_SOURCE_DIR}/windows/sdl2/lib/SDL2.dll
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
	  COMMAND
	  ${CMAKE_COMMAND} -E copy_directory
	  ${CMAKE_SOURCE_DIR}/res
	  $<TARGET_FILE_DIR:${PROJECT_NAME}>/res)
  add_custom_command(
	  TARGET ${PROJECT_NAME}
	  POST_BUILD
	  COMMAND
	  ${CMAKE_COMMAND} -E copy_directory
	  ${CMAKE_SOURCE_DIR}/res
	  ${CMAKE_BINARY_DIR}/res)
  add_custom_command(
	  TARGET ${PROJECT_NAME}
	  POST_BUILD
	  COMMAND
	  ${CMAKE_COMMAND} -E copy_directory
	  ${CMAKE_SOURCE_DIR}/shaders
	  $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders)

endif()
