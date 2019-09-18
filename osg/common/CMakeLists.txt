set(COMMON_SRCS
    src/auxiliary.cpp
    src/common.cpp
    src/glm.cpp
    src/gridfloor.cpp
    src/menu.cpp
    src/menuelement.cpp
    src/menuitem.cpp
    src/nurb.cpp
    src/submenu.cpp
    src/zmath.cpp
    src/matstack.cpp
    src/geometryutil.cpp
    src/streamutil.cpp
)

add_library(common SHARED ${COMMON_SRCS})
target_include_directories(common
    PUBLIC
    include
    )

#target_link_libraries(common ${EXTRA_LIBS})
