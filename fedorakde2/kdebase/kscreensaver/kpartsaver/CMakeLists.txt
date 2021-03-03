set(kpartsaver_SRCS kpartsaver.cpp)

qt2_wrap_ui(kpartsaver_SRCS SOURCES configwidget.ui)
qt2_wrap_cpp(kpartsaver_SRCS SOURCES kpartsaver.h)

add_executable(kpartsaver ${kpartsaver_SRCS})
target_link_libraries(kpartsaver kde2::kscreensaver kde2::kfile)

install(FILES
        kpartsaver.desktop
    DESTINATION "${KDE2_APPSDIR}/System/ScreenSavers"
)

install(TARGETS kpartsaver RUNTIME DESTINATION ${KDE2_BINDIR})
