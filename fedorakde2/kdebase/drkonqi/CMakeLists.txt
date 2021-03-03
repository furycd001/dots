add_subdirectory(debuggers)
add_subdirectory(pics)
#add_subdirectory(presets)

set(drkonqi_SRCS
        main.cpp
        debugger.cpp
        krashconf.cpp
        drbugreport.cpp
        backtrace.cpp
        toplevel.cpp
    )

qt2_wrap_cpp(drkonqi_SRCS
    SOURCES
        backtrace.h
        debugger.h
        drbugreport.h
        toplevel.h
        )

kde2_kinit_executable(drkonqi2
    SOURCES
        ${drkonqi_SRCS}
    LIBS
        kde2::kdeui
        kde2::kfile
)

