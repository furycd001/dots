macro(KDE2_ICON app)
    foreach(real_icon ${ARGN})
        string(REPLACE "hi16" "icons/hicolor/16x16" icon ${real_icon})
        string(REPLACE "hi24" "icons/hicolor/24x24" icon ${icon})
        string(REPLACE "hi32" "icons/hicolor/32x32" icon ${icon})
        string(REPLACE "hi48" "icons/hicolor/48x48" icon ${icon})
        string(REPLACE "hi64" "icons/hicolor/64x64" icon ${icon})
        string(REPLACE "app" "apps" icon ${icon})
        string(REGEX REPLACE "-([^.])" "/\\1" icon ${real_icon})
        get_filename_component(icon_path ${icon} DIRECTORY)
        get_filename_component(icon_name ${icon} NAME)
        if(${app} EQUAL "icondir")
            install(FILES ${real_icon}
                DESTINATION ${CMAKE_INSTALL_DATADIR}/${icon_path}
                RENAME ${icon_name}
                )
        else()
            install(FILES ${real_icon}
                DESTINATION ${KDE2_DATADIR}/${app}/${icon_path}
                RENAME ${icon_name}
                )
        endif()
    endforeach()
endmacro()

macro(KDE2_STDICON)
    set(oneValueArgs OUTPUT_DIR)
    cmake_parse_arguments(_ico "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    file(GLOB _ico_FILES LIST_DIRECTORIES false hi*.png)
    foreach(icon_file ${_ico_FILES})
        get_filename_component(real_icon ${icon_file} NAME)
        string(REGEX REPLACE "-([^.])" ";\\1" icon_list ${real_icon})
        list(LENGTH icon_list lsize)
        math(EXPR icon_list_size "${lsize}-1")
        list(GET icon_list ${icon_list_size} icon_name)
        list(REMOVE_AT icon_list ${icon_list_size})
        list(GET icon_list 0 icon_size)
        list(REMOVE_AT icon_list 0)
        string(REPLACE "hi" "" size ${icon_size})
        string(REPLACE ";" "/" pre_path "${icon_list}")
        list(GET icon_list 0 action)
        if(${action} STREQUAL "filesys")
            string(REPLACE "filesys" "filesystems" icon_path ${pre_path})
        elseif(${action} STREQUAL "action")
            string(REPLACE "action" "actions" icon_path ${pre_path})
        elseif(${action} STREQUAL "mime")
            string(REPLACE "mime" "mimetypes" icon_path ${pre_path})
        elseif(${action} STREQUAL "device")
            string(REPLACE "device" "devices" icon_path ${pre_path})
        elseif(${action} STREQUAL "app")
            string(REPLACE "app" "apps" icon_path ${pre_path})
        endif()
        set(outdir "${KDE2_ICONDIR}")
        if(_ico_OUTPUT_DIR)
            set(outdir "${_ico_OUTPUT_DIR}")
        endif()
        install(
            FILES ${real_icon}
            DESTINATION "${outdir}/hicolor/${size}x${size}/${icon_path}"
            RENAME ${icon_name}
            )
    endforeach()
endmacro()
