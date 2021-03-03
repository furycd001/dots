qt2_wrap_cpp(kcm_info_MOC
    SOURCES
        memory.h
    )

kde2_module(libkcm_info
    SOURCES
        main.cpp memory.cpp
        ${kcm_info_MOC}
    LIBS
        kde2::kdeui
    )
install(
    FILES
        memory.desktop processor.desktop dma.desktop interrupts.desktop ioports.desktop
        pci.desktop sound.desktop devices.desktop scsi.desktop partitions.desktop
        xserver.desktop
    DESTINATION "${KDE2_APPSDIR}/Settings/Information")
