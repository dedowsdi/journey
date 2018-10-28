set(SRC
    MovieDialog.cpp
    main.cpp
    )

#find_path(RAPIDJSON_HOME document.h PATH_SUFFIXES rapidjson)
#include_directories(${RAPIDJSON_HOME})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/ratings.json DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
add_executable(movie_recommender ${SRC})
target_link_libraries(movie_recommender Qt5::Widgets)
set_target_properties(movie_recommender PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
