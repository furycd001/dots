qt2_wrap_cpp(kcm_themes_MOC
    SOURCES
        about.h
        groupdetails.h
        installer.h
        newthemedlg.h
        options.h
        theme.h
    )
kde2_module(libkcm_themes
    SOURCES
        main.cpp installer.cpp
        options.cpp about.cpp groupdetails.cpp
        themecreator.cpp newthemedlg.cpp theme.cpp
        ${kcm_themes_MOC}
    LIBS
        kde2::kfile
    )

install(FILES kthememgr.desktop DESTINATION "${KDE2_APPSDIR}/Settings/LookNFeel")
install(FILES theme.mappings DESTINATION "${KDE2_DATADIR}/kthememgr")
install(FILES Template.themerc DESTINATION "${KDE2_DATADIR}/kthememgr/Themes")
install(FILES x-ktheme.desktop DESTINATION ${KDE2_MIMEDIR}/application)

add_subdirectory(Themes)
