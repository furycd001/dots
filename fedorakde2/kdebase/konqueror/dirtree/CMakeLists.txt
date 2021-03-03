set(dirtree_SRCS
    konq_tree.cpp konq_treepart.cpp konq_treeitem.cpp konq_treetoplevelitem.cpp
    bookmark_module/bookmark_module.cpp bookmark_module/bookmark_item.cpp
    dirtree_module/dirtree_module.cpp dirtree_module/dirtree_item.cpp
    history_module/history_module.cpp history_module/history_item.cpp history_module/history_settings.cpp
    )

kde2_wrap_kidl(dirtree_SRCS
    SOURCES
        history_module/history_settings.h
    )

qt2_wrap_cpp(dirtree_SRCS
    SOURCES
        konq_tree.h
        konq_treepart.h
        bookmark_module/bookmark_module.h
        dirtree_module/dirtree_module.h
        history_module/history_module.h
        history_module/history_settings.h
    )

qt2_wrap_ui(dirtree_SRCS
    SOURCES
        history_module/history_dlg.ui
    )

kde2_module(libkonqtree
    SOURCES
        ${dirtree_SRCS}
    LIBS
        kde2::konq
    )

install(FILES konq_dirtree.desktop DESTINATION ${KDE2_SERVICESDIR})

install(FILES
    init/home.desktop
    init/root.desktop
    init/history.desktop
    init/bookmarks.desktop
    DESTINATION ${KDE2_DATADIR}/konqueror/dirtree)

install(FILES
    init/remote/.directory
    DESTINATION ${KDE2_DATADIR}/konqueror/dirtree/remote)

install(FILES
    init/remote/web/apps_web.desktop
    init/remote/web/dot_web.desktop
    init/remote/web/kde_web.desktop
    init/remote/web/.directory
    DESTINATION ${KDE2_DATADIR}/konqueror/dirtree/remote/web)

install(FILES
    init/remote/ftp/.directory
    init/remote/ftp/kde_ftp.desktop
    DESTINATION ${KDE2_DATADIR}/konqueror/dirtree/remote/ftp)

install(FILES
    init/services/.directory
    init/services/audiocd.desktop
    DESTINATION ${KDE2_DATADIR}/konqueror/dirtree/services)
