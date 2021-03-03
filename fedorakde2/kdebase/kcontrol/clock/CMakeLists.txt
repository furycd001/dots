qt2_wrap_cpp(kcm_clock_MOC SOURCES dtime.h main.h tzone.h)
kde2_module(libkcm_clock SOURCES dtime.cpp tzone.cpp main.cpp  ${kcm_clock_MOC} LIBS kde2::kdeui)
install(FILES clock.desktop DESTINATION "${KDE2_APPSDIR}/Settings/System")
