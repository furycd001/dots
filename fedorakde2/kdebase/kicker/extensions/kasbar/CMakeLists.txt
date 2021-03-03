qt2_wrap_cpp(kasbarextension_MOC
    SOURCES
        kasaboutdlg.h
        kasbarextension.h
        kasbar.h
        kasgroupitem.h
        kasgrouppopup.h
        kasitem.h
        kaspopup.h
        kasprefsdlg.h
        kasstartupitem.h
        kastasker.h
        kastaskitem.h
        kastaskpopup.h
    )

kde2_module(libkasbarextension
    SOURCES
        kasbar.cpp
        kasitem.cpp
        kasbarextension.cpp
        kastaskitem.cpp
        kastasker.cpp
        kaspopup.cpp
        kastaskpopup.cpp
        kasstartupitem.cpp
        kasaboutdlg.cpp
        kasprefsdlg.cpp
        kasgroupitem.cpp
        kasgrouppopup.cpp
        ${kasbarextension_MOC}
    LIBS
        kde2::taskmanager
)

install(FILES kasbarextension.desktop DESTINATION ${KDE2_DATADIR}/kicker/extensions)
