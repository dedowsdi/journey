cmake_minimum_required(VERSION 3.5)

# old policy for FindOpenGL
cmake_policy(SET CMP0072 OLD)

project (gl4 VERSION 0.0 LANGUAGES C CXX)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")
include(util)

# Require out-of-source builds
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()

set(DEFAULT_BUILD_TYPE "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)

  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

option(BUILD_SHARED_LIBS "build shared instead of static" OFF)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG -D_DEBUG"  )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread")


IF(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fstandalone-debug -pthread -Wall -Wno-unused-function -Wno-switch-bool")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstandalone-debug -pthread -Wall -Wno-unused-function -Wno-switch-bool -Wno-missing-braces -Wno-writable-strings -Wno-reorder")
ENDIF()

IF(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # use drawf debug format for gdb, use -g3 to include macro
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -gdwarf -g3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -gdwarf -g3")
ENDIF()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#this variable will be used at findOSG
set(CMAKE_DEBUG_POSTFIX d CACHE STRING "i need to get this at build file, so it's cached")
FIND_PACKAGE(PkgConfig REQUIRED)

FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(Freetype REQUIRED)
PKG_CHECK_MODULES(GLFW3 REQUIRED glfw3)
pkg_get_variable(GLFW3_REQUIRES glfw3 Requires.private)

findLibraryRequird(FIP_LIBS freeimageplus)
findPathRequird(FIP_INCLUDE_DIR FreeImagePlus.h)
findPathRequird(RAPIDJSON_INCLUDE_DIR rapidjson.h rapidjson)

IF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_LINUX")
ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

#file(COPY data DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

#generate cfg file

add_subdirectory(gl43)
SET(GLC_GL_VERSION 43)
add_subdirectory(glc)
add_subdirectory(gl43app)
add_subdirectory(demo)
add_subdirectory(util)
add_subdirectory(challenge)
add_subdirectory(plant)
