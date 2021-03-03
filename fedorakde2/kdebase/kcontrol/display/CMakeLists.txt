qt2_wrap_cpp(kcm_style_MOC SOURCES general.h)

kde2_module(libkcm_style SOURCES general.cpp krdb.cpp ${kcm_style_MOC} LIBS kde2::ksycoca)
install(FILES style.desktop DESTINATION "${KDE2_APPSDIR}/Settings/LookNFeel")
add_subdirectory(kcs)
add_subdirectory(ad)
add_subdirectory(pics)
