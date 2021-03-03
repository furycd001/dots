qt2_wrap_cpp(kxmlrpcd_MOC SOURCES kxmlrpcserver.h)
kde2_kinit_executable(kxmlrpcd
    SOURCES
        kxmlrpcserver.cpp
        kxmlrpcparser.cpp
        kxmlrpcdaemon.cpp
        kxmlrpcutil.cpp
        ${kxmlrpcd_MOC}
    LIBS
        kde2::ksycoca
    )

kde2_module(libkcm_xmlrpcd
    SOURCES
        kxmlrpcinit.cpp
    LIBS
        kde2::kdecore
    )

install(FILES kxmlrpcd.desktop DESTINATION ${KDE2_SERVICESDIR})

# TODO
#add_subdirectory(tests)
