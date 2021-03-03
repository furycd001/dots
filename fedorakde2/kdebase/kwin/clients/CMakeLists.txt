# could probably make a macro or something, if I knew cmake

## KDE1 theme
qt2_wrap_cpp(kde1_MOC SOURCES kde1/kde1client.h)
kde2_module(libkwinkde1
    SOURCES kde1/kde1client.cpp ${kde1_MOC} LIBS kde2::kwin)
install(FILES kde1/kde1.desktop DESTINATION "${KDE2_DATADIR}/kwin")

## Redmond theme
qt2_wrap_cpp(redmond_MOC SOURCES redmond/redmond.h)
kde2_module(libkwinredmond
    SOURCES redmond/redmond.cpp ${redmond_MOC} LIBS kde2::kwin)
install(FILES redmond/redmond.desktop DESTINATION "${KDE2_DATADIR}/kwin")

## MWM theme
qt2_wrap_cpp(mwm_MOC SOURCES mwm/mwmclient.h)
kde2_module(libkwinmwm
    SOURCES mwm/mwmclient.cpp ${mwm_MOC} LIBS kde2::kwin)
install(FILES mwm/mwm.desktop DESTINATION "${KDE2_DATADIR}/kwin")

## B2 theme
qt2_wrap_cpp(b2_MOC SOURCES b2/b2client.h)
kde2_module(libkwinb2
    SOURCES b2/b2client.cpp ${b2_MOC} LIBS kde2::kwin)
install(FILES b2/b2.desktop DESTINATION "${KDE2_DATADIR}/kwin")

## Laptop theme
qt2_wrap_cpp(laptop_MOC SOURCES laptop/laptopclient.h)
kde2_module(libkwinlaptop
    SOURCES laptop/laptopclient.cpp ${laptop_MOC} LIBS kde2::kwin)
install(FILES laptop/laptop.desktop DESTINATION "${KDE2_DATADIR}/kwin")

## System theme
qt2_wrap_cpp(system_MOC SOURCES system/systemclient.h)
kde2_module(libkwinsystem
    SOURCES system/systemclient.cpp ${system_MOC} LIBS kde2::kwin)
install(FILES system/system.desktop DESTINATION "${KDE2_DATADIR}/kwin")

## KStep theme
qt2_wrap_cpp(kstep_MOC SOURCES kstep/nextclient.h)
kde2_module(libkwinkstep
    SOURCES kstep/nextclient.cpp ${kstep_MOC} LIBS kde2::kwin)
install(FILES kstep/kstep.desktop DESTINATION "${KDE2_DATADIR}/kwin")

add_subdirectory(default)
add_subdirectory(icewm)
add_subdirectory(kwmtheme)
add_subdirectory(modernsystem)
add_subdirectory(quartz)
add_subdirectory(riscos)
add_subdirectory(web)
