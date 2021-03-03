qt2_wrap_cpp(kminipagerapplet_MOC SOURCES pagerapplet.h)
kde2_module(libkminipagerapplet
    SOURCES
        pagerapplet.cpp
        ${kminipagerapplet_MOC}
    LIBS
        kinit_kicker
    )
install(FILES kminipagerapplet.desktop DESTINATION ${KDE2_DATADIR}/kicker/applets)

