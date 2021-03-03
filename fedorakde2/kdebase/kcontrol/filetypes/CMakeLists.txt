qt2_wrap_cpp(kcm_filetypes_MOC
    SOURCES
        filegroupdetails.h
        filetypedetails.h
        filetypesview.h
        keditfiletype.h
        kservicelistwidget.h
        kserviceselectdlg.h
)

kde2_library(filetypes
    SOURCES
	filetypedetails.cpp filegroupdetails.cpp
	kservicelistwidget.cpp typeslistitem.cpp newtypedlg.cpp
	kserviceselectdlg.cpp
        ${kcm_filetypes_MOC}
    LIBS
        kde2::kdeui
        kde2::ksycoca
        kde2::kfile
    )

kde2_module(libkcm_filetypes SOURCES filetypesview.cpp LIBS kde2::filetypes)
kde2_kinit_executable(keditfiletype SOURCES keditfiletype.cpp LIBS kde2::filetypes)

install(FILES filetypes.desktop DESTINATION "${KDE2_APPSDIR}/Settings/FileBrowsing")
