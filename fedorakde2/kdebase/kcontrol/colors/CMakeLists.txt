qt2_wrap_cpp(kcm_colors_MOC SOURCES colorscm.h widgetcanvas.h)

kde2_module(libkcm_colors SOURCES colorscm.cpp widgetcanvas.cpp ${kcm_colors_MOC} LIBS kde2::kdeui)
install(FILES colors.desktop DESTINATION "${KDE2_APPSDIR}/Settings/LookNFeel")
