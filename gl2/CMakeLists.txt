cmake_minimum_required (VERSION 2.6)

project (gl2)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")
include(util)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG -D_DEBUG"  )

if( CMAKE_BUILD_TYPE STREQUAL ""  )
    set( CMAKE_BUILD_TYPE Debug CACHE STRING "One of: Debug Release
    RelWithDebInfo MinSizeRel." FORCE  )
endif()

IF(CMAKE_C_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fstandalone-debug -pthread -Wall -Wno-unused-function -Wno-switch-bool")
ENDIF()

IF(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstandalone-debug -pthread -Wall -Wno-unused-function -Wno-switch-bool -std=c++11 -Wno-missing-braces -Wno-writable-strings -Wno-reorder")
    
ENDIF()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
#this variable will be used at findOSG
set(CMAKE_DEBUG_POSTFIX d CACHE STRING "i need to get this at build file, so it's cached")

#becareful cmake prebuilt find## use smallcase
FIND_PACKAGE(GLUT REQUIRED)
FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(Freetype REQUIRED)

findLibraryRequird(FIP_LIBS freeimageplus)
findPathRequird(FIP_INCLUDE_DIR FreeImagePlus.h)

include_directories(
    ${GLUT_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIR}
    ${FREETYPE_INCLUDE_DIR}
    ${FREETYPE_INCLUDE_DIR_ft2build}
    ${FIP_INCLUDE_DIR}
    common/include
    ../glc/include
    gl21/include
    )

IF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_LINUX")
ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

file(COPY data DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

#generate cfg file

add_subdirectory(gl21)
add_subdirectory(glc)
add_subdirectory(common)
add_subdirectory(demo)
add_subdirectory(effect)
add_subdirectory(util)
