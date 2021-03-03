set(kicker_SRCS
    #share/pluginmgr.cpp
    #    share/appletinfo.cpp

        core/panelbutton.cpp
        core/main.cpp
        core/global.cpp
        core/panel.cpp
        core/panner.cpp
        core/userrectsel.cpp
        core/containerarea.cpp
        core/panelbuttonbase.cpp
        core/applethandle.cpp
        core/container_base.cpp
        core/container_panel.cpp
        core/container_button.cpp
        core/container_applet.cpp
        core/containerareabox.cpp
        core/container_extension.cpp
        core/extensionmanager.cpp

        ui/addbutton_mnu.cpp
        ui/addapplet_mnu.cpp
        ui/appletop_mnu.cpp
        ui/base_mnu.cpp
        ui/browser_mnu.cpp
        ui/client_mnu.cpp
        ui/dirdrop_mnu.cpp
        ui/exe_dlg.cpp
        ui/k_mnu.cpp
        ui/panelop_mnu.cpp
        ui/quickbrowser_mnu.cpp
        ui/recent_mnu.cpp
        ui/service_mnu.cpp
        ui/addextension_mnu.cpp
        ui/extensionop_mnu.cpp
        ui/konsole_mnu.cpp
        ui/addcontainer_mnu.cpp
        ui/recentapps.cpp
        ui/browser_dlg.cpp
    )

kde2_wrap_kidl(kicker_SRCS
    SOURCES
        core/panel.h
    )

qt2_wrap_moc(kicker_SRCS
    SOURCES
        ui/kickermenumanager.h
    )

qt2_wrap_cpp(kicker_SRCS
    SOURCES
        core/applethandle.h
        core/container_applet.h
        core/containerareabox.h
        core/containerarea.h
        core/container_base.h
        core/container_button.h
        core/container_extension.h
        core/container_panel.h
        core/extensionmanager.h
        core/panelbuttonbase.h
        core/panelbutton.h
        core/panel.h
        core/panner.h
        core/userrectsel.h

        ui/addapplet_mnu.h
        ui/addbutton_mnu.h
        ui/addcontainer_mnu.h
        ui/addextension_mnu.h
        ui/base_mnu.h
        ui/browser_dlg.h
        ui/browser_mnu.h
        ui/client_mnu.h
        ui/k_mnu.h
        ui/konsole_mnu.h
        ui/panelop_mnu.h
        ui/quickbrowser_mnu.h
        ui/recent_mnu.h
        ui/service_mnu.h
    )

#set(libkicker_SRCS
#        share/pluginmgr.cpp
#        share/appletinfo.cpp
#    )
#kde2_library(libkicker
#    SOURCES
#    HEADERS
#        share/appletinfo.h
#        share/pluginmgr.h
#    LIBS
#        kde2::konq
#   )
#
kde2_kinit_executable(kicker
    SOURCES
        ${kicker_SRCS}
    LIBS
        kde2::kicker
    )
target_include_directories(kinit_kicker PUBLIC ui core share)

install(FILES core/default-apps DESTINATION ${KDE2_DATADIR}/kicker)
install(FILES panel.desktop DESTINATION ${KDEDIR}/share/autostart)

add_subdirectory(share)
add_subdirectory(proxy)
add_subdirectory(taskmanager)
add_subdirectory(taskbar)
add_subdirectory(applets)
add_subdirectory(extensions)
add_subdirectory(data)
