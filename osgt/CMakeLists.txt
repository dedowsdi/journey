cmake_minimum_required(VERSION 3.8)

project(whackamole VERSION 0.0 LANGUAGES CXX C)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(DEFAULT_BUILD_TYPE "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)

    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

option(PRINT_STAGE "Print rener stage in DebugHander" OFF)

if(PRINT_STAGE)
    add_definitions(-DPRINT_STAGE)
endif()

find_package(OpenGL REQUIRED)
find_package(Fontconfig REQUIRED)
find_package(OpenSceneGraph REQUIRED COMPONENTS
    osgViewer
    osgText
    osgAnimation
    osgParticle
    osgFX
    osgShadow
    osgGA
    osgDB
    osgUtil
    osgTerrain
    )
find_package(PkgConfig REQUIRED)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# You can only add one option at a time???
add_compile_options($<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:GNU>>:-gdwarf>)
add_compile_options($<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:GNU>>:-g3>)
add_compile_options($<$<CONFIG:Debug>:-DDEBUG>)

add_subdirectory(common)
add_subdirectory(rt_basic_sphere)
add_subdirectory(distortion)
add_subdirectory(water)
