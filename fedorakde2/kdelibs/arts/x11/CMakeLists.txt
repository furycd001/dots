
add_custom_command(
    OUTPUT x11globalcomm.h x11globalcomm.cc
    COMMAND mcopidl -I ${CMAKE_CURRENT_SOURCE_DIR}/../mcop/ ${CMAKE_CURRENT_SOURCE_DIR}/x11globalcomm.idl
    )
set(x11globalcomm_source
        x11globalcomm.cc x11globalcomm_impl.cc
    )

kde2_library(libx11globalcomm
    SOURCES
        ${x11globalcomm_source}
    HEADERS
        x11globalcomm.idl
    LIBS
        kde2::mcop
        X11::X11
    VERSION
        3.0.0
    )

