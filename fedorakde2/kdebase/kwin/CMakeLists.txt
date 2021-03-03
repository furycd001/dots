set(kwin_SRCS
    workspace.cpp atoms.cpp  client.cpp main.cpp
    tabbox.cpp options.cpp plugins.cpp events.cpp
    killwindow.cpp kwinbutton.cpp
    KWinInterface_stub.cpp
    )

kde2_wrap_kidl(kwin_SRCS
    SOURCES
        KWinInterface.h
    )

qt2_wrap_cpp(kwin_SRCS
    SOURCES
        client.h
        kwinbutton.h
        options.h
        plugins.h
        tabbox.h
        workspace.h
    )

kde2_library(libkwin
    SOURCES
        ${kwin_SRCS}
    HEADERS
        ${CMAKE_CURRENT_BINARY_DIR}/KWinInterface_stub.h
    LIBS
        kde2::kdecore
        kde2::kdeui
        X11::Xinerama
        ltdl
    )

add_executable(kwin kwin_main.cpp)
target_link_libraries(kwin kde2::kwin kde2::kdeui)

install(TARGETS kwin RUNTIME DESTINATION ${KDE2_BINDIR})

install(FILES eventsrc DESTINATION "${KDE2_DATADIR}/kwin")
install(FILES pop.wav DESTINATION "${KDE2_SOUNDDIR}")

add_subdirectory(clients)
add_subdirectory(pics)

