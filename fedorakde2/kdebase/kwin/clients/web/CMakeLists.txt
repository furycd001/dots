qt2_wrap_cpp(web_MOC
    SOURCES 
        WebButtonClose.h
        WebButton.h
        WebButtonHelp.h
        WebButtonIconify.h
        WebButtonLower.h
        WebButtonMaximize.h
        WebButtonSticky.h
        Web.h
    )

kde2_module(libkwinweb
    SOURCES 
        Web.cpp
        WebButton.cpp
        WebButtonClose.cpp
        WebButtonHelp.cpp
        WebButtonIconify.cpp
        WebButtonLower.cpp
        WebButtonMaximize.cpp
        WebButtonSticky.cpp
        ${web_MOC}
    LIBS
        kde2::kwin
    )
install(FILES web.desktop DESTINATION "${KDE2_DATADIR}/kwin")


