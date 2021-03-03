qt2_wrap_cpp(runapplet_MOC SOURCES runapplet.h)
kde2_module(libkrunapplet
    SOURCES
        runapplet.cpp
        ${runapplet_MOC}
    LIBS
        kinit_kicker
    )
install(FILES krunapplet.desktop DESTINATION ${KDE2_DATADIR}/kicker/applets)

