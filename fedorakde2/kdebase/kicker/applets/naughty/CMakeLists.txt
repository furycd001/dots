qt2_wrap_cpp(naughtyapplet_MOC
    SOURCES
        NaughtyApplet.h
        NaughtyProcessMonitor.h
    )
qt2_wrap_moc(naughtyapplet_MOC
    SOURCES
        NaughtyConfigDialog.h
    )

kde2_module(libnaughtyapplet
    SOURCES
        NaughtyProcessMonitor.cpp
        NaughtyConfigDialog.cpp
        NaughtyApplet.cpp
        ${naughtyapplet_MOC}
    LIBS
        kinit_kicker
)

install(FILES naughtyapplet.desktop DESTINATION ${KDE2_DATADIR}/kicker/applets)
install(FILES naughty-happy.png naughty-sad.png DESTINATION ${KDE2_DATADIR}/naughtyapplet/pics)
