qt2_wrap_cpp(kpager_MOC
    SOURCES
        config.h
        desktop.h
        kpager.h
    )

add_executable(kpager2
    desktop.cpp kpager.cpp config.cpp windowdrag.cpp main.cpp
    ${kpager_MOC}
    )

target_link_libraries(kpager2 kde2::kdeui)
install(FILES kpager.desktop DESTINATION ${KDE2_APPSDIR}/Utilities)
kde2_stdicon()
install(TARGETS kpager2 DESTINATION ${KDE2_BINDIR})

