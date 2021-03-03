set(kfmexec_SRCS
    main.cpp
    )
qt2_wrap_cpp(kfmexec_SRCS
    SOURCES
    main.h
    )
add_executable(kfmexec2 ${kfmexec_SRCS})
target_link_libraries(kfmexec2
    kde2::kdeui
    kde2::kio
    )
install(TARGETS kfmexec2 RUNTIME DESTINATION ${KDE2_BINDIR})
