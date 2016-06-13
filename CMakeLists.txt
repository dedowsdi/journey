cmake_minimum_required (VERSION 2.8)

project (Ogre2)

set(OGRE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/dependencies/Ogre)
#prerequisite
IF(NOT EXISTS ${OGRE_SOURCE_DIR})
    message(FATAL_ERROR "dependencies/Ogre not found")
ENDIF()

set(CMAKE_MODULE_PATH
    "${CMAKE_SOURCE_DIR}/cmake"
    "${OGRE_SOURCE_DIR}/CMake"
    "${OGRE_SOURCE_DIR}/CMake/Utils"
    "${OGRE_SOURCE_DIR}/CMake/Packages"
    "${CMAKE_MODULE_PATH}"
    )
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG -D_DEBUG"  )

if( CMAKE_BUILD_TYPE STREQUAL ""  )
    set( CMAKE_BUILD_TYPE Debug CACHE STRING "One of: Debug Release
    RelWithDebInfo MinSizeRel." FORCE  )
endif()

#flag
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -Wall -Wno-unused-local-typedef -Wno-overloaded-virtual -Wno-inconsistent-missing-override -Wno-switch-bool -Wno-int-to-void-pointer-cast -Wno-unused-function -Wno-unknown-pragmas")

#heads and srcs
macro( add_recursive dir retVal  )
    file( GLOB_RECURSE ${retVal} ${dir}/*.h ${dir}/*.cpp ${dir}/*.c  ${dir}/*.hpp )
endmacro()

macro( add_direct dir retVal  )
    file( GLOB ${retVal} ${dir}/*.h ${dir}/*.cpp ${dir}/*.c  ${dir}/*.hpp )
endmacro()


set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(Boost REQUIRED COMPONENTS system )
find_package(SDL2 REQUIRED)

include(${CMAKE_SOURCE_DIR}/cmake/Dependencies/OGRE.cmake)
set(OGRE_SDK /usr/local/ogre/ogre2.1/gcc/Debug CACHE PATH "OGRE SDK")
set(OGRE_SOURCE_DIR /usr/local/source/ogre/ogre2.1 CACHE PATH "ogre source" FORCE)
setupOgre()
include_directories(${OGRE_INCLUDE_DIRS} ${OGRE_SAMPLES_INCLUDE_DIRS} ${SDL2_INCLUDE_DIR} ${Boost_INCLUDE_DIRS} common)

add_subdirectory(common)
add_subdirectory(demo)
