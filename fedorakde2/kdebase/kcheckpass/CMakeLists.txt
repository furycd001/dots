set(kcheckpass_SRCS
    kcheckpass.c
    checkpass_etcpasswd.c
    checkpass_pam.c
    checkpass_shadow.c
    checkpass_osfc2passwd.c
    checkpass_aix.c
)

# TODO: fix build with anything other than pam (see the Makefile.am)

add_executable(kcheckpass ${kcheckpass_SRCS})
target_link_libraries(kcheckpass
    kde2::kdefakes
    ${PAM_LIBRARY}
    )
target_include_directories(kcheckpass PRIVATE ${PAM_INCLUDE_DIR})
install(TARGETS kcheckpass RUNTIME DESTINATION ${KDE2_BINDIR})
