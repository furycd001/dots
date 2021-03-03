add_subdirectory(pics)
add_subdirectory(htmlsearch)

set(libkhelpcenter_SRCS
        khc_factory.cc
        khc_navigator.cc
        khc_navigatoritem.cc
        khc_view.cc
)
qt2_wrap_cpp(libkhelpcenter_SRCS
    SOURCES
        khc_factory.h
        khc_main.h
        khc_navigator.h
        khc_navigator.h
        khc_navigator.h
        khc_searchwidget.h
        khc_view.h
        klanguagebutton.h
)

# TODO: fix build with anything other than pam (see the Makefile.am)

kde2_library(libkhelpcenter
    SOURCES
        ${libkhelpcenter_SRCS}
    LIBS
        kde2::khtml
    )

add_executable(khelpcenter2 main.cpp khc_main.cc)
target_link_libraries(khelpcenter2 kde2::khelpcenter)
install(TARGETS khelpcenter2 RUNTIME DESTINATION ${KDE2_BINDIR})

install(FILES
    khelpcenterui.rc intro.html.in glossary.html.in
    DESTINATION ${KDE2_DATADIR}/khelpcenter)

install(FILES khelpcenter.desktop DESTINATION ${KDE2_SERVICESDIR})
install(FILES Help.desktop DESTINATION ${KDE2_APPSDIR})


install(FILES
        plugins/Tutorials/.directory
        plugins/Tutorials/quickstart.desktop
        plugins/Tutorials/visualdict.desktop
    DESTINATION ${KDE2_DATADIR}/khelpcenter/plugins/Tutorials
    )
