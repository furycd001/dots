add_subdirectory(uasproviders)

qt2_wrap_ui(kcm_kio_UI
    SOURCES
        kproxydlgui.ui
    )
qt2_wrap_cpp(kcm_kio_MOC
    SOURCES
        kcookiesmain.h
        kcookiesmanagement.h
        kcookiespolicies.h
        kproxydlg.h
        ksmboptdlg.h
        netpref.h
        policydlg.h
        smbrodlg.h
        uagentproviderdlg.h
        useragentdlg.h
    )

kde2_module(libkcm_kio
    SOURCES
        fakeuaprovider.cpp
        kcookiesmain.cpp
        kcookiesmanagement.cpp
        kcookiespolicies.cpp
        kproxydlg.cpp
        main.cpp
        netpref.cpp
        policydlg.cpp
        smbrodlg.cpp
        uagentproviderdlg.cpp
        useragentdlg.cpp
        ${kcm_kio_UI}
        ${kcm_kio_MOC}
    LIBS
        kde2::ksycoca
        kde2::kfile
        kde2::kio
    )
install(FILES
    cookies.desktop proxy.desktop useragent.desktop
    DESTINATION "${KDE2_APPSDIR}/Settings/WebBrowsing")
install(FILES uninstall.desktop DESTINATION "${KDE2_APPSDIR}/Settings/WebBrowsing" RENAME smb.desktop)
install(FILES uasprovider.desktop DESTINATION "${KDE2_SERVICETYPESDIR}")

# TODO: tests
# TODO (or skipped on purpose) smb
