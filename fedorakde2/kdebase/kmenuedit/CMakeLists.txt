kde2_stdicon()

qt2_wrap_cpp(kmenuedit_MOC
    SOURCES
        basictab.h
        desktopfileeditor.h
        kmenuedit.h
        menueditview.h
        treeview.h
    )

kde2_kinit_executable(kmenuedit
    SOURCES
        namedlg.cpp basictab.cpp treeview.cpp desktopfileeditor.cpp
        menueditview.cpp kmenuedit.cpp main.cpp khotkeys.cpp
        ${kmenuedit_MOC}
    LIBS
        kde2::kdeui
        kde2::kfile
    )

install(FILES kmenuedit.desktop DESTINATION ${KDE2_APPSDIR}/System/)
install(FILES kmenueditui.rc DESTINATION ${KDE2_DATADIR}/kmenuedit)

add_subdirectory(pixmaps)
