add_executable(kdesud kdesud.cpp repo.cpp lexer.cpp handler.cpp secure.cpp)
target_link_libraries(kdesud kde2::kdecore kde2::kdesu)
install(TARGETS kdesud)

