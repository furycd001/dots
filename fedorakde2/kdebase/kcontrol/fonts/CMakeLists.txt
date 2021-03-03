set(kcm_fonts_SRCS
    fonts.cpp
)

qt2_wrap_cpp(kcm_fonts_SRCS
    SOURCES
        fonts.h
)

kde2_module(libkcm_fonts
    SOURCES
        ${kcm_fonts_SRCS}
    LIBS
        kde2::kdeui
    )

install(FILES fonts.desktop  DESTINATION "${KDE2_APPSDIR}/Settings/LookNFeel")
