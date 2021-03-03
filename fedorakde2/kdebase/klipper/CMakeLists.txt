qt2_wrap_cpp(klipper_MOC
    SOURCES
        configdialog.h
        toplevel.h
        urlgrabber.h
    )

kde2_kinit_executable(klipper2
    SOURCES
        main.cpp toplevel.cpp urlgrabber.cpp configdialog.cpp
        ${klipper_MOC}
    LIBS
        kde2::ksycoca
    )

install(FILES klipper.desktop DESTINATION ${KDE2_APPSDIR}/Utilities)
install(FILES klipper.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/autostart)
install(FILES klipperrc DESTINATION ${KDE2_CONFDIR})

# TODO:
### update script:
##updatedir = $(kde_datadir)/kconf_update
##update_DATA = klipperrc.upd klipper-1-2.pl
