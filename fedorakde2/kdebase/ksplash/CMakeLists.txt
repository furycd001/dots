add_subdirectory(pics)

kde2_wrap_kidl(ksplash_IDL
    SOURCES
        ksplash.h
    )
qt2_wrap_cpp(ksplash_MOC
    SOURCES
        ksplash.h
    )

kde2_kinit_executable(ksplash
    SOURCES
        ksplash.cpp
        ${ksplash_IDL}
        ${ksplash_MOC}
    LIBS
        kde2::kdeui
    )
