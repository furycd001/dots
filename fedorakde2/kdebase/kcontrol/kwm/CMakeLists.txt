qt2_wrap_cpp(kcm_kwinoptions_MOC SOURCES advanced.h main.h mouse.h titlebar.h windows.h)

kde2_module(libkcm_kwinoptions SOURCES windows.cpp mouse.cpp main.cpp ${kcm_kwinoptions_MOC} LIBS kde2::kdeui)
install(FILES kwinoptions.desktop DESTINATION "${KDE2_APPSDIR}/Settings/LookNFeel")

install(FILES uninstall.desktop DESTINATION "${KDE2_APPSDIR}/Settings/LookNFeel/Windows" RENAME actions.desktop)
install(FILES uninstall.desktop DESTINATION "${KDE2_APPSDIR}/Settings/LookNFeel/Windows" RENAME kwinmouse.desktop)
install(FILES uninstall.desktop DESTINATION "${KDE2_APPSDIR}/Settings/LookNFeel/Windows" RENAME mouse.desktop)
