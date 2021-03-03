qt2_wrap_cpp(kdeprint_part_MOC
    SOURCES
        printpartfactory.h
        printpart.h
    )
kde2_module(libkdeprint_part
    SOURCES
        printpartfactory.cpp printpart.cpp
        ${kdeprint_part_MOC}
    LIBS
        kde2::kdeprint_management
    )

install(FILES kdeprint_part.desktop DESTINATION ${KDE2_SERVICESDIR})
install(FILES kdeprint_part.rc DESTINATION ${KDE2_DATADIR}/kdeprint_part)


