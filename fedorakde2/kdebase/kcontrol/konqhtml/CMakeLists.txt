qt2_wrap_cpp(kcm_konqhtml_MOC
    SOURCES
        appearance.h
        htmlopts.h
        javaopts.h
        jsopts.h
        khttpoptdlg.h
        main.h
        pluginopts.h
        policydlg.h
    )

kde2_module(libkcm_konqhtml
    SOURCES
        appearance.cpp
        htmlopts.cpp
        javaopts.cpp
        jsopts.cpp
        khttpoptdlg.cpp
        main.cpp
        pluginopts.cpp
        policydlg.cpp
        ${kcm_konqhtml_MOC}
    LIBS
        kde2::khtml
        kde2::konq
    )
install(FILES konqhtml.desktop DESTINATION "${KDE2_APPSDIR}/Settings/WebBrowsing")
