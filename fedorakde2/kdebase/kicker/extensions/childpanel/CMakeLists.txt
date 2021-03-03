qt2_wrap_cpp(childpanelextension_MOC
    SOURCES
        childpanelextension.h
    )
kde2_wrap_kidl(childpanelextension_IDL
    SOURCES
        childpanelextension.h
    )

kde2_module(libchildpanelextension
    SOURCES
        childpanelextension.cpp
        ${childpanelextension_IDL}
        ${childpanelextension_MOC}
    LIBS
        kinit_kicker
)

install(FILES childpanelextension.desktop DESTINATION ${KDE2_DATADIR}/kicker/extensions)
