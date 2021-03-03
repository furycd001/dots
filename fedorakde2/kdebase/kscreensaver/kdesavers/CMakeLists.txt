set(kbanner_SRCS banner.cpp)
qt2_wrap_cpp(kbanner_SRCS SOURCES banner.h)
add_executable(kbanner2.kss ${kbanner_SRCS})
target_link_libraries(kbanner2.kss kde2::kscreensaver)

set(kblankscreen_SRCS blankscrn.cpp)
qt2_wrap_cpp(kblankscreen_SRCS SOURCES blankscrn.h)
add_executable(kblankscreen.kss ${kblankscreen_SRCS})
target_link_libraries(kblankscreen.kss kde2::kscreensaver)

set(kblob_SRCS blob.cpp)
qt2_wrap_cpp(kblob_SRCS SOURCES blob.h)
add_executable(kblob2.kss ${kblob_SRCS})
target_link_libraries(kblob2.kss kde2::kscreensaver)

set(klines_SRCS lines.cpp)
qt2_wrap_cpp(klines_SRCS SOURCES lines.h)
add_executable(klines2.kss ${klines_SRCS})
target_link_libraries(klines2.kss kde2::kscreensaver)

set(klorenz_SRCS lorenz.cpp)
qt2_wrap_cpp(klorenz_SRCS SOURCES lorenz.h)
add_executable(klorenz.kss ${klorenz_SRCS})
target_link_libraries(klorenz.kss kde2::kscreensaver)

set(kmatrix_SRCS matrix.cpp)
qt2_wrap_cpp(kmatrix_SRCS SOURCES matrix.h)
add_executable(kmatrix2.kss ${kmatrix_SRCS})
target_link_libraries(kmatrix2.kss kde2::kscreensaver)

set(kpolygon_SRCS polygon.cpp)
qt2_wrap_cpp(kpolygon_SRCS SOURCES polygon.h)
add_executable(kpolygon2.kss ${kpolygon_SRCS})
target_link_libraries(kpolygon2.kss kde2::kscreensaver)

set(kscience_SRCS science.cpp)
qt2_wrap_cpp(kscience_SRCS SOURCES science.h)
add_executable(kscience2.kss ${kscience_SRCS})
target_link_libraries(kscience2.kss kde2::kscreensaver)

set(kslideshow_SRCS slideshow.cpp)
qt2_wrap_cpp(kslideshow_SRCS SOURCES slideshow.h)
add_executable(kslideshow.kss ${kslideshow_SRCS})
target_link_libraries(kslideshow.kss kde2::kscreensaver kde2::kfile)

set(kvm_SRCS kvm.cpp vm.c vm_random.c)
qt2_wrap_cpp(kvm_SRCS SOURCES kvm.h)
add_executable(kvm.kss ${kvm_SRCS})
target_link_libraries(kvm.kss kde2::kscreensaver)


install(TARGETS
    kbanner2.kss
    kblankscreen.kss
    kblob2.kss
    klines2.kss
    klorenz.kss
    kmatrix2.kss
    kpolygon2.kss
    kscience2.kss
    kslideshow.kss
    kvm.kss

    RUNTIME DESTINATION ${KDE2_BINDIR}
)

install(FILES
        KBanner.desktop
        KBlankscreen.desktop
        KPolygon.desktop
        KSlideshow.desktop
        KLines.desktop
        KLorenz.desktop
        KBlob.desktop
        KVm.desktop
        KMatrix.desktop
        KScience.desktop
    DESTINATION "${KDE2_APPSDIR}/System/ScreenSavers"
)

install(FILES
        kscience.png
    DESTINATION "${KDE2_DATADIR}/kscreensaver"
)
