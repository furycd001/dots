add_executable(artsmessage artsmessage.cc)
target_link_libraries(artsmessage
    Qt::Qt2
    kde2::kdecore
    kde2::kdeui
    )
install(TARGETS artsmessage)
