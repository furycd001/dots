qt2_wrap_cpp(kcm_screensaver_MOC SOURCES scrnsave.h)
kde2_module(libkcm_screensaver
    SOURCES
        scrnsave.cpp
        ${kcm_screensaver_MOC}
    LIBS
        kde2::ksycoca
    )

install(FILES screensaver.desktop DESTINATION "${KDE2_APPSDIR}/Settings/LookNFeel")

add_subdirectory(pics)
