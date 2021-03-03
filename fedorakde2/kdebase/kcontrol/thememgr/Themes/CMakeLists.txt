function(pack_ktheme THEME_NAME)
    add_custom_command(OUTPUT  "${CMAKE_CURRENT_BINARY_DIR}/${THEME_NAME}.ktheme"
        COMMAND ${CMAKE_COMMAND} -E tar "cfvz" "${CMAKE_CURRENT_BINARY_DIR}/${THEME_NAME}.ktheme" "${THEME_NAME}"
            VERBATIM
            DEPENDS ${THEME_NAME}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            )

        install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${THEME_NAME}.ktheme" DESTINATION "${KDE2_DATADIR}/kthememgr/Themes")

        add_custom_target(theme_${THEME_NAME} ALL DEPENDS ${THEME_NAME}.ktheme)
        add_dependencies(module_libkcm_themes theme_${THEME_NAME})
endfunction()

pack_ktheme(Default)
pack_ktheme(MGBreizh)
pack_ktheme(Nostalgy)
pack_ktheme(Wood)
