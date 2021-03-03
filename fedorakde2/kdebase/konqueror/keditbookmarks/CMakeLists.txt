kde2_stdicon()

set(keditbookmarks_SRCS
    main.cpp toplevel.cpp commands.cpp
    )
qt2_wrap_cpp(keditbookmarks_SRCS
    SOURCES
        toplevel.h
        commands.h
    )

kde2_kinit_executable(keditbookmarks
    SOURCES
        ${keditbookmarks_SRCS}
    LIBS
        kde2::konq
    )

install(FILES keditbookmarksui.rc DESTINATION ${KDE2_DATADIR}/keditbookmarks)
install(FILES keditbookmarks.desktop DESTINATION ${KDE2_APPSDIR}/Internet)
