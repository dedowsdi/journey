set(SRC
    src/Game.cpp
    src/Config.cpp
    src/DebugHandler.cpp
    src/LODVisitor.cpp
    src/OsgFactory.cpp
    src/OsgQuery.cpp
    src/Resource.cpp
    src/ToyMath.cpp
    src/ToyViewer.cpp
    )

add_library(common ${SRC})

target_link_libraries(common
    PRIVATE
    ${OPENSCENEGRAPH_LIBRARIES}
    OpenGL::GL
    )

target_include_directories(common
    PUBLIC
    include
    ${OPENSCENEGRAPH_INCLUDE_DIRS}
    )
