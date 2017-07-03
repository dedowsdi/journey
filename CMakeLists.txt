cmake_minimum_required (VERSION 2.6)

project (osg)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;/usr/local/source/OpenSceneGraph/CMakeModules;${CMAKE_MODULE_PATH}")
set(CMAKE_PREFIX_PATH /usr/local/lib64)
include(util)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG -D_DEBUG"  )

if( CMAKE_BUILD_TYPE STREQUAL ""  )
    set( CMAKE_BUILD_TYPE Debug CACHE STRING "One of: Debug Release
    RelWithDebInfo MinSizeRel." FORCE  )
endif()

#flag
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -Wall -Wno-unused-local-typedef -Wno-overloaded-virtual -Wno-inconsistent-missing-override -Wno-switch-bool -Wno-int-to-void-pointer-cast -Wno-unused-function -Wno-reorder")
#SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -v")


set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
#this variable will be used at findOSG
set(CMAKE_DEBUG_POSTFIX d CACHE STRING "i need to get this at build file, so it's cached")

#becareful cmake prebuilt find## use smallcase
FIND_PACKAGE(PkgConfig REQUIRED)
FIND_PACKAGE(OSG REQUIRED)
#FIND_PACKAGE(OSGDB REQUIRED)
#FIND_PACKAGE(OSGUTIL REQUIRED)
#FIND_PACKAGE(OSGVIEWER REQUIRED)
#FIND_PACKAGE(OPENTHREADS REQUIRED)

#PKG_CHECK_MODULES(OPENTHREADS REQUIRED openthreads)
#PKG_CHECK_MODULES(osg REQUIRED)
#PKG_CHECK_MODULES(osgDB REQUIRED)
#PKG_CHECK_MODULES(osgUtil REQUIRED)
#PKG_CHECK_MODULES(osgViewer REQUIRED)
#message(0 ${OPENTHREADS_INCLUDE_DIR})
#message(0 ${OSG_INCLUDE_DIR})
#message(0 ${OSGDB_INCLUDE_DIR})
#message(0 ${OSGUTIL_INCLUDE_DIR})
#message(0 ${OSGVIEWER_INCLUDE_DIR})

FIND_PACKAGE(OpenGL REQUIRED)

include_directories(
    #${OPENTHREADS_INCLUDE_DIR}
    ${OSG_INCLUDE_DIR}
    #${OSGDB_INCLUDE_DIR}
    #${OSGUTIL_INCLUDE_DIR}
    #${OSGVIEWER_INCLUDE_DIR}
    common/include
    )

IF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_LINUX")
ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

#generate cfg file

add_subdirectory(common)
set(COMMON_LIBS common)
add_subdirectory(demo)
add_subdirectory(practice)
add_subdirectory(local)
