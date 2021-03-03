qt2_wrap_cpp(kcm_ioslaveinfo_MOC SOURCES kcmioslaveinfo.h)
kde2_module(libkcm_ioslaveinfo SOURCES kcmioslaveinfo.cpp ${kcm_ioslaveinfo_MOC} LIBS kde2::kfile)
install(FILES ioslaveinfo.desktop DESTINATION "${KDE2_APPSDIR}/Settings/Network")
install(FILES uninstall.desktop DESTINATION "${KDE2_APPSDIR}/Settings/Information/" RENAME ioslaveinfo.desktop)
