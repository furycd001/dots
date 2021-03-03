qt2_wrap_ui(kcm_konsole_UI
    SOURCES
	schemadialog.ui kcmkonsoledialog.ui
    )

qt2_wrap_cpp(kcm_konsole_MOC SOURCES kcmkonsole.h)
qt2_wrap_moc(kcm_konsole_MOC SOURCES schemaeditor.h)

kde2_module(libkcm_konsole
    SOURCES
        schemaeditor.cpp kcmkonsole.cpp
        ${kcm_konsole_UI}
        ${kcm_konsole_MOC}
    LIBS
        kde2::kfile
    )
install(FILES kcmkonsole.desktop DESTINATION "${KDE2_APPSDIR}/Settings/System")

