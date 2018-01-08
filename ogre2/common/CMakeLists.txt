add_direct(./ DEMOAPP_SRC)
add_library(common ${DEMOAPP_SRC})
target_link_libraries(common ${OGRE_SAMPLES_LIBRARIES} ${Boost_LIBRARIES} ${OGRE_LIBRARIES}   ${SDL2_LIBRARY})
