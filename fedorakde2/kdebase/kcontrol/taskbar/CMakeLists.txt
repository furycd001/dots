qt2_wrap_cpp(kcm_taskbar_MOC SOURCES kcmtaskbar.h)
qt2_wrap_ui(kcm_taskbar_UI SOURCES kcmtaskbarui.ui)

kde2_module(libkcm_taskbar SOURCES kcmtaskbar.cpp ${kcm_taskbar_UI} ${kcm_taskbar_MOC} LIBS kde2::kdeui)
install(FILES kcmtaskbar.desktop DESTINATION "${KDE2_APPSDIR}/Settings/LookNFeel")
