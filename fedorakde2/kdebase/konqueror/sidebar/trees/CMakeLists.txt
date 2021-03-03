set(sidebar_tree_SRCS
        konq_sidebartree.cpp
        konq_sidebartreeitem.cpp
        konq_sidebartreetoplevelitem.cpp
        konqsidebar_tree.cpp
    )

qt2_wrap_cpp(sidebar_tree_SRCS
    SOURCES
        konq_sidebartree.h
        konqsidebar_tree.h
    )

kde2_library(libkonqsidebar_tree
    SOURCES
        ${sidebar_tree_SRCS}
    LIBS
        kde2::konqsidebarplugin
    )

include(GNUInstallDirs)
get_target_property(LAPROP_SONAME libkonqsidebar_tree OUTPUT_NAME)
set(LAPROP_SONAME "lib${LAPROP_SONAME}")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/libkonqsidebar_tree.la.in ${CMAKE_CURRENT_BINARY_DIR}/libkonqsidebar_tree.la @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libkonqsidebar_tree.la DESTINATION "${CMAKE_INSTALL_LIBDIR}/kde2")

add_subdirectory(dirtree_module)
add_subdirectory(history_module)
add_subdirectory(bookmark_module)
add_subdirectory(init)
