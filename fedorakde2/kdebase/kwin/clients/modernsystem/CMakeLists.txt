## Modern System theme
qt2_wrap_cpp(modernsys_MOC SOURCES modernsys.h)
kde2_module(libkwinmodernsys
    SOURCES modernsys.cpp ${modernsys_MOC} LIBS kde2::kwin)
install(FILES modernsystem.desktop DESTINATION "${KDE2_DATADIR}/kwin")

qt2_wrap_cpp(modernsys_config_MOC SOURCES config/config.h)
kde2_module(libkwinmodernsys_config
    SOURCES config/config.cpp ${modernsys_config_MOC} LIBS kde2::kwin kde2::ksycoca)
