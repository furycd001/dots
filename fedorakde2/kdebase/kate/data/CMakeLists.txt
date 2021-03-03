install(FILES kate.desktop kwrite.desktop DESTINATION ${KDE2_APPSDIR}/Editors)
install(FILES kateui.rc katepartui.rc katepartbrowserui.rc DESTINATION ${KDE2_DATADIR}/kate)

install(
    FILES
        language.dtd cpp.xml c.xml html.xml kbasic.xml objectivec.xml changelog.xml
        ada.xml css.xml perl.xml php.xml xml.xml java.xml rpmspec.xml sql.xml vhdl.xml
        diff.xml bash.xml latex.xml postscript.xml ruby.xml desktop.xml syntax.template
    DESTINATION
        ${KDE2_DATADIR}/kate/syntax
    )

install(FILES kwriteui.rc DESTINATION ${KDE2_DATADIR}/kwrite)
install(FILES kate_servicetype.desktop DESTINATION ${KDE2_SERVICETYPESDIR})
install(FILES kate_component.desktop kwrite_component.desktop DESTINATION ${KDE2_SERVICESDIR})
