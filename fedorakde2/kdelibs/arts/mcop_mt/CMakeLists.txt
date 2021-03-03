kde2_library(libmcop_mt
    SOURCES
        threads_posix.cc
    LIBS
        kde2::mcop
        Threads::Threads
    VERSION
        3.0.0
    )

