qt2_wrap_cpp(kstart_MOC SOURCES kstart.h)
add_executable(kstart2 kstart.C ${kstart_MOC})
target_link_libraries(kstart2 kde2::kdecore)
install(TARGETS kstart2 RUNTIME DESTINATION ${KDE2_BINDIR})

