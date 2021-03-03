qt2_wrap_cpp(kcm_passwords_MOC SOURCES passwords.h)

kde2_module(libkcm_passwords SOURCES passwords.cpp ${kcm_passwords_MOC} LIBS kde2::kdeui kde2::kdesu)
install(FILES passwords.desktop DESTINATION "${KDE2_APPSDIR}/Settings/Personalization")
