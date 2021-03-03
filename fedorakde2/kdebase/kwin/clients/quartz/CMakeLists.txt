## Quartz theme
qt2_wrap_cpp(quartz_MOC SOURCES quartz.h)
kde2_module(libkwinquartz
    SOURCES quartz.cpp ${quartz_MOC} LIBS kde2::kwin)
install(FILES quartz.desktop DESTINATION "${KDE2_DATADIR}/kwin")

qt2_wrap_cpp(quartz_config_MOC SOURCES config/config.h)
kde2_module(libkwinquartz_config
    SOURCES config/config.cpp ${quartz_config_MOC} LIBS kde2::kwin kde2::ksycoca)
