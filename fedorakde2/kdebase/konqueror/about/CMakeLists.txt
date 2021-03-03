set(konqaboutpage_SRCS
        konq_aboutpage.cc
    )

qt2_wrap_cpp(konqaboutpage_SRCS
    SOURCES
        konq_aboutpage.h
    )

kde2_module(konqaboutpage
    SOURCES
        ${konqaboutpage_SRCS}
    LIBS
        kde2::konq
        kde2::khtml
    )


install(FILES konq_aboutpage.desktop DESTINATION ${KDE2_SERVICESDIR})
install(FILES konqaboutpage.desktop DESTINATION ${KDE2_SERVICETYPESDIR})

install(FILES 
    back.png locationbar_erase.png background.png viewmag.png bgtable.png
    gohome.png window_fullscreen.png history.png view_left_right.png
    openterm.png lightning.png intro.html konq.css konqueror.png lines.png
    pointers.gif shadow1.png specs.html tips.html lines2.png kdelogo2.png
    more.png
    DESTINATION ${KDE2_DATADIR}/konqueror/about)

