#ifndef __blah__h__
#define __blah__h__

#include <qobject.h>
#include <stdio.h>

class MyObject: public QObject
{
    Q_OBJECT
public:
    MyObject();

public slots:
    void slotPaletteChanged() { printf("SIGNAL: Palette changed\n"); }
    void slotStyleChanged() { printf("SIGNAL: Style changed\n"); }
    void slotFontChanged() { printf("SIGNAL: Font changed\n"); }
    void slotBackgroundChanged(int i) { printf("SIGNAL: Background %d changed\n", i); }
    void slotAppearanceChanged() { printf("SIGNAL: Appearance changed\n"); }
    void slotMessage(int id, int arg) { printf("SIGNAL: user message: %d,%d\n", id, arg); }
};

#endif
