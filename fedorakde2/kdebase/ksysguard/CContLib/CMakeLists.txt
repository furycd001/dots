
kde2_library(ccont
    SOURCES
        ccont.c
        dll.c
        pga.c
        sll.c
    HEADERS
        ccont.h
    )

add_executable(conntest
    conttest.c
    )
target_link_libraries(conntest kde2::ccont)


