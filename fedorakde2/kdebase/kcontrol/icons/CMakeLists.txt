qt2_wrap_cpp(kcm_icons_MOC
    SOURCES
    icons.h
    iconthemes.h
    main.h
)

kde2_module(libkcm_icons SOURCES iconthemes.cpp icons.cpp main.cpp ${kcm_icons_MOC} LIBS kde2::kdeui kde2::kfile)
install(FILES uninstall.desktop DESTINATION "${KDE2_APPSDIR}/Settings/LookNFeel/Themes" RENAME iconthemes.desktop)
install(FILES icons.desktop DESTINATION "${KDE2_APPSDIR}/Settings/LookNFeel")

# TODO: test exec
