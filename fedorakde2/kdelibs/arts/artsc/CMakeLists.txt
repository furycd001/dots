kde2_library(libartsdsp_st
    SOURCES
        artsc.c artsdsp.c
    LIBS
        kde2::artsflow
    VERSION
        3.0.0
    )


kde2_library(libartsc
    SOURCES
        artsc.c
    HEADERS
        artsc.h
    LIBS
        kde2::artsflow
    VERSION
        3.0.0
    )

kde2_library(libartsdsp
    SOURCES
        artsdsp.c
    LIBS
        kde2::artsc
    VERSION
        3.0.0
    )


kde2_library(libartscbackend
    SOURCES
        artscbackend.cc
    LIBS
        kde2::artsflow
        kde2::soundserver
    VERSION
        3.0.0
    )

# TODO: this apparently doesn't work

#message(STATUS $<TARGET_FILE_NAME:libartscbackend>)
target_compile_definitions(libartsdsp_st
    PRIVATE
    -DARTSC_BACKEND="${CMAKE_INSTALL_FULL_LIBDIR}/$<TARGET_FILE_NAME:libartscbackend>"
    )

target_compile_definitions(libartsc
    PRIVATE
    -DARTSC_BACKEND="${CMAKE_INSTALL_FULL_LIBDIR}/$<TARGET_FILE_NAME:libartscbackend>"
    )
