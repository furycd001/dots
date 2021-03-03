qt2_wrap_cpp(kcm_kurifilt_MOC SOURCES filteropts.h main.h)
kde2_module(libkcm_kurifilt SOURCES filteropts.cpp main.cpp ${kcm_kurifilt_MOC} LIBS kde2::ksycoca)
install(FILES ebrowsing.desktop DESTINATION "${KDE2_APPSDIR}/Settings/WebBrowsing")

add_subdirectory(plugins/ikws)
add_subdirectory(plugins/shorturi)
