kde2_module(kio_print
    SOURCES
        kio_print.cpp
    LIBS
        kde2::kio
        kde2::kdeprint
    )

add_subdirectory(pics)

install(
    FILES
        mimetypes/folder.desktop
        mimetypes/printer.desktop
        mimetypes/class.desktop
        mimetypes/manager.desktop
    DESTINATION
        ${KDE2_MIMEDIR}/print
    )

install(
    FILES
        templates/class.template
        templates/printer.template
    DESTINATION
        "${KDE2_DATADIR}/kdeprint/template"
    )
