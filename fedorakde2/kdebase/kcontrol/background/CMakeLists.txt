qt2_wrap_cpp(bgnd_MOC SOURCES bgrender.h)
kde2_library(bgnd
    SOURCES
        bgsettings.cc bgrender.cc
        ${bgnd_MOC}
    LIBS
        kde2::ksycoca
        kde2::kfile
    )
qt2_wrap_cpp(kcm_background_MOC SOURCES backgnd.h bgdialogs.h)
kde2_module(libkcm_background SOURCES backgnd.cpp bgdialogs.cpp  ${kcm_background_MOC} LIBS kde2::bgnd)
install(FILES background.desktop DESTINATION "${KDE2_APPSDIR}/Settings/LookNFeel")

add_subdirectory(pics)
