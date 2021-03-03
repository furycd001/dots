qt2_wrap_cpp(libkcm_keyboard_MOC
    SOURCES
        kcmlayout.h
        kcmmisc.h
    )

kde2_module(libkcm_keyboard
    SOURCES
        rules.cpp
        kcmlayout.cpp
        pixmap.cpp
        kcmmisc.cpp
        ${libkcm_keyboard_MOC}
    LIBS
        kde2::kfile
        X11::Xtst
    )

qt2_wrap_cpp(kxkb_MOC SOURCES kxkb.h)
kde2_kinit_executable(kxkb
    SOURCES
        rules.cpp
        kxkb.cpp
        extension.cpp
        pixmap.cpp
        ${kxkb_MOC}
    LIBS
        kde2::kfile
    )


install(FILES keyboard.desktop DESTINATION "${KDE2_APPSDIR}/Settings/Peripherals")
install(FILES uninstall.desktop DESTINATION "${KDE2_APPSDIR}/Settings/Personalization" RENAME kcmlayout.desktop)
install(FILES kxkb.desktop DESTINATION ${KDE2_SERVICESDIR})
