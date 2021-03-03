kde2_module(kio_thumbnail SOURCES thumbnail.cpp LIBS kde2::kio kde2::ksycoca)

kde2_module(imagethumbnail SOURCES imagecreator.cpp LIBS kde2::ksycoca)
kde2_module(textthumbnail SOURCES textcreator.cpp LIBS kde2::kdecore)
kde2_module(gsthumbnail SOURCES gscreator.cpp LIBS kde2::kdecore)

qt2_wrap_cpp(htmlcreator_MOC SOURCES htmlcreator.h)
kde2_module(htmlthumbnail SOURCES htmlcreator.cpp ${htmlcreator_MOC} LIBS kde2::khtml)

install(
    FILES
        thumbnail.protocol
        imagethumbnail.desktop
        textthumbnail.desktop
        htmlthumbnail.desktop
        gsthumbnail.desktop
    DESTINATION ${KDE2_SERVICESDIR}
    )
install(
    FILES
        thumbcreator.desktop
    DESTINATION ${KDE2_SERVICETYPESDIR}
    )
