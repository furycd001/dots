qt2_wrap_ui(kcm_kicker_UI
    SOURCES
        positiontab.ui hidingtab.ui lookandfeeltab.ui menutab.ui extensionstab.ui
    )
qt2_wrap_cpp(kcm_kicker_MOC
    SOURCES
        applettab.h
        extensionstab_impl.h
        hidingtab_impl.h
        lookandfeeltab_impl.h
        main.h
        menutab_impl.h
        positiontab_impl.h
    )

kde2_module(libkcm_kicker
    SOURCES
        positiontab_impl.cpp hidingtab_impl.cpp lookandfeeltab_impl.cpp
        menutab_impl.cpp extensionstab_impl.cpp applettab.cpp main.cpp
        ${kcm_kicker_MOC}
        ${kcm_kicker_UI}
    LIBS
        kde2::ksycoca
        kde2::kfile
    )
install(FILES panel.desktop DESTINATION "${KDE2_APPSDIR}/Settings/LookNFeel")
