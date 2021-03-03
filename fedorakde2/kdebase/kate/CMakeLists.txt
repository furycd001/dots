#add_subdirectory(qt3back)
#add_subdirectory(document)
#add_subdirectory(view)
#add_subdirectory(app)
#add_subdirectory(console)
#add_subdirectory(filelist)
#add_subdirectory(fileselector)
#add_subdirectory(mainwindow)
#add_subdirectory(pluginmanager)
#add_subdirectory(factory)
#add_subdirectory(interfaces)
#add_subdirectory(main)
add_subdirectory(data)
add_subdirectory(pics)

set(qt3back_SRCS
    qt3back/qregexp3.cpp
    )
set(katedocument_SRCS
    document/katedocmanager.cpp
    document/katecmd.cpp
    document/katedocument.cpp
    document/katevmallocator.cpp
    document/katetextline.cpp
    document/katebuffer.cpp
    document/katehighlight.cpp
    document/katesyntaxdocument.cpp
    document/katedialogs.cpp
    document/katecmds.cpp

    katedocumentIface_skel.cpp
    )
set(kateview_SRCS
    view/kateviewspace.cpp
    view/kateviewmanager.cpp
    view/katesplitter.cpp
    view/kateview.cpp
    view/kateundohistory.cpp
    view/kateviewdialog.cpp

    kateviewIface_skel.cpp
    )
set(kateapp_SRCS
    app/kateapp.cpp
    kateappIface_skel.cpp
    )
set(kateconsole_SRCS
    console/kateconsole.cpp
    )
set(katefilelist_SRCS
    filelist/katefilelist.cpp
    )
set(katefileselector_SRCS
    fileselector/katefileselector.cpp
    )
set(katemainwindow_SRCS
    mainwindow/katemainwindow.cpp
    mainwindow/kateconfigdialog.cpp

    kateIface_skel.cpp
    )
set(katepluginmanager_SRCS
    pluginmanager/katepluginmanager.cpp
    pluginmanager/kateconfigplugindialogpage.cpp
    )
set(katefactory_SRCS
    factory/katefactory.cpp
    )
set(kateinterfaces_SRCS
    interfaces/interfaces.cpp
    )
set(katemain_SRCS
    interfaces/interfaces.cpp
    )

qt2_wrap_cpp(kate_MOC
    SOURCES
        app/kateapp.h
        console/kateconsole.h
        document/katebuffer.h
        document/katecmd.h
        document/katedialogs.h
        document/katedocmanager.h
        document/katedocument.h
        document/katehighlight.h
        factory/katefactory.h
        filelist/katefilelist.h
        fileselector/katefileselector.h
        interfaces/application.h
        interfaces/docmanager.h
        interfaces/document.h
        interfaces/mainwindow.h
        interfaces/plugin.h
        interfaces/view.h
        interfaces/viewmanager.h
        main/kwritemain.h
        mainwindow/kateconfigdialog.h
        mainwindow/katemainwindow.h
        pluginmanager/kateconfigplugindialogpage.h
        pluginmanager/katepluginmanager.h
        view/katesplitter.h
        view/kateundohistory.h
        view/kateviewdialog.h
        view/kateview.h
        view/kateviewmanager.h
        view/kateviewspace.h
)

set(kate_SRCS
    ${qt3back_SRCS}
    ${katedocument_SRCS}
    ${kateview_SRCS}
    ${kateapp_SRCS}
    ${kateconsole_SRCS}
    ${katefilelist_SRCS}
    ${katefileselector_SRCS}
    ${katemainwindow_SRCS}
    ${katepluginmanager_SRCS}
    ${katefactory_SRCS}
    ${kateinterfaces_SRCS}
    ${kate_MOC}
    )
kde2_kidl(kate_SRCS
    SOURCES
        document/katedocumentIface.h
        view/kateviewIface.h
        app/kateappIface.h
        mainwindow/kateIface.h
    )

kde2_library(katecore
    SOURCES
        ${kate_SRCS}
    LIBS
        kde2::ktexteditor kde2::kspell kde2::kdeprint
    )
target_include_directories(katecore PRIVATE qt3back)

# TODO reuse
include(GNUInstallDirs)
get_target_property(LAPROP_SONAME katecore OUTPUT_NAME)
set(LAPROP_SONAME "lib${LAPROP_SONAME}")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/libkatecore.la.in ${CMAKE_CURRENT_BINARY_DIR}/libkatecore.la @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libkatecore.la DESTINATION "${CMAKE_INSTALL_LIBDIR}/kde2")

kde2_kinit_executable(kate2
    SOURCES
        main/katemain.cpp
    LIBS
        kde2::katecore
    )

kde2_kinit_executable(kwrite2
    SOURCES
        main/kwritemain.cpp
    LIBS
        kde2::katecore
    )

#set(katekdevelop_SRCS
#    kdevelop/kate_part.cpp
#    kdevelop/kate_factory.cpp
#    kdevelop/document_impl.cpp
#    kdevelop/clipboard_iface_impl.cpp
#    kdevelop/cursor_iface_impl.cpp
#    kdevelop/undo_iface_impl.cpp
#    kdevelop/edit_iface_impl.cpp
#    kdevelop/status_iface_impl.cpp
#    kdevelop/debug_iface_impl.cpp
#    )
#qt2_wrap_cpp(katekdevelop_SRCS
#    SOURCES
#        kdevelop/clipboard_iface_impl.h
#        kdevelop/cursor_iface_impl.h
#        kdevelop/debug_iface_impl.h
#        kdevelop/document_impl.h
#        kdevelop/edit_iface_impl.h
#        kdevelop/kate_factory.h
#        kdevelop/kate_part.h
#        kdevelop/status_iface_impl.h
#        kdevelop/undo_iface_impl.h
#    )
#kde2_module(kateeditorpart
#    SOURCES
#        ${katekdevelop_SRC
#    LIBS
#        kde2::katecore
#    )
#
