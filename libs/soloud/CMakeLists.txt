# CMakeLists.txt in libs/soloud
cmake_minimum_required(VERSION 3.27)
project(SoLoud)

# Include all source files in the Soloud directory
file(GLOB_RECURSE SOLOUD_SOURCES "*.cpp" "*.h")
file(GLOB STB_VORBIS_SOURCE "/usr/include/stb/stb_vorbis.c")

# Create a library target for Soloud
add_library(${PROJECT_NAME} STATIC ${SOLOUD_SOURCES} ${STB_VORBIS_SOURCE})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Find stb package
find_package(STB REQUIRED)
include_directories(${STB_INCLUDE_DIRS})

# Find SDL package
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${SDL2_INCLUDE_DIRS}/SDL)


target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
target_compile_definitions(${PROJECT_NAME} PRIVATE WITH_SDL2_STATIC=1)
