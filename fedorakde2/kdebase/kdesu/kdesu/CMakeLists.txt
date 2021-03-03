qt2_wrap_cpp(kdesu_MOC SOURCES sudlg.h)
add_executable(kdesu2 kdesu.cpp sudlg.cpp ${kdesu_MOC})
target_link_libraries(kdesu2 kde2::kdeui kde2::kdesu)
install(TARGETS kdesu2)


