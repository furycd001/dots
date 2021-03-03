set(sidebar_history_SRCS
    history_module.cpp history_item.cpp
    history_dlg.ui history_settings.cpp
    )

kde2_wrap_kidl(sidebar_history_SRCS
    SOURCES
        history_settings.h
    )

qt2_wrap_cpp(sidebar_history_SRCS
    SOURCES
        history_module.h
        history_settings.h
    )

qt2_wrap_ui(sidebar_history_SRCS
    SOURCES
        history_dlg.ui
    )

kde2_module(libkonqsidebartree_history
    SOURCES
        ${sidebar_history_SRCS}
    LIBS
        kde2::konqsidebar_tree
    )


