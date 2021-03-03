set(sidebar_SRCS
    konqsidebar.cpp
    sidebar_widget.cpp
    )

qt2_wrap_cpp(sidebar_SRCS
    SOURCES
        konqsidebar.h
        sidebar_widget.h
    )

qt2_wrap_cpp(sidebarplugin_MOC
    SOURCES
        konqsidebarplugin.h
    )

kde2_library(libkonqsidebarplugin
    SOURCES
        konqsidebarplugin.cpp
        ${sidebarplugin_MOC}
    LIBS
        kde2::konq
    )

kde2_module(libkonqsidebar
    SOURCES
        ${sidebar_SRCS}
    LIBS
        kde2::konqsidebarplugin
    )

install(FILES konq_sidebartng.desktop DESTINATION ${KDE2_SERVICESDIR})
install(FILES konqsidebartng.rc DESTINATION ${KDE2_CONFDIR})
install(FILES .version DESTINATION ${KDE2_DATADIR}/konqsidebartng/entries/)

add_subdirectory(sidebar_classic)
add_subdirectory(trees)
