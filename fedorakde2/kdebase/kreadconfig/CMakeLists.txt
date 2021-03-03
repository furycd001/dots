add_executable(kreadconfig kreadconfig.cpp)
target_link_libraries(kreadconfig kde2::kdecore)
install(TARGETS kreadconfig)

