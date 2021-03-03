qt2_wrap_cpp(ktip_MOC SOURCES ktipwindow.h)

kde2_kinit_executable(ktip
    SOURCES
        ktipwindow.cpp
        ${ktip_MOC}
    LIBS
        kde2::kdeui
    )

install(FILES ktip.desktop DESTINATION ${KDE2_APPSDIR}/Toys/)
install(FILES ktip.desktop DESTINATION ${KDEDIR}/share/autostart)
install(FILES tips DESTINATION ${KDE2_DATADIR}/kdewizard)

add_subdirectory(pics)

