# Set the source files for the "goop" library
file(GLOB_RECURSE GOOP_SRC_FILES "*.cpp" "*.h")

# Create the "goop" library
add_library(goop ${GOOP_SRC_FILES})

# Include the "sys" directory in the interface so that it can be accessed via #include <goop/sys/...>
target_include_directories(goop INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

# Optionally, set compile options and definitions for the "goop" library here
# target_compile_definitions(goop PRIVATE SOME_DEFINITION)

# Specify any dependencies for the "goop" library here
# target_link_libraries(goop PRIVATE some_dependency)

# Export the "goop" library for use in the main project
export(TARGETS goop FILE "${CMAKE_BINARY_DIR}/goop-targets.cmake")

