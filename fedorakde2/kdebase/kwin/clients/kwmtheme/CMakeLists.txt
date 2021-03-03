## KWM theme
qt2_wrap_cpp(kwmtheme_MOC SOURCES kwmthemeclient.h)
kde2_module(libkwinkwmtheme
    SOURCES kwmthemeclient.cpp ${kwmtheme_MOC} LIBS kde2::kwin)
install(FILES kwmtheme.desktop DESTINATION "${KDE2_DATADIR}/kwin")

add_executable(kwmtheme cli_installer/main.cpp)
target_link_libraries(kwmtheme kde2::kdecore)
install(TARGETS kwmtheme RUNTIME DESTINATION ${KDE2_BINDIR})
