#ifdef WITH_LABELS
 keys->insertItem(i18n("Clipboard"), "Program:klipper", 0);
#endif
    keys->insertItem(i18n("Show klipper popupmenu"),
                                "show-klipper-popupmenu", KKey("CTRL+ALT+V"), KKey("Meta+Ctrl+V"));
    keys->insertItem(i18n("Manually invoke action on current clipboard"),
                                "repeat-last-klipper-action", KKey("CTRL+ALT+R"), KKey("Meta+Ctrl+R"));
    keys->insertItem(i18n("Enable/disable clipboard actions"),
                           "toggle-clipboard-actions", KKey("CTRL+ALT+X"), KKey("Meta+Ctrl+X"));
