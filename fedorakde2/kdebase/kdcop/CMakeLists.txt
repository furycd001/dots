qt2_wrap_cpp(kdcop_MOC
    SOURCES
        kdcopwindow.h
    )
add_executable(kdcop
    kdcop.cpp
    kdcopwindow.cpp
    ${kdcop_MOC}
    )
target_link_libraries(kdcop kde2::kdeui)
install(TARGETS kdcop)


install(FILES kdcopui.rc DESTINATION ${KDE2_DATADIR}/kdcop)
