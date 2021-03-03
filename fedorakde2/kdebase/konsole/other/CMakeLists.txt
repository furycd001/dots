kde2_stdicon()

install(
    FILES
        Example.Schema
        default.Schema
        BlackOnLightYellow.schema
        BlackOnWhite.schema
        DarkPicture.schema
        GreenOnBlack.schema
        GreenTint.schema
        GreenTint_MC.schema
        LightPicture.schema
        Linux.schema
        README.KeyTab
        README.Schema
        Transparent.schema
        Transparent_MC.schema
        Transparent_darkbg.schema
        Transparent_lightbg.schema
        WhiteOnBlack.schema
        XTerm.schema
        default.Keytab
        linux.desktop
        mc.desktop
        shell.desktop
        su.desktop
        sumc.desktop
        syscolor.schema
        vim.schema
        vt100.keytab
        vt420pc.keytab
        x11r5.keytab
    DESTINATION "${KDE2_DATADIR}/konsole"
    )

# Conflicts with current/normal konsole
install(
    FILES linux.keytab
    DESTINATION "${KDE2_DATADIR}/konsole"
    RENAME kde2-linux.keytab
    )
