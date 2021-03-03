qt2_wrap_cpp(libkcm_hotkeys_MOC SOURCES kcmkhotkeys.h)
kde2_module(libkcm_hotkeys
    SOURCES
        kcmkhotkeys.cpp
        ${libkcm_hotkeys_MOC}
    LIBS
        kde2::hotkeys
        kde2::kdeui
    )
