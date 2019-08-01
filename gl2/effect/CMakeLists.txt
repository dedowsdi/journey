set (PROJECTS
    circular
    hdr
    bloom
    gaussian_blur
    directional_shadowmap
    omni_shadowmap
    blinnphong
    rim
    instance
    bitmap_text
    freetype_text
    )

foreach(PROJECT ${PROJECTS})
    message("add effect ${PROJECT}")
    add_executable(${PROJECT} ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT}.cpp)
    set_target_properties(${PROJECT} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
    target_link_libraries(${PROJECT}
        PRIVATE
            gl21app
        )
endforeach()
