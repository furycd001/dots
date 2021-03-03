set(sidebar_classic_SRCS
    konqsidebar_classic_wrap.cpp
    )

qt2_wrap_cpp(sidebar_classic_SRCS
    SOURCES
        konqsidebar_classic_wrap.h
    )

kde2_module(libkonqsidebar_classic_wrap
    SOURCES
        ${sidebar_classic_SRCS}
    LIBS
        kde2::konqsidebarplugin
    )

install(FILES sidebar_classic.desktop DESTINATION ${KDE2_DATADIR}/konqsidebartng/entries)
install(FILES sidebar_classic_add.desktop DESTINATION ${KDE2_DATADIR}/konqsidebartng/add)


