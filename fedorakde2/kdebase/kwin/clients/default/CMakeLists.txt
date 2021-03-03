## Default theme
qt2_wrap_cpp(default_MOC SOURCES kdedefault.h)
kde2_module(libkwindefault
    SOURCES kdedefault.cpp ${default_MOC} LIBS kde2::kwin)

qt2_wrap_cpp(default_config_MOC SOURCES config/config.h)
kde2_module(libkwindefault_config
    SOURCES config/config.cpp ${default_config_MOC} LIBS kde2::kwin)


