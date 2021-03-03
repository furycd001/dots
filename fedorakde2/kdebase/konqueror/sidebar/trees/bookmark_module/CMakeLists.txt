set(sidebar_bookmark_SRCS
        bookmark_module.cpp bookmark_item.cpp
    )

qt2_wrap_cpp(sidebar_bookmark_SRCS
    SOURCES
        bookmark_module.h
    )

kde2_module(libkonqsidebartree_bookmarks
    SOURCES
        ${sidebar_bookmark_SRCS}
    LIBS
        kde2::konqsidebar_tree
    )



