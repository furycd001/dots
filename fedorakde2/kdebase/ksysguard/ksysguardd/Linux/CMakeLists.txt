add_executable(ksysguardd2
    ksysguardd.c Command.c ProcessList.c
    Memory.c stat.c netdev.c PWUIDCache.c apm.c loadavg.c cpuinfo.c
    lmsensors.c netstat.c diskstat.c logfile.c conf.c
    )

include(GNUInstallDirs)
add_definitions(-DVERSION=\"2.2.2\" -DKSYSGUARDDRCFILE=\"${CMAKE_INSTALL_FULL_SYSCONFDIR}/ksysguarddrc\")

include(CheckIncludeFiles)
check_include_files(sensors/sensors.h HAVE_SENSORS_SENSORS_H)
if(${HAVE_SENSORS_SENSORS_H})
    add_definitions(-DHAVE_SENSORS_SENSORS_H)
endif()

# TODO: actually use findsensors.cmake
target_link_libraries(ksysguardd2
    kde2::ccont
    sensors
    )

install(TARGETS ksysguardd2 RUNTIME DESTINATION ${KDE2_BINDIR})
