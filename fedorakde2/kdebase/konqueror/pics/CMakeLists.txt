kde2_stdicon()

install(FILES
    tiles/bluemorning.png
    tiles/canvas.png
    tiles/checker.png
    tiles/cubic.png
    tiles/kde4ever.png
    tiles/kenwimer.png
    tiles/noise.png
    tiles/paper_flieder.png
    tiles/redfiber.png
    tiles/punika_attack.png
    DESTINATION ${KDE2_DATADIR}/konqueror/tiles)

install(FILES
    indicators/indicator_anchor.png
    indicators/indicator_connect.png
    indicators/indicator_noconnect.png
    indicators/indicator_empty.png
    indicators/indicator_viewactive.png
    DESTINATION ${KDE2_DATADIR}/konqueror/pics)

add_subdirectory(actions)
