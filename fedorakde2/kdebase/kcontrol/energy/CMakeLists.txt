qt2_wrap_cpp(kcm_energy_MOC SOURCES energy.h)

kde2_module(libkcm_energy SOURCES energy.cpp ${kcm_energy_MOC} LIBS kde2::kdeui)
install(FILES energy.desktop DESTINATION "${KDE2_APPSDIR}/Settings/PowerControl")

add_subdirectory(pics)
