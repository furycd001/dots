#ifdef KICKER_ALL_BINDINGS
#define LAUNCH_MENU
#define SHOW_DESKTOP
#endif


#ifdef LAUNCH_MENU
#ifdef WITH_LABELS
keys->insertItem(i18n("Panel"), "Program:kicker", 0);
#endif
keys->insertItem(i18n("Popup Launch Menu"),"Popup Launch Menu", KKey("ALT+F1"), KKey("Meta+Space"));
#endif

#ifdef SHOW_DESKTOP
keys->insertItem(i18n("Toggle showing Desktop"),"Toggle Show Desktop", KKey("CTRL+ALT+D"), KKey("Meta+Ctrl+D"));
#endif
