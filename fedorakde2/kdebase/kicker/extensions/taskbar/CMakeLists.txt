qt2_wrap_cpp(taskbarextension_MOC SOURCES taskbarextension.h)

kde2_module(libtaskbarextension
    SOURCES
        taskbarextension.cpp
        ${taskbarextension_MOC}
    LIBS
        kde2::taskbar
)

install(FILES taskbarextension.desktop DESTINATION ${KDE2_DATADIR}/kicker/extensions)
