qt2_wrap_cpp(systemtrayapplet_MOC SOURCES systemtrayapplet.h)
kde2_module(libksystemtrayapplet
    SOURCES
        systemtrayapplet.cpp
        ${systemtrayapplet_MOC}
    LIBS
        kinit_kicker
    )
install(FILES ksystemtrayapplet.desktop DESTINATION ${KDE2_DATADIR}/kicker/applets)

