set(kfindpart_SRCS
    kftabdlg.cpp kfind.cpp kquery.cpp kfindpart.cpp
    )

qt2_wrap_cpp(kfindpart_SRCS
    SOURCES
        kftabdlg.h
        kfind.h
        kfindpart.h
        kquery.h
    )

kde2_module(kfindpart
    SOURCES
        ${kfindpart_SRCS}
    LIBS
        kde2::konq
    )


install(FILES kfindpart.desktop DESTINATION ${KDE2_SERVICESDIR})
install(FILES findpart.desktop DESTINATION ${KDE2_SERVICETYPESDIR})
