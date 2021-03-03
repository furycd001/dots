qt2_wrap_cpp(kcm_bell_MOC SOURCES bell.h)
kde2_module(libkcm_bell SOURCES bell.cpp ${kcm_bell_MOC} LIBS kde2::kdeui)
install(FILES bell.desktop DESTINATION "${KDE2_APPSDIR}/Settings/Sound")
