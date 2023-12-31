cmake_minimum_required(VERSION 3.5)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)
cmake_policy(SET CMP0017 NEW)
# this line has to appear before 'PROJECT' in order to be able to disable
# incremental linking
set(MSVC_INCREMENTAL_DEFAULT ON)

project(BULLET_PHYSICS)
file(STRINGS "VERSION" BULLET_VERSION)

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
  if(POLICY CMP0042)
    # Enable MACOSX_RPATH by default.
    cmake_policy(SET CMP0042 NEW)
  endif(POLICY CMP0042)
endif(COMMAND cmake_policy)

if(NOT CMAKE_BUILD_TYPE)
  # SET(CMAKE_BUILD_TYPE "Debug")
  set(CMAKE_BUILD_TYPE "Release")
endif(NOT CMAKE_BUILD_TYPE)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")
# MESSAGE("CMAKE_CXX_FLAGS_DEBUG="+${CMAKE_CXX_FLAGS_DEBUG})

option(USE_DOUBLE_PRECISION "Use double precision" OFF)
set(CLAMP_VELOCITIES
    "0"
    CACHE
      STRING
      "Clamp rigid bodies' velocity to this value, if larger than zero. Useful to prevent floating point errors or in general runaway velocities in complex scenarios"
)
option(USE_GRAPHICAL_BENCHMARK "Use Graphical Benchmark" OFF)
option(BUILD_SHARED_LIBS "Use shared libraries" OFF)
option(USE_SOFT_BODY_MULTI_BODY_DYNAMICS_WORLD
       "Use btSoftMultiBodyDynamicsWorld" ON)
option(USE_OPENVR "Use OpenVR for virtual reality" OFF)

option(ENABLE_VHACD "Use VHACD in BulletRobotics and pybullet" ON)

option(
  BULLET2_MULTITHREADING
  "Build Bullet 2 libraries with mutex locking around certain operations (required for multi-threading)"
  OFF)
if(BULLET2_MULTITHREADING)
  option(
    BULLET2_USE_OPEN_MP_MULTITHREADING
    "Build Bullet 2 with support for multi-threading with OpenMP (requires a compiler with OpenMP support)"
    OFF)
  option(
    BULLET2_USE_TBB_MULTITHREADING
    "Build Bullet 2 with support for multi-threading with Intel Threading Building Blocks (requires the TBB library to be already installed)"
    OFF)
  if(MSVC)
    option(
      BULLET2_USE_PPL_MULTITHREADING
      "Build Bullet 2 with support for multi-threading with Microsoft Parallel Patterns Library (requires MSVC compiler)"
      OFF)
  endif(MSVC)
endif(BULLET2_MULTITHREADING)

if(NOT WIN32)
  set(DL ${CMAKE_DL_LIBS})
  if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    message("Linux")
    set(OSDEF -D_LINUX)
  else(CMAKE_SYSTEM_NAME MATCHES "Linux")
    if(APPLE)
      message("Apple")
      set(OSDEF -D_DARWIN)
    else(APPLE)
      message("BSD?")
      set(OSDEF -D_BSD)
    endif(APPLE)
  endif(CMAKE_SYSTEM_NAME MATCHES "Linux")
endif(NOT WIN32)

option(USE_MSVC_INCREMENTAL_LINKING "Use MSVC Incremental Linking" OFF)

option(USE_MSVC_RUNTIME_LIBRARY_DLL
       "Use MSVC Runtime Library DLL (/MD or /MDd)" ON)
option(USE_MSVC_RELEASE_RUNTIME_ALWAYS
       "Use MSVC Release Runtime Library even in Debug" OFF)

# SET(CMAKE_EXE_LINKER_FLAGS_INIT    "/STACK:10000000 /INCREMENTAL:NO")
# SET(CMAKE_EXE_LINKER_FLAGS    "/STACK:10000000 /INCREMENTAL:NO")

