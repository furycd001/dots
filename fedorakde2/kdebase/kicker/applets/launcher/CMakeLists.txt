qt2_wrap_cpp(quicklauncher_MOC
    SOURCES
        quickaddappsmenu.h
        quickbutton.h
        quicklauncher.h
    )

kde2_module(libquicklauncher
    SOURCES
        quicklauncher.cpp quickbutton.cpp quickaddappsmenu.cpp
        ${quicklauncher_MOC}
    LIBS
        kinit_kicker
        INCLUDE_DIRECTORIES
)

install(FILES quicklauncher.desktop DESTINATION ${KDE2_DATADIR}/kicker/applets)
