qt2_wrap_cpp(kcm_smserver_MOC SOURCES kcmsmserver.h)
qt2_wrap_moc(kcm_smserver_MOC SOURCES smserverconfigimpl.h)
qt2_wrap_ui(kcm_smserver_UI SOURCES smserverconfigdlg.ui)

kde2_module(libkcm_smserver SOURCES kcmsmserver.cpp smserverconfigimpl.cpp ${kcm_smserver_UI} ${kcm_smserver_MOC} LIBS kde2::kdeui)
install(FILES kcmsmserver.desktop DESTINATION "${KDE2_APPSDIR}/Settings/System")
