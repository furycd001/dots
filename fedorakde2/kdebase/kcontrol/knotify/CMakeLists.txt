qt2_wrap_cpp(kcm_knotify_MOC SOURCES knotify.h)

kde2_module(libkcm_knotify SOURCES main.cpp knotify.cpp events.cpp ${kcm_knotify_MOC} LIBS kde2::kfile)
install(FILES kcmnotify.desktop DESTINATION "${KDE2_APPSDIR}/Settings/Sound")
install(FILES play.png DESTINATION "${KDE2_DATADIR}/kcontrol/pics")
add_subdirectory(sounds)
