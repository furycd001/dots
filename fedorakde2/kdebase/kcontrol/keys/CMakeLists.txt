qt2_wrap_cpp(kcm_keys_MOC SOURCES keyconfig.h main.h savescm.h)

kde2_module(libkcm_keys SOURCES keyconfig.cpp savescm.cpp main.cpp ${kcm_keys_MOC} LIBS kde2::kdeui)
install(FILES keys.desktop DESTINATION "${KDE2_APPSDIR}/Settings/LookNFeel")

install(
    FILES
        app-kde3.kksrc app-kde4.kksrc app-mac3.kksrc app-unix3.kksrc
        app-win3.kksrc
    DESTINATION
        "${KDE2_DATADIR}/kcmkeys/standard"
    )

install(
    FILES
        global-kde3.kksrc global-kde4.kksrc global-mac3.kksrc
        global-unix3.kksrc global-win3.kksrc global-win4.kksrc
    DESTINATION
        "${KDE2_DATADIR}/kcmkeys/global"
    )
