# Locate the stb library
#
# This module defines the following variables:
#
# STB_INCLUDE_DIR where to find stb include files.
# STB_FOUND true if STB_INCLUDE_DIR has been found.
# STB_VORBIS_SOURCE the path to the stb_vorbis source file.
#
# To help locate the library and include file, you can define a
# variable called STB_ROOT which points to the root of the stb library
# installation.
#
# default search dirs
#
# Cmake file adapted from: https://github.com/daw42/glslcookbook

# Define the search directories
set(_stb_HEADER_SEARCH_DIRS
  "/usr/include"
  "/usr/local/include"
  "${CMAKE_SOURCE_DIR}/includes"
  "/usr/include/stb"  # Add this line to include the stb headers
)

# Check environment for root search directory
set(_stb_ENV_ROOT $ENV{STB_ROOT})
if(NOT STB_ROOT AND _stb_ENV_ROOT)
  set(STB_ROOT ${_stb_ENV_ROOT})
endif()

# Put user-specified location at the beginning of search
if(STB_ROOT)
  list(INSERT _stb_HEADER_SEARCH_DIRS 0 "${STB_ROOT}/include")
endif()

# Search for the stb_vorbis header
FIND_PATH(STB_INCLUDE_DIR "stb/stb_vorbis.c"
  PATHS ${_stb_HEADER_SEARCH_DIRS}
)

# Set the STB_VORBIS_SOURCE variable
set(STB_VORBIS_SOURCE "/usr/include/stb/stb_vorbis.c")

# Include the FindPackageHandleStandardArgs module
include(FindPackageHandleStandardArgs)

# Set the package variables
set(STB_INCLUDE_DIR "${STB_INCLUDE_DIR}")
set(STB_FOUND TRUE)

