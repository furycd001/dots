qt2_wrap_cpp(icewm_MOC SOURCES icewm.h)
kde2_module(libkwinicewm
    SOURCES icewm.cpp ${icewm_MOC} LIBS kde2::kwin)
install(FILES icewm.desktop DESTINATION "${KDE2_DATADIR}/kwin")

qt2_wrap_cpp(icewm_config_MOC SOURCES config/config.h)
kde2_module(libkwinicewm_config
    SOURCES config/config.cpp ${icewm_config_MOC} LIBS kde2::kwin kde2::ksycoca)


install(FILES
        icewm-themes/titleAB.xpm
        icewm-themes/titleAJ.xpm
        icewm-themes/titleAM.xpm
        icewm-themes/titleAP.xpm
        icewm-themes/titleAQ.xpm
        icewm-themes/titleAR.xpm
        icewm-themes/titleAS.xpm
        icewm-themes/titleAT.xpm
        icewm-themes/titleIB.xpm
        icewm-themes/titleIJ.xpm
        icewm-themes/titleIM.xpm
        icewm-themes/titleIQ.xpm
        icewm-themes/titleIR.xpm
        icewm-themes/titleIS.xpm
        icewm-themes/titleIT.xpm
        icewm-themes/titleIP.xpm
        icewm-themes/closeA.xpm
        icewm-themes/closeI.xpm
        icewm-themes/depthA.xpm
        icewm-themes/depthI.xpm
        icewm-themes/maximizeA.xpm
        icewm-themes/maximizeI.xpm
        icewm-themes/menuButtonA.xpm
        icewm-themes/menuButtonI.xpm
        icewm-themes/minimizeA.xpm
        icewm-themes/minimizeI.xpm
        icewm-themes/restoreA.xpm
        icewm-themes/restoreI.xpm
        icewm-themes/rolldownA.xpm
        icewm-themes/rolldownI.xpm
        icewm-themes/rollupA.xpm
        icewm-themes/rollupI.xpm
        icewm-themes/default.theme
    DESTINATION
        "${KDE2_DATADIR}/kwin/icewm-themes"
    )
