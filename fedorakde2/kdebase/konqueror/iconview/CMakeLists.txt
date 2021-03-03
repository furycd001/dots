set(iconview_SRCS
    konq_iconview.cc
    )
qt2_wrap_cpp(iconview_SRCS
    SOURCES
        konq_iconview.h
    )

kde2_module(libkonqiconview
    SOURCES
        ${iconview_SRCS}
    LIBS
        kde2::konq
    )

install(FILES konq_iconview.rc konq_multicolumnview.rc DESTINATION ${KDE2_DATADIR}/konqiconview)
install(FILES konq_iconview.desktop konq_multicolumnview.desktop DESTINATION ${KDE2_SERVICESDIR})
