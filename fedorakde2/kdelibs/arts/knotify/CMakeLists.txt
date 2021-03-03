set(knotify_SRCS knotify.cpp)

qt2_wrap_cpp(knotify_SRCS
    SOURCES
        knotify.h
    )
kde2_wrap_kidl(knotify_SRCS SOURCES knotify.h)

kde2_kinit_executable(knotify
    SOURCES
        ${knotify_SRCS}
    LIBS
        kde2::kdeui
        kde2::soundserver
        kde2::qtmcop
    )
