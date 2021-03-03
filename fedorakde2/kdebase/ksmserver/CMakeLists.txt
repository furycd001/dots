qt2_wrap_cpp(ksmserver_MOC
    SOURCES
        server.h
        shutdown.h
    )
kde2_kinit_executable(ksmserver2
    SOURCES
        server.cpp shutdown.cpp main.cpp
        ${ksmserver_MOC}
    LIBS
        kde2::kdecore
    )

add_executable(testsh test.cpp shutdown.moc shutdown.cpp)
target_link_libraries(testsh kde2::kdecore)

