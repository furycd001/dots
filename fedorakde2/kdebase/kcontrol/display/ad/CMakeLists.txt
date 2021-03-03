install(
    FILES
        AAAAAAGeneral.ad AAAMotif.ad AAATk.ad AAAXaw.ad AcroRead.ad
        Editres.ad Emacs.ad GV.ad ML.ad Nedit.ad Netscape.ad RVPlayer.ad
        WPerfect.ad XCalc.ad Xawtv.ad Xdvi.ad XTerm.ad XOsview.ad XV.ad Xpdf.ad
    DESTINATION
        "${KDE2_DATADIR}/kdisplay2/app-defaults"
    )

install(FILES krdb DESTINATION ${KDE2_BINDIR} RENAME krdb2)
