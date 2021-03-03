qt2_wrap_ui(kcm_css_UI
    SOURCES
        cssconfig.ui
        preview.ui
    )
qt2_wrap_cpp(kcm_css_MOC
    SOURCES
        kcmcss.h
    )

kde2_module(libkcm_css
    SOURCES
        template.cpp kcmcss.cpp
        ${kcm_css_UI}
        ${kcm_css_MOC}
    LIBS
        kde2::kdeui
        kde2::kfile
    )
install(FILES kcmcss.desktop DESTINATION "${KDE2_APPSDIR}/Settings/WebBrowsing")
