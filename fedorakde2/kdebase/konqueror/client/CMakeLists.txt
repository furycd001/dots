
set(kfmclient_SRCS
    kfmclient.cc 
    )

qt2_wrap_cpp(kfmclient_SRCS
    SOURCES
        kfmclient.h
    )
add_executable(kfmclient ${kfmclient_SRCS})
target_link_libraries(kfmclient
    kde2::kdeui
    kde2::kio
    kde2::kfile
    kde2::konqueror
    kde2::kdesktop
    )
install(TARGETS kfmclient RUNTIME DESTINATION ${KDE2_BINDIR})

