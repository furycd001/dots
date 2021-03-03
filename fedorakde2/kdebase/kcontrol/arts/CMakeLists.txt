qt2_wrap_cpp(kcm_arts_MOC SOURCES arts.h)
qt2_wrap_ui(kcm_arts_UI SOURCES artsconfig.ui)
kde2_module(libkcm_arts SOURCES arts.cpp ${kcm_arts_UI} ${kcm_arts_MOC} LIBS kde2::kdeui)
install(FILES arts.desktop DESTINATION "${KDE2_APPSDIR}/Settings/Sound")
