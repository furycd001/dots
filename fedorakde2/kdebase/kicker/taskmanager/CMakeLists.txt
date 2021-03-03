set(taskmanager_SRCS taskmanager.cpp)
qt2_wrap_cpp(taskmanager_SRCS
    SOURCES
        taskmanager.h
    )
kde2_library(libtaskmanager
    SOURCES
        ${taskmanager_SRCS}
    HEADERS
        taskmanager.h
    LIBS
        kde2::kicker
   )
