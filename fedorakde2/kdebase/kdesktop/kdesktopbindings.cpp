#ifdef WITH_LABELS
 keys->insertItem(i18n("Desktop"), "Program:kdesktop", 0);
#endif
 keys->insertItem(i18n("Execute Command"),  "Execute command", KKey("ALT+F2"), KKey("Meta+Return"));
 keys->insertItem(i18n("Show Taskmanager"), "Show taskmanager", KKey("CTRL+Escape"), KKey("Meta+Ctrl+Pause"));
 keys->insertItem(i18n("Show Window List"), "Show window list", KKey("ALT+F5"), KKey("Meta+0"));
 keys->insertItem(i18n("Lock Screen"),"Lock screen", KKey("CTRL+ALT+L"), KKey("Meta+ScrollLock"));
 keys->insertItem(i18n("Logout"),"Logout", KKey("CTRL+ALT+Delete"), KKey("Meta+Escape"));
 keys->insertItem(i18n("Logout without Confirmation"),"Logout without Confirmation", KKey("CTRL+ALT+Shift+Delete"), KKey("Meta+Shift+Escape"));
