
set(kxsconfig_SRCS
    kxsconfig.cpp kxscontrol.cpp kxsitem.cpp
    )

qt2_wrap_cpp(kxsconfig_SRCS
    SOURCES
        kxsconfig.h
        kxscontrol.h
    )
add_executable(kxsconfig ${kxsconfig_SRCS})
target_link_libraries(kxsconfig kde2::kdeui)
