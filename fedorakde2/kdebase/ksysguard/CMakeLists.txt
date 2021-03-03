add_subdirectory(gui)
add_subdirectory(CContLib)
add_subdirectory(ksysguardd)
add_subdirectory(pics)
add_subdirectory(example)

install(FILES hi16-app-ksysguard.png DESTINATION icons/hicolor/16x16/ RENAME ksysguard2.png)
install(FILES hi32-app-ksysguard.png DESTINATION icons/hicolor/32x32/ RENAME ksysguard2.png)
install(FILES hi48-app-ksysguard.png DESTINATION icons/hicolor/48x48/ RENAME ksysguard2.png)

install(TARGETS ksysguard2 RUNTIME DESTINATION ${KDE2_BINDIR})
