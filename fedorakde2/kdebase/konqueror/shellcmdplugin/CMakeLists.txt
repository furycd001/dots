set(shellcmdplugin_SRCS
        kshellcmdexecutor.cpp
        kshellcmddialog.cpp
        kshellcmdplugin.cpp
    )

qt2_wrap_cpp(shellcmdplugin_SRCS
    SOURCES
        kshellcmdexecutor.h
        kshellcmddialog.h
        kshellcmdplugin.h
    )

kde2_module(libkshellcmdplugin
    SOURCES
        ${shellcmdplugin_SRCS}
    LIBS
        kde2::konq
    )


install(FILES kshellcmdplugin.rc DESTINATION ${KDE2_DATADIR}/konqiconview/kpartplugins)
install(FILES kshellcmdplugin.rc DESTINATION ${KDE2_DATADIR}/konqlistview/kpartplugins)

