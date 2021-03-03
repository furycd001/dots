qt2_wrap_cpp(dockbarextension_MOC
    SOURCES
        dockbarextension.h
        dockcontainer.h
    )

kde2_module(libdockbarextension
    SOURCES
        dockcontainer.cpp dockbarextension.cpp
        ${dockbarextension_MOC}
    LIBS
        kde2::kicker
)

install(FILES dockbarextension.desktop DESTINATION ${KDE2_DATADIR}/kicker/extensions)
