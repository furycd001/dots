qt2_wrap_cpp(kcm_usb_MOC SOURCES kcmusb.h)
kde2_module(libkcm_usb SOURCES kcmusb.cpp usbdevices.cpp usbdb.cpp ${kcm_usb_MOC} LIBS kde2::kdeui)
install(FILES kcmusb.desktop DESTINATION "${KDE2_APPSDIR}/Settings/Information")
install(FILES usb.ids DESTINATION "${KDE2_DATADIR}/kcmusb")
