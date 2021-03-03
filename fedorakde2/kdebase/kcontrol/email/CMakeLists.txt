qt2_wrap_cpp(kcm_email_MOC SOURCES email.h)
qt2_wrap_ui(kcm_email_UI SOURCES emailbase.ui)
kde2_module(libkcm_email SOURCES email.cpp ${kcm_email_UI} ${kcm_email_MOC} LIBS kde2::kfile)
install(FILES email.desktop DESTINATION "${KDE2_APPSDIR}/Settings/Network")
