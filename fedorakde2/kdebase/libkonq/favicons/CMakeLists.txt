kde2_kidl(favicons_IDL
    SOURCES
        favicons.h
    )
qt2_wrap_cpp(favicons_MOC
    SOURCES
        favicons.h
    )

kde2_module(libkded_favicons
    SOURCES
        favicons.cpp
        favicons_skel.cpp
        ${favicons_IDL}
        ${favicons_MOC}
    LIBS
        kde2::ksycoca
        kde2::kio
        kde2::kdedmodule
    )

