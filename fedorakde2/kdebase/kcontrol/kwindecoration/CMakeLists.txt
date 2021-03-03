qt2_wrap_cpp(kcm_kwindecoration_MOC SOURCES buttons.h kwindecoration.h)
kde2_kidl(kcm_kwindecoration_IDL
    SOURCES
        kwindecorationIface.h
    )

kde2_module(libkcm_kwindecoration
    SOURCES
        kwindecoration.cpp buttons.cpp
        kwindecorationIface_skel.cpp

        ${kcm_kwindecoration_MOC}
        ${kcm_kwindecoration_IDL}
    LIBS
        kde2::kdeui
    )
install(FILES kwindecoration.desktop DESTINATION "${KDE2_APPSDIR}/Settings/LookNFeel")
