set(kdebugdialog_SRCS
    main.cpp kabstractdebugdialog.cpp kdebugdialog.cpp klistdebugdialog.cpp
)

qt2_wrap_cpp(kdebugdialog_SRCS
    SOURCES
        kabstractdebugdialog.h
        kdebugdialog.h
        klistdebugdialog.h
    )

add_executable(kdebugdialog2 ${kdebugdialog_SRCS})
target_link_libraries(kdebugdialog2
    kde2::kdeui
    )
install(TARGETS kdebugdialog2 RUNTIME DESTINATION ${KDE2_BINDIR})
