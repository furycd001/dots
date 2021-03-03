qt2_wrap_cpp(kcm_input_MOC SOURCES mouse.h)
qt2_wrap_ui(kcm_input_UI SOURCES kmousedlg.ui)

kde2_module(libkcm_input SOURCES main.cpp mouse.cpp  ${kcm_input_UI} ${kcm_input_MOC} LIBS kde2::kio)
install(FILES mouse.desktop DESTINATION "${KDE2_APPSDIR}/Settings/Peripherals")
install(FILES cursor_large.pcf.gz DESTINATION "${KDE2_DATADIR}/kcminput")

add_subdirectory(pics)
