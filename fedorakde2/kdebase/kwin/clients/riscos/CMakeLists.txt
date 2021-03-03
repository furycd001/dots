qt2_wrap_cpp(riskos_MOC SOURCES 
    Button.h
    CloseButton.h
    HelpButton.h
    IconifyButton.h
    LowerButton.h
    Manager.h
    MaximiseButton.h
    StickyButton.h
    )

kde2_module(libkwinriskos
    SOURCES 
        Button.cpp
        CloseButton.cpp
        IconifyButton.cpp
        LowerButton.cpp
        Manager.cpp
        MaximiseButton.cpp
        StickyButton.cpp
        HelpButton.cpp
        Static.cpp
        ${riskos_MOC}
    LIBS
        kde2::kwin
    )
install(FILES riscos.desktop DESTINATION "${KDE2_DATADIR}/kwin")

