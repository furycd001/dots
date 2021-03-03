qt2_wrap_cpp(common_moc_SRCS
    SOURCES
        ColorPicker.h
        FancyPlotter.h
        SignalPlotter.h
        SensorDisplay.h
        StyleEngine.h
        SensorManager.h
        SensorAgent.h
        SensorShellAgent.h
        SensorSocketAgent.h
    )

set(ksysguard_SRCS
        ColorPicker.cc
        SensorDisplay.cc
        DummyDisplay.cc
        FancyPlotter.cc
        SignalPlotter.cc
        MultiMeter.cc
        DancingBars.cc
        BarGraph.cc
        ProcessController.cc
        ProcessList.cc
        ListView.cc
        LogFile.cc
        LogFileSettings.cc
        StyleEngine.cc
        SensorLogger.cc
        SensorBrowser.cc
        SensorManager.cc
        SensorAgent.cc
        SensorShellAgent.cc
        SensorSocketAgent.cc
        WorkSheet.cc
        Workspace.cc
        ksysguard.cc
        ksysguard_stub.cpp
        ${common_moc_SRCS}
)

qt2_wrap_ui(ksysguard_SRCS
    SOURCES
	DancingBarsSettings.ui MultiMeterSettings.ui
	FancyPlotterSettings.ui HostConnector.ui WorkSheetSettings.ui
	ListViewSettings.ui BarGraphSettings.ui StyleSettings.ui SensorLoggerDlg.ui SensorLoggerSettings.ui
    )
qt2_wrap_cpp(ksysguard_SRCS
    SOURCES
        BarGraph.h
        DancingBars.h
        DummyDisplay.h
        KSysGuardApplet.h
        ListView.h
        LogFile.h
        LogFileSettings.h
        MultiMeter.h
        ProcessController.h
        ProcessList.h
        ReniceDlg.h
        SensorBrowser.h
        SensorLogger.h
        SensorLogger.h
        WorkSheet.h
        WorkSheetSetup.h
        Workspace.h
        ksysguard.h
    )

kde2_wrap_kidl(ksysguard_SRCS
    SOURCES
        ksysguard.h
    )

add_executable(ksysguard2 ${ksysguard_SRCS})
target_link_libraries(ksysguard2
    kde2::kdeui
    kde2::kfile
    )

install(TARGETS ksysguard2 RUNTIME DESTINATION ${KDE2_BINDIR})

install(FILES ksysguard.desktop DESTINATION ${KDE2_APPSDIR}/System)
install(FILES ksysguardui.rc DESTINATION ${KDE2_DATADIR}/ksysguard2)
install(FILES x-ksysguard.desktop DESTINATION ${KDE2_MIMEDIR}/application)

install(
    FILES
        ProcessTable.sgrd SystemLoad.sgrd KSysGuardApplet.xml
    DESTINATION ${KDE2_DATADIR}/ksysguard2
    )

set(ksysguardapplet_SRCS
        ColorPicker.cc
        FancyPlotter.cc
        SignalPlotter.cc
        SensorDisplay.cc
        StyleEngine.cc
        SensorManager.cc
        SensorAgent.cc
        SensorShellAgent.cc
        SensorSocketAgent.cc
        KSysGuardApplet.cc
        ${common_moc_SRCS}
    )

qt2_wrap_cpp(ksysguardapplet_SRCS
    SOURCES
        KSysGuardApplet.h
)

qt2_wrap_ui(ksysguardapplet_SRCS
    SOURCES
	FancyPlotterSettings.ui HostConnector.ui KSysGuardAppletSettings.ui
	StyleSettings.ui
    )

kde2_module(libksysguardapplet
    SOURCES
        ${ksysguardapplet_SRCS}
    LIBS
        kde2::kdeui
    )

install(FILES ksysguardapplet.desktop DESTINATION "${KDE2_DATADIR}/kicker/applets/")


