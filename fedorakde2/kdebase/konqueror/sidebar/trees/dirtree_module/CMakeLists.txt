set(sidebar_tree_SRCS
        dirtree_module.cpp dirtree_item.cpp
    )

qt2_wrap_cpp(sidebar_tree_SRCS
    SOURCES
        dirtree_module.h
    )

kde2_module(libkonqsidebartree_dirtree
    SOURCES
        ${sidebar_tree_SRCS}
    LIBS
        kde2::konqsidebar_tree
    )

