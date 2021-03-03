set(ARTS_MAJOR_VERSION 0)
set(ARTS_MINOR_VERSION 6)
set(ARTS_MICRO_VERSION 1)
set(ARTS_VERSION ${ARTS_MAJOR_VERSION}.${ARTS_MINOR_VERSION}.${ARTS_MICRO_VERSION})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/artsversion.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/artsversion.h)

add_custom_command(
    OUTPUT kmedia2.h kmedia2.cc
    COMMAND mcopidl -I${CMAKE_CURRENT_SOURCE_DIR}/../flow/ -I${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/kmedia2.idl
    )
add_custom_command(
    OUTPUT soundserver.h soundserver.cc
    COMMAND mcopidl -I${CMAKE_CURRENT_SOURCE_DIR}/../flow/ -I${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/soundserver.idl
    )

kde2_library(libkmedia2
    SOURCES
        fileinputstream_impl.cc
        stdoutwriter_impl.cc
        soundserver.cc
        kmedia2.cc
    LIBS
        kde2::mcop
        kde2::artsflow
    HEADERS
        ${CMAKE_CURRENT_BINARY_DIR}/soundserver.h
        kmedia2.idl
        soundserver.idl
    VERSION
        3.0.0
    )

kde2_library(libsoundserver
    SOURCES
        soundserver.cc
    LIBS
        kde2::kmedia2
    VERSION
        3.0.0
    )

kde2_library(libartswavplayobject
    SOURCES
        wavplayobject_impl.cc
    LIBS
        kde2::mcop
        kde2::artsflow
        kde2::soundserver
    VERSION
        3.0.0
    )

kde2_kinit_executable(artsd
    SOURCES
        soundserverv2_impl.cc soundserver_impl.cc
        simplesoundserver_impl.cc artsd.cc cpuusage.cc
        samplestorage_impl.cc
    LIBS
        kde2::mcop
        kde2::artsflow
        kde2::soundserver
    )

add_executable(artsplay artsplay.cc)
target_link_libraries(artsplay
    kde2::mcop
    kde2::artsflow
    kde2::soundserver
    )

add_executable(artscat artscat.cc)
target_link_libraries(artscat
    kde2::mcop
    kde2::artsflow
    kde2::soundserver
    )
add_executable(artswrapper artswrapper.c)
target_compile_definitions(artswrapper
    PRIVATE
    -DEXECUTE="${CMAKE_INSTALL_FULL_BINDIR}/$<TARGET_FILE_NAME:artsd>"
)

add_executable(artsshell artsshell.cc)
target_link_libraries(artsshell
    kde2::mcop
    kde2::artsflow
    kde2::soundserver
    )

install(TARGETS artsplay artscat artswrapper artsshell)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/artsversion.h
        ${CMAKE_CURRENT_BINARY_DIR}/soundserver.h
        ${CMAKE_CURRENT_BINARY_DIR}/kmedia2.h
    DESTINATION ${KDE2_INCLUDEDIR}/kmedia2/)
