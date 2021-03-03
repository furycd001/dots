macro(DIRLIST dirs)
    file(GLOB files LIST_DIRECTORIES true *)
    foreach(thefile ${files})
        if(IS_DIRECTORY ${thefile})
            get_filename_component(thedir ${thefile} NAME)
            list(APPEND dirs ${thedir})
        endif()
    endforeach()
endmacro()

install(
    FILES
        africa.desktop asia.desktop caribbean.desktop easteurope.desktop
        northamerica.desktop southamerica.desktop westeurope.desktop
        centraleurope.desktop northeurope.desktop southeurope.desktop
    DESTINATION
        ${KDE2_LOCALE}/l10n
    )

dirlist(dirs)
foreach(dir ${dirs})
    install(DIRECTORY ${dir} DESTINATION ${KDE2_LOCALE}/l10n)
endforeach()