# MESSAGE("MSVC_INCREMENTAL_YES_FLAG"+${MSVC_INCREMENTAL_YES_FLAG})

if(MSVC)
  if(NOT USE_MSVC_INCREMENTAL_LINKING)
    # MESSAGE("MSVC_INCREMENTAL_DEFAULT"+${MSVC_INCREMENTAL_DEFAULT})
    set(MSVC_INCREMENTAL_YES_FLAG "/INCREMENTAL:NO")

    string(REPLACE "INCREMENTAL:YES" "INCREMENTAL:NO" replacementFlags
                   "${CMAKE_EXE_LINKER_FLAGS_DEBUG}")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "/INCREMENTAL:NO ${replacementFlags}")
    message("CMAKE_EXE_LINKER_FLAGS_DEBUG=${CMAKE_EXE_LINKER_FLAGS_DEBUG}")

    string(REPLACE "INCREMENTAL:YES" "INCREMENTAL:NO" replacementFlags2
                   "${CMAKE_EXE_LINKER_FLAGS}")

    set(CMAKE_EXE_LINKER_FLAGS ${replacementFlag2})
    string(REPLACE "INCREMENTAL:YES" "" replacementFlags3
                   "${CMAKE_EXTRA_LINK_FLAGS}")

    set(CMAKE_EXTRA_LINK_FLAGS ${replacementFlag3})

    string(REPLACE "INCREMENTAL:YES" "INCREMENTAL:NO" replacementFlags3
                   "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO}")
    set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO ${replacementFlags3})
    set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO
        "/INCREMENTAL:NO ${replacementFlags3}")

  endif(NOT USE_MSVC_INCREMENTAL_LINKING)

  if(NOT USE_MSVC_RUNTIME_LIBRARY_DLL)
    # We statically link to reduce dependencies
    foreach(
      flag_var
      CMAKE_CXX_FLAGS
      CMAKE_CXX_FLAGS_DEBUG
      CMAKE_CXX_FLAGS_RELEASE
      CMAKE_CXX_FLAGS_MINSIZEREL
      CMAKE_CXX_FLAGS_RELWITHDEBINFO
      CMAKE_C_FLAGS
      CMAKE_C_FLAGS_DEBUG
      CMAKE_C_FLAGS_RELEASE
      CMAKE_C_FLAGS_MINSIZEREL
      CMAKE_C_FLAGS_RELWITHDEBINFO)
      if(${flag_var} MATCHES "/MD")
        string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
      endif(${flag_var} MATCHES "/MD")
      if(${flag_var} MATCHES "/MDd")
        string(REGEX REPLACE "/MDd" "/MTd" ${flag_var} "${${flag_var}}")
      endif(${flag_var} MATCHES "/MDd")
    endforeach(flag_var)
  endif(NOT USE_MSVC_RUNTIME_LIBRARY_DLL)

  if(USE_MSVC_RELEASE_RUNTIME_ALWAYS)
    foreach(
      flag_var
      CMAKE_CXX_FLAGS
      CMAKE_CXX_FLAGS_DEBUG
      CMAKE_CXX_FLAGS_RELEASE
      CMAKE_CXX_FLAGS_MINSIZEREL
      CMAKE_CXX_FLAGS_RELWITHDEBINFO
      CMAKE_C_FLAGS
      CMAKE_C_FLAGS_DEBUG
      CMAKE_C_FLAGS_RELEASE
      CMAKE_C_FLAGS_MINSIZEREL
      CMAKE_C_FLAGS_RELWITHDEBINFO)
      if(${flag_var} MATCHES "/MDd")
        string(REGEX REPLACE "/MDd" "/MD" ${flag_var} "${${flag_var}}")
      endif(${flag_var} MATCHES "/MDd")
      if(${flag_var} MATCHES "/MTd")
        string(REGEX REPLACE "/MTd" "/MT" ${flag_var} "${${flag_var}}")
      endif(${flag_var} MATCHES "/MTd")
      # Need to remove _DEBUG too otherwise things like _ITERATOR_DEBUG_LEVEL
      # mismatch
      if(${flag_var} MATCHES "-D_DEBUG")
        string(REGEX REPLACE "-D_DEBUG" "" ${flag_var} "${${flag_var}}")
      endif(${flag_var} MATCHES "-D_DEBUG")
    endforeach(flag_var)
  endif(USE_MSVC_RELEASE_RUNTIME_ALWAYS)

  if(CMAKE_CL_64)
    add_definitions(-D_WIN64)
  else()
    option(USE_MSVC_SSE "Use MSVC /arch:sse option" OFF)
    option(USE_MSVC_SSE2 "Compile your program with SSE2 instructions" ON)

    if(USE_MSVC_SSE)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:SSE")
    endif()
    if(USE_MSVC_SSE2)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:SSE2")
    endif()

  endif()

  option(USE_MSVC_AVX "Compile your program with AVX instructions" OFF)

  if(USE_MSVC_AVX)
    add_definitions(/arch:AVX)
  endif()

  option(USE_MSVC_FAST_FLOATINGPOINT "Use MSVC /fp:fast option" ON)
  if(USE_MSVC_FAST_FLOATINGPOINT)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fp:fast")
  endif()

  option(USE_MSVC_STRING_POOLING "Use MSVC /GF string pooling option" ON)
  if(USE_MSVC_STRING_POOLING)
    set(CMAKE_C_FLAGS "/GF ${CMAKE_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "/GF ${CMAKE_CXX_FLAGS}")
  endif()

  option(USE_MSVC_FUNCTION_LEVEL_LINKING
         "Use MSVC /Gy function level linking option" ON)
  if(USE_MSVC_FUNCTION_LEVEL_LINKING)
    set(CMAKE_C_FLAGS "/Gy ${CMAKE_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "/Gy ${CMAKE_CXX_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /OPT:REF")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /OPT:REF")
  endif(USE_MSVC_FUNCTION_LEVEL_LINKING)

  option(USE_MSVC_EXEPTIONS "Use MSVC C++ exceptions option" OFF)

  option(USE_MSVC_COMDAT_FOLDING "Use MSVC /OPT:ICF COMDAT folding option" ON)

  if(USE_MSVC_COMDAT_FOLDING)
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /OPT:ICF")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /OPT:ICF")
  endif()

  option(USE_MSVC_DISABLE_RTTI "Use MSVC /GR- disabled RTTI flags option" ON)
  if(USE_MSVC_DISABLE_RTTI)
    string(REGEX REPLACE "/GR" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}) # Disable
                                                                      # RTTI
    set(CMAKE_C_FLAGS "/GR- ${CMAKE_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "/GR- ${CMAKE_CXX_FLAGS}")
  endif(USE_MSVC_DISABLE_RTTI)

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4244 /wd4267")
endif(MSVC)

if(WIN32)
  option(INTERNAL_CREATE_DISTRIBUTABLE_MSVC_PROJECTFILES
         "Create MSVC projectfiles that can be distributed" OFF)

  if(INTERNAL_CREATE_DISTRIBUTABLE_MSVC_PROJECTFILES)
    set(LIBRARY_OUTPUT_PATH
        ${BULLET_PHYSICS_SOURCE_DIR}/lib
        CACHE PATH "Single output directory for building all libraries.")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BULLET_PHYSICS_SOURCE_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BULLET_PHYSICS_SOURCE_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BULLET_PHYSICS_SOURCE_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${BULLET_PHYSICS_SOURCE_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO
        ${BULLET_PHYSICS_SOURCE_DIR})
  else()
    set(LIBRARY_OUTPUT_PATH
        ${CMAKE_BINARY_DIR}/lib
        CACHE PATH "Single output directory for building all libraries.")
  endif()

  option(INTERNAL_CREATE_MSVC_RELATIVE_PATH_PROJECTFILES
         "Create MSVC projectfiles with relative paths" OFF)
  option(INTERNAL_ADD_POSTFIX_EXECUTABLE_NAMES
         "Add MSVC postfix for executable names (_Debug)" OFF)

  set(CMAKE_DEBUG_POSTFIX
      "_Debug"
      CACHE STRING "Adds a postfix for debug-built libraries.")
  set(CMAKE_MINSIZEREL_POSTFIX
      "_MinsizeRel"
      CACHE STRING "Adds a postfix for MinsizeRelease-built libraries.")
  set(CMAKE_RELWITHDEBINFO_POSTFIX
      "_RelWithDebugInfo"
      CACHE STRING "Adds a postfix for ReleaseWithDebug-built libraries.")

  if(INTERNAL_CREATE_MSVC_RELATIVE_PATH_PROJECTFILES)
    set(CMAKE_SUPPRESS_REGENERATION 1)
    set(CMAKE_USE_RELATIVE_PATHS 1)
  endif(INTERNAL_CREATE_MSVC_RELATIVE_PATH_PROJECTFILES)

endif(WIN32)

option(BUILD_CPU_DEMOS "Build original Bullet CPU examples" OFF)

option(INTERNAL_UPDATE_SERIALIZATION_STRUCTURES
       "Internal update serialization structures" OFF)
if(INTERNAL_UPDATE_SERIALIZATION_STRUCTURES)
  add_definitions(-DBT_INTERNAL_UPDATE_SERIALIZATION_STRUCTURES)
endif(INTERNAL_UPDATE_SERIALIZATION_STRUCTURES)

if(CLAMP_VELOCITIES)
  add_definitions(-DBT_CLAMP_VELOCITY_TO=${CLAMP_VELOCITIES})
endif(CLAMP_VELOCITIES)

if(USE_DOUBLE_PRECISION)
  add_definitions(-DBT_USE_DOUBLE_PRECISION)
  set(BULLET_DOUBLE_DEF "-DBT_USE_DOUBLE_PRECISION")
endif(USE_DOUBLE_PRECISION)

if(NOT USE_SOFT_BODY_MULTI_BODY_DYNAMICS_WORLD)
  add_definitions(-DSKIP_SOFT_BODY_MULTI_BODY_DYNAMICS_WORLD)
endif()

if(USE_GRAPHICAL_BENCHMARK)
  add_definitions(-DUSE_GRAPHICAL_BENCHMARK)
endif(USE_GRAPHICAL_BENCHMARK)

if(BULLET2_MULTITHREADING)
  add_definitions(-DBT_THREADSAFE=1)
  if(NOT MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  endif(NOT MSVC)
  if(NOT WIN32)
    find_package(Threads)
    link_libraries(${CMAKE_THREAD_LIBS_INIT})
  endif(NOT WIN32)
endif(BULLET2_MULTITHREADING)

if(BULLET2_USE_OPEN_MP_MULTITHREADING)
  add_definitions("-DBT_USE_OPENMP=1")
  if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /openmp")
  else(MSVC)
    # GCC, Clang
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
  endif(MSVC)
endif(BULLET2_USE_OPEN_MP_MULTITHREADING)

if(BULLET2_USE_TBB_MULTITHREADING)
  set(BULLET2_TBB_INCLUDE_DIR
      "not found"
      CACHE PATH "Directory for Intel TBB includes.")
  set(BULLET2_TBB_LIB_DIR
      "not found"
      CACHE PATH "Directory for Intel TBB libraries.")
  find_library(TBB_LIBRARY tbb PATHS ${BULLET2_TBB_LIB_DIR})
  find_library(TBBMALLOC_LIBRARY tbbmalloc PATHS ${BULLET2_TBB_LIB_DIR})
  add_definitions("-DBT_USE_TBB=1")
  include_directories(${BULLET2_TBB_INCLUDE_DIR})
  link_libraries(${TBB_LIBRARY} ${TBBMALLOC_LIBRARY})
endif(BULLET2_USE_TBB_MULTITHREADING)

if(BULLET2_USE_PPL_MULTITHREADING)
  add_definitions("-DBT_USE_PPL=1")
endif(BULLET2_USE_PPL_MULTITHREADING)

if(WIN32)
  option(USE_GLUT "Use Glut" ON)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
  add_definitions(-D_SCL_SECURE_NO_WARNINGS)

  if(USE_GLUT AND MSVC)
    string(REPLACE "/D_WINDOWS" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    remove_definitions(-D_WINDOWS)
  endif()

else(WIN32)
  option(USE_GLUT "Use Glut" ON)
endif(WIN32)

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

if(APPLE)
  find_library(COCOA_LIBRARY Cocoa)
endif()

option(BUILD_BULLET3 "Set when you want to build Bullet 3" ON)

option(BUILD_ENET
       "Set when you want to build apps with enet UDP networking support" ON)
option(BUILD_CLSOCKET
       "Set when you want to build apps with enet TCP networking support" ON)

subdirs(src)

if("${CMAKE_GENERATOR}" MATCHES "Unix Makefiles")
  option(INSTALL_LIBS "Set when you want to install libraries" ON)
else()
  if(APPLE AND FRAMEWORK)
    option(INSTALL_LIBS "Set when you want to install libraries" ON)
  else()
    # by default, don't enable the 'INSTALL' option for Xcode and MSVC
    # projectfiles
    option(INSTALL_LIBS "Set when you want to install libraries" OFF)
  endif()
endif()

if(INSTALL_LIBS)
  # INSTALL of other files requires CMake 2.6
  if(BUILD_EXTRAS)
    if(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 2.5)
      option(INSTALL_EXTRA_LIBS "Set when you want extra libraries installed"
             ON)
    endif(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 2.5)
  endif(BUILD_EXTRAS)

  set(LIB_SUFFIX
      ""
      CACHE STRING "Define suffix of directory name (32/64)")
  set(LIB_DESTINATION
      "lib${LIB_SUFFIX}"
      CACHE STRING "Library directory name")
  # the following are directories where stuff will be installed to
  set(INCLUDE_INSTALL_DIR
      "include/bullet/"
      CACHE PATH "The subdirectory to the header prefix")
  set(PKGCONFIG_INSTALL_PREFIX
      "lib${LIB_SUFFIX}/pkgconfig/"
      CACHE STRING "Base directory for pkgconfig files")
  if(NOT MSVC)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/bullet.pc.cmake
                   ${CMAKE_CURRENT_BINARY_DIR}/bullet.pc @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/bullet.pc
            DESTINATION ${PKGCONFIG_INSTALL_PREFIX})
  endif(NOT MSVC)
endif()

set(BULLET_CONFIG_CMAKE_PATH lib${LIB_SUFFIX}/cmake/bullet)
list(APPEND BULLET_DEFINITIONS ${BULLET_DOUBLE_DEF})
list(APPEND BULLET_LIBRARIES LinearMath)
list(APPEND BULLET_LIBRARIES Bullet3Common)
list(APPEND BULLET_LIBRARIES BulletInverseDynamics)
list(APPEND BULLET_LIBRARIES BulletCollision)
list(APPEND BULLET_LIBRARIES BulletDynamics)
list(APPEND BULLET_LIBRARIES BulletSoftBody)
set(BULLET_USE_FILE ${BULLET_CONFIG_CMAKE_PATH}/UseBullet.cmake)
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/BulletConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/BulletConfig.cmake @ONLY ESCAPE_QUOTES)
option(INSTALL_CMAKE_FILES "Install generated CMake files" OFF)

if(INSTALL_CMAKE_FILES)
  install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/UseBullet.cmake
                ${CMAKE_CURRENT_BINARY_DIR}/BulletConfig.cmake
          DESTINATION ${BULLET_CONFIG_CMAKE_PATH})
endif(INSTALL_CMAKE_FILES)
