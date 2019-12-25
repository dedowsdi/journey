cmake_minimum_required (VERSION 3.8)

# old policy for FindOpenGL
# cmake_policy(SET CMP0072 OLD)

project (osg)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
include(util)

if( CMAKE_BUILD_TYPE STREQUAL ""  )
    set( CMAKE_BUILD_TYPE Debug CACHE STRING "One of: Debug Release
    RelWithDebInfo MinSizeRel." FORCE  )
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG -D_DEBUG"  )
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DSOURCE=\\\"${CMAKE_CURRENT_SOURCE_DIR}\\\""  )

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#this variable will be used at findOSG
# set(CMAKE_DEBUG_POSTFIX d CACHE STRING "i need to get this at build file, so it's cached")
set(CMAKE_DEBUG_POSTFIX d)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

FIND_PACKAGE(PkgConfig REQUIRED)

# FIND_PACKAGE(OpenThreads)
# FIND_PACKAGE(osg)
# FIND_PACKAGE(osgDB)
# FIND_PACKAGE(osgUtil)
# FIND_PACKAGE(osgGA)
# FIND_PACKAGE(osgViewer)

FIND_PACKAGE(Fontconfig REQUIRED)
FIND_PACKAGE(OpenSceneGraph REQUIRED COMPONENTS
    osgViewer
    osgAnimation
    osgParticle
    osgFX
    osgShadow
    osgWidget
    osgDB
    osgUtil
    osgText
    osgGA
    )

INCLUDE_DIRECTORIES(${OPENSCENEGRAPH_INCLUDE_DIRS})

FIND_PACKAGE(Qt5 CONFIG REQUIRED
    COMPONENTS Gui Xml UiTools OpenGL)
FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(Bullet)

IF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_LINUX")
ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

#copy data faile
FILE(COPY data DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

add_subdirectory(common)
set(COMMON_LIBS common)
add_subdirectory(demo)
add_subdirectory(practice)
add_subdirectory(local)
add_subdirectory(challenge)

IF(${BULLET_FOUND})
    ADD_SUBDIRECTORY(bullet)
ENDIF()
