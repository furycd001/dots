qt2_wrap_cpp(kcm_spellchecking_MOC SOURCES spellchecking.h)

kde2_module(libkcm_spellchecking SOURCES spellchecking.cpp ${kcm_spellchecking_MOC} LIBS kde2::kspell)
install(FILES spellchecking.desktop DESTINATION "${KDE2_APPSDIR}/Settings/Personalization")
