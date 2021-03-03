qt2_wrap_cpp(kcm_launch_MOC SOURCES kcmlaunch.h)
kde2_module(libkcm_launch SOURCES kcmlaunch.cpp ${kcm_launch_MOC} LIBS kde2::kdeui)
install(FILES kcmlaunch.desktop DESTINATION "${KDE2_APPSDIR}/Settings/LookNFeel/")
