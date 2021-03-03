add_subdirectory(kdesavers)
add_subdirectory(kxsconfig)
add_subdirectory(kpartsaver)

add_executable(krandom.kss random.cpp)
target_link_libraries(krandom.kss kde2::kdeui)
get_target_property(KSS_INCLUDE_PATH kde2::kscreensaver INTERFACE_INCLUDE_DIRECTORIES)
target_include_directories(krandom.kss PRIVATE ${KSS_INCLUDE_PATH})
install(TARGETS krandom.kss RUNTIME DESTINATION ${KDE2_BINDIR})
