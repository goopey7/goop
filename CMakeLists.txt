cmake_minimum_required(VERSION 3.12)
project(goop)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

include_directories(".")

# Find Vulkan package
find_package(Vulkan REQUIRED)

# Find GLFW package
find_package(GLFW3 REQUIRED)
include_directories(${GLFW3_INCLUDE_DIR})

# Include Vulkan headers
include_directories(${Vulkan_INCLUDE_DIRS})

# Recursively glob all .cpp and .h files in src/ directory
file(GLOB_RECURSE SRC_FILES "goop/*.cpp" "goop/*.h")

# Define the C++ executable
add_executable(${PROJECT_NAME} ${SRC_FILES})

# define systems
target_compile_definitions(${PROJECT_NAME} PRIVATE RENDERER_VULKAN=1)
target_compile_definitions(${PROJECT_NAME} PRIVATE WINDOW_GLFW=1)

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

# Link the C++ executable with the Rust dynamic library
target_link_libraries(${PROJECT_NAME} PRIVATE ${RUST_LIB} ${GLFW3_LIBRARY}
                                              ${Vulkan_LIBRARIES})
# ##############################################################################

# ##################### Build shaders
find_program(GLSLC_EXECUTABLE glslc)
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
  COMMAND ${GLSLC_EXECUTABLE} -o ${CMAKE_BINARY_DIR}/shaders/shader.frag.spv
          ${CMAKE_SOURCE_DIR}/shaders/shader.frag
  COMMAND ${GLSLC_EXECUTABLE} -o ${CMAKE_SOURCE_DIR}/shaders/shader.vert.spv
          ${CMAKE_SOURCE_DIR}/shaders/shader.vert
  COMMAND ${GLSLC_EXECUTABLE} -o ${CMAKE_SOURCE_DIR}/shaders/shader.frag.spv
          ${CMAKE_SOURCE_DIR}/shaders/shader.frag
  COMMENT "Compiling GLSL shaders...")

# Specify the output directory for shader files
set(SHADER_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/shaders)

# Add the custom shader target as a dependency for your C++ project
add_dependencies(${PROJECT_NAME} compile_shaders)
# ##############################################################################

# Copy the rust_hello_lib.dll to the build directory where the .exe goes
if(WIN32)
  add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND
      ${CMAKE_COMMAND} -E copy
      ${CMAKE_SOURCE_DIR}/rust_lib/target/release/rust_hello_lib.dll
      $<TARGET_FILE_DIR:${PROJECT_NAME}>)
endif()
