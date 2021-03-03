qt2_wrap_cpp(khotkeys_MOC
    SOURCES
        khkglobalaccel.h
        khotkeys.h
    )

kde2_kidl(khotkeys_IDL
    SOURCES
        khotkeys.h
    )

kde2_kinit_executable(khotkeys
    SOURCES
        main.cpp
        khkglobalaccel.cpp
        khotkeys.cpp
        khotkeys_skel.cpp

        ${khotkeys_MOC}
        ${khotkeys_IDL}
    LIBS
        kde2::hotkeys
        kde2::ksycoca
    )

