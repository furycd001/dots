set(listview_SRCS
    konq_listview.cc konq_listviewitems.cc konq_listviewwidget.cc
    konq_treeviewwidget.cc konq_textviewwidget.cc konq_textviewitem.cc
    konq_treeviewitem.cc
    )

qt2_wrap_cpp(listview_SRCS
    SOURCES
        konq_listview.h
        konq_listviewwidget.h
        konq_treeviewwidget.h
        konq_textviewwidget.h
    )

kde2_module(libkonqlistview
    SOURCES
        ${listview_SRCS}
    LIBS
        kde2::konq
    )

install(FILES konq_treeview.desktop konq_detailedlistview.desktop konq_textview.desktop DESTINATION ${KDE2_SERVICESDIR})
install(FILES konq_treeview.rc konq_detailedlistview.rc konq_textview.rc
    DESTINATION ${KDE2_DATADIR}/konqlistview)
