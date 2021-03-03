qt2_wrap_cpp(kcm_locale_MOC
    SOURCES
        klanguagebutton.h
        klocalesample.h
        locale.h
        localemon.h
        localenum.h
        localetime.h
        toplevel.h
    )

kde2_module(libkcm_locale
    SOURCES
        klocaleadv.cpp main.cpp toplevel.cpp locale.cpp klanguagebutton.cpp
        klocalesample.cpp localenum.cpp localemon.cpp localetime.cpp
        ${kcm_locale_MOC}
    LIBS
        kde2::kdeui
    )
install(FILES language.desktop DESTINATION "${KDE2_APPSDIR}/Settings/Personalization")
