kde2_stdicon()


qt2_wrap_ui(kpersonalizer_UI
    SOURCES
        kcountrypagedlg.ui kospagedlg.ui
        keyecandypagedlg.ui kstylepagedlg.ui
        krefinepagedlg.ui
    )

qt2_wrap_cpp(kpersonalizer_MOC
    SOURCES
        kcountrypage.h
        keyecandypage.h
        klanguagebutton.h
        kospage.h
        kpersonalizer.h
        krefinepage.h
        kstylepage.h
    )
kde2_kinit_executable(kpersonalizer
    SOURCES
        klocaleadv.cpp klanguagebutton.cpp krefinepage.cpp kstylepage.cpp
        keyecandypage.cpp kospage.cpp kcountrypage.cpp kpersonalizer.cpp
        main.cpp

        ${kpersonalizer_UI}
        ${kpersonalizer_MOC}
    LIBS
        kde2::kdeui
        kde2::ksycoca
    COMPILE_DEFINITIONS
        -DVERSION=\"2.2.2\"
    )

install(FILES kpersonalizer.desktop DESTINATION ${KDE2_APPSDIR}/System)
install(FILES kpersonalizer.desktop DESTINATION ${KDEDIR}/share/autostart)

add_subdirectory(pics)
