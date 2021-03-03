kde2_stdicon()

qt2_wrap_cpp(kjobviewer_MOC
    SOURCES
        kjobviewer.h
    )

kde2_kinit_executable(kjobviewer
    SOURCES
        kjobviewer.cpp main.cpp
        ${kjobviewer_MOC}
    LIBS
        kde2::kdeui
        kde2::kdeprint_management
    )

install(FILES kjobviewerui.rc DESTINATION ${KDE2_DATADIR}/kjobviewer)
install(FILES kjobviewer.desktop DESTINATION ${KDE2_APPSDIR}/Utilities)

