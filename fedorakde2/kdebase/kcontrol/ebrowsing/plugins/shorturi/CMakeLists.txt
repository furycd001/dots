qt2_wrap_cpp(libkshorturi_MOC SOURCES kshorturifilter.h)
kde2_kidl(libkshorturi_IDL SOURCES kshorturifilter.h)
kde2_module(libkshorturifilter
    SOURCES
        kshorturifilter.cpp
        kshorturifilter_skel.cpp
        ${libkshorturi_MOC}
        ${libkshorturi_IDL}
    LIBS
        kde2::ksycoca
    )

install(FILES kshorturifilter.desktop DESTINATION ${KDE2_SERVICESDIR})
install(FILES kshorturifilterrc DESTINATION ${KDE2_CONFDIR})
