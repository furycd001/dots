include(GNUInstallDirs)
install(FILES eventsrc DESTINATION ${KDE2_DATADIR}/ksysguard2)
install(FILES ksysguarddrc DESTINATION ${CMAKE_INSTALL_FULL_SYSCONFDIR} RENAME ksysguardd2rc)
