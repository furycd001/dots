set(taskbar_SRCS
    lmbmenu.cpp rmbmenu.cpp taskcontainer.cpp taskbar.cpp
    fittslawscrollview.cpp scrollbutton.cpp windowlistbutton.cpp
    )

qt2_wrap_cpp(taskbar_SRCS
    SOURCES
        fittslawscrollview.h
        lmbmenu.h
        rmbmenu.h
        scrollbutton.h
        taskbar.h
        taskcontainer.h
        windowlistbutton.h
    )
kde2_library(libtaskbar
    SOURCES
        ${taskbar_SRCS}
    HEADERS
        fittslawscrollview.h
        lmbmenu.h
        rmbmenu.h
        scrollbutton.h
        taskbar.h
        taskcontainer.h
        windowlistbutton.h
    LIBS
        kde2::taskmanager
   )
