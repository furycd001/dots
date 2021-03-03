qt2_wrap_cpp(libkuriikwsfilter_MOC
    SOURCES
        kuriikwsfilter.h
    )

kde2_kidl(libkuriikwsfilter_IDL
    SOURCES
        kuriikwsfilter.h
    )

kde2_module(libkuriikwsfilter
    SOURCES
        kuriikwsfiltereng.cpp
        kuriikwsfilter_skel.cpp
        kuriikwsfilter.cpp
        searchprovider.cpp
        ${libkuriikwsfilter_MOC}
        ${libkuriikwsfilter_IDL}
    LIBS
        kde2::ksycoca
    )

qt2_wrap_cpp(libkurisearchfilter_MOC
    SOURCES
        ikwsopts.h
        kurisearchfilter.h
        searchproviderdlg.h
    )
kde2_kidl(libkurisearchfilter_IDL
    SOURCES
        kurisearchfilter.h
    )
kde2_module(libkurisearchfilter
    SOURCES
        kurisearchfilter.cpp
        kuriikwsfiltereng.cpp
        ikwsopts.cpp
        searchproviderdlg.cpp
        searchprovider.cpp
        kurisearchfilter_skel.cpp
        ${libkurisearchfilter_MOC}
        ${libkurisearchfilter_IDL}
    LIBS
        kde2::ksycoca
    )

install(FILES searchprovider.desktop DESTINATION ${KDE2_SERVICETYPESDIR})
install(FILES kuriikwsfilter.desktop kurisearchfilter.desktop DESTINATION ${KDE2_SERVICESDIR})
install(FILES kuriikwsfilterrc DESTINATION ${KDE2_CONFDIR})

add_subdirectory(searchproviders)
