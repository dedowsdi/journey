cmake_minimum_required (VERSION 3.0)

project (c++)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON )

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")
include(util)


if( CMAKE_BUILD_TYPE STREQUAL ""  )
    set( CMAKE_BUILD_TYPE Debug CACHE STRING "One of: Debug Release
    RelWithDebInfo MinSizeRel." FORCE  )
endif()

#flag
message("CMAKE_C_COMPILER ${CMAKE_C_COMPILER_ID}")
message("CMAKE_CXX_COMPILER ${CMAKE_CXX_COMPILER_ID}")

IF(CMAKE_C_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fstandalone-debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstandalone-debug -D_COMPILER_CLANG")
ENDIF()

IF(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # use drawf debug format for gdb, use -g3 to include macro
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -gdwarf -g3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -gdwarf -g3 -D_COMPILER_GCC")

    set(CXX_COMPILER "GCC")
ENDIF()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread -Wall -DDEBUG -D_DEBUG")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# doens' work on executable
set(CMAKE_DEBUG_POSTFIX d CACHE STRING "i need to get this at build file, so it's cached")

FIND_PACKAGE(PkgConfig REQUIRED)

find_package(Boost REQUIRED COMPONENTS program_options filesystem unit_test_framework)
message(${Boost_LIBRARIES})

include_directories(
    ${CMAKE_SOURCE_DIR}/common/include
    ${Boost_INCLUDE_DIRS}
    )

IF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_LINUX")
ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

file(COPY data DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

add_subdirectory(common)
set(COMMON_LIBS common)
add_subdirectory(basic)
add_subdirectory(algorithm)
add_subdirectory(util)
add_subdirectory(pattern)
add_subdirectory(trick)
add_subdirectory(macro)
add_subdirectory(boost)
add_subdirectory(quiz)
add_subdirectory(idiom)
add_subdirectory(task)
