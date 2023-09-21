cmake_minimum_required(VERSION 3.12)
project(goop)

# Set the C++ standard to C++11 or later
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_COMPILER g++)

# Define the C++ executable
add_executable(${PROJECT_NAME} main.cpp)

# Build rust library

# Find the cargo executable
find_program(CARGO_EXECUTABLE cargo)

if(NOT CARGO_EXECUTABLE)
    message(FATAL_ERROR "Cargo not found. Make sure it is installed and in your PATH.")
endif()

# Add a custom target to build your Rust project using cargo
add_custom_target(build_rust_project
    COMMAND ${CARGO_EXECUTABLE} build --release
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/rust_lib
    COMMENT "Building Rust project..."
)

include_directories("rust_lib/include")

# Add a dependency to ensure that the Rust project is built before your C++ project
add_dependencies(${PROJECT_NAME} build_rust_project)

# Link the C++ executable with the Rust dynamic library
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    ${CMAKE_SOURCE_DIR}/rust_lib/target/release/librust_hello_lib.so
)
