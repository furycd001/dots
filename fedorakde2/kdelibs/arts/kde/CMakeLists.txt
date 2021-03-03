
add_custom_command(
    OUTPUT artskde.h artskde.cc
    COMMAND mcopidl -I ${CMAKE_CURRENT_SOURCE_DIR}/../flow/ -I ${CMAKE_CURRENT_SOURCE_DIR}/../soundserver/ ${CMAKE_CURRENT_SOURCE_DIR}/artskde.idl
    )
set(artskde_SRC
    artskde.cc kioinputstream_impl.cpp kplayobject.cc
    kplayobjectfactory.cc kartsfloatwatch.cpp kartsdispatcher.cc
    )

qt2_wrap_cpp(artskde_SRC
    SOURCES
        kartsdispatcher.h
        kartsfloatwatch.h
        kioinputstream_impl.h
        kplayobject.h
    )

kde2_library(libartskde
    SOURCES
        ${artskde_SRC}
    HEADERS
        kplayobject.h
        kplayobjectfactory.h
        kartsfloatwatch.h
        kartsdispatcher.h
        artskde.idl
    LIBS
        kde2::qtmcop
        kde2::kfile
        kde2::kmedia2
    VERSION
        3.0.0
    )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/artskde.h DESTINATION ${KDE2_INCLUDEDIR}/artskde/)
