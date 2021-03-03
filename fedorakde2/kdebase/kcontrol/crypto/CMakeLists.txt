qt2_wrap_cpp(kcm_crypto_MOC
    SOURCES
        certexport.h
        crypto.h
        kdatetimedlg.h
    )

kde2_module(libkcm_crypto
    SOURCES
        crypto.cpp certexport.cpp kdatetimedlg.cpp
        ${kcm_crypto_MOC}
    LIBS
        kde2::kdeui
        kde2::kfile
        OpenSSL::SSL
        OpenSSL::Crypto
    )
install(FILES crypto.desktop DESTINATION "${KDE2_APPSDIR}/Settings/Personalization")
