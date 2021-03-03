set(kio_man_SRC
    kio_man.cpp
    man2html.cpp
    )
qt2_wrap_cpp(kio_man_SRC
    SOURCES
        kio_man.h
    )
kde2_module(kio_man SOURCES ${kio_man_SRC} LIBS kde2::kio kde2::ksycoca)
install(FILES man.protocol DESTINATION "${KDE2_SERVICESDIR}")

