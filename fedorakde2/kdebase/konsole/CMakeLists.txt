#TODO:
#add_subdirectory(doc)

add_subdirectory(src)
add_subdirectory(other)
add_subdirectory(fonts)

install(FILES konsole.desktop konsolesu.desktop DESTINATION "${KDE2_APPSDIR}/System")
install(FILES konsole.upd DESTINATION "${KDE2_DATADIR}/kconf_update")
install(FILES kwrited.desktop DESTINATION ${KDEDIR}/share/autostart)
install(FILES konsolepart.desktop DESTINATION ${KDE2_SERVICESDIR})
