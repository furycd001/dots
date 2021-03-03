qt2_wrap_cpp(clockapplet_MOC SOURCES clock.h)
qt2_wrap_ui(clockapplet_UI SOURCES conf.ui)

kde2_module(libclockapplet
    SOURCES
        clock.cpp
        ${clockapplet_MOC}
        ${clockapplet_UI}
    LIBS
        kde2::kicker
)

install(FILES clockapplet.desktop DESTINATION ${KDE2_DATADIR}/kicker/applets)
install(FILES lcd.png DESTINATION ${KDE2_DATADIR}/clockapplet/pics)
