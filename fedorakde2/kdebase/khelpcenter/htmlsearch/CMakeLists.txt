set(libhtmlsearch_SRCS
    htmlsearch.cpp progressdialog.cpp
)
qt2_wrap_cpp(libhtmlsearch_SRCS
    SOURCES
        htmlsearch.h
        progressdialog.h
)

kde2_library(libhtmlsearch
    SOURCES
        ${libhtmlsearch_SRCS}
    LIBS
        kde2::kfile
    )

set(kcmhtmlsearch_SRCS
    kcmhtmlsearch.cpp klangcombo.cpp ktagcombobox.cpp
)
qt2_wrap_cpp(kcmhtmlsearch_SRCS
    SOURCES
        kcmhtmlsearch.h
        klangcombo.h
        ktagcombobox.h
)
kde2_module(libkcm_htmlsearch
    SOURCES
        ${kcmhtmlsearch_SRCS}
    LIBS
        kde2::htmlsearch
)

install(FILES htmlsearch.desktop DESTINATION ${KDE2_APPSDIR}/Settings/Help)
install(FILES
    unchecked.xpm checked.xpm
    star.png star_blank.png
    DESTINATION ${KDE2_DATADIR}/khelpcenter/pics
    )
install(FILES
    long.html short.html wrapper.html nomatch.html syntax.html
    DESTINATION ${KDE2_DATADIR}/khelpcenter/en
    )
install(FILES
    meinproc_wrapper
    DESTINATION ${KDE2_DATADIR}/khelpcenter
    )
