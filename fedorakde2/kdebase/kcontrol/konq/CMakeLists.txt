set(kcm_konq_SRCS
    behaviour.cpp
    rootopts.cpp
    trashopts.cpp
    fontopts.cpp
    desktop.cpp
    main.cpp
)

qt2_wrap_cpp(kcm_konq_SRCS
    SOURCES
        behaviour.h
        desktop.h
        fontopts.h
        main.h
        main.h
        rootopts.h
        trashopts.h
)

kde2_module(libkcm_konq SOURCES ${kcm_konq_SRCS} LIBS kde2::konqueror)

install(FILES overlapping.png onlyone.png DESTINATION "${KDE2_DATADIR}/kcontrol/pics")

install(FILES desktop.desktop virtualdesktops.desktop DESTINATION "${KDE2_APPSDIR}/Settings/LookNFeel")
install(FILES kcmkonq.desktop DESTINATION "${KDE2_APPSDIR}/Settings/FileBrowsing")

install(FILES uninstall.desktop DESTINATION "${KDE2_APPSDIR}/Settings/LookNFeel/Desktop" RENAME virtualdesktops.desktop)
install(FILES uninstall.desktop DESTINATION "${KDE2_APPSDIR}/Settings/LookNFeel/Desktop" RENAME desktop.desktop)
install(FILES uninstall.desktop DESTINATION "${KDE2_APPSDIR}/Settings/LookNFeel/Desktop" RENAME borders.desktop)
install(FILES uninstall.desktop DESTINATION "${KDE2_APPSDIR}/Settings/LookNFeel/Desktop" RENAME background.desktop)
