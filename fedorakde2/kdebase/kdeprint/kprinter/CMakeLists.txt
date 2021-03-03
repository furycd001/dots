qt2_wrap_cpp(kprinter_MOC SOURCES printwrapper.h)
add_executable(kprinter main.cpp printwrapper.cpp ${kprinter_MOC})
target_link_libraries(kprinter kde2::kdeprint)
install(TARGETS kprinter)

