qt2_wrap_cpp(swallowapplet_MOC
    SOURCES
        swallow.h
    )

kde2_module(libswallowapplet
    SOURCES
        swallow.cpp
        ${swallowapplet_MOC}
    LIBS
        kinit_kicker
)

install(FILES swallowapplet.desktop DESTINATION ${KDE2_DATADIR}/kicker/applets)
