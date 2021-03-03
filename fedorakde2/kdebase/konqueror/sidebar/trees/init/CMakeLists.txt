install(
    FILES
        home.desktop root.desktop history.desktop bookmarks.desktop
        services.desktop remote.desktop
    DESTINATION
        ${KDE2_DATADIR}/konqsidebartng/entries
    )

install(
    FILES
        virtualfolderadd.desktop
    DESTINATION
        ${KDE2_DATADIR}/konqsidebartng/add
    )

install(
    FILES
        history_module.desktop dirtree_module.desktop bookmarks_module.desktop
    DESTINATION
        ${KDE2_DATADIR}/konqsidebartng/dirtree
    )

install(
    FILES
        remote/.directory
    DESTINATION
        ${KDE2_DATADIR}/konqsidebartng/virtual_folders/remote
    )

install(
    FILES
        remote/ftp/.directory
        remote/ftp/kde_ftp.desktop
    DESTINATION
        ${KDE2_DATADIR}/konqsidebartng/virtual_folders/remote/ftp
    )

install(
    FILES
        remote/web/.directory
        remote/web/apps_web.desktop
        remote/web/dot_web.desktop
        remote/web/kde_web.desktop
    DESTINATION
        ${KDE2_DATADIR}/konqsidebartng/virtual_folders/remote/web
    )
