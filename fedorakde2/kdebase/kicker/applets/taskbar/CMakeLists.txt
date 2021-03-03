qt2_wrap_cpp(taskbarapplet_MOC SOURCES taskbarapplet.h)
kde2_module(libtaskbarapplet
    SOURCES
        taskbarapplet.cpp
        ${taskbarapplet_MOC}
    LIBS
        kinit_kicker
        kde2::taskbar
    )
install(FILES ktaskbarapplet.desktop DESTINATION ${KDE2_DATADIR}/kicker/applets)

