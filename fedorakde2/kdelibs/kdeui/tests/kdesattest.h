//
// Simple little hack to show off blending effects.
//
// (C) KDE Artistic Cristian Tibirna <tibirna@kde.org>
//

#ifndef __KBLEND_TEST_H
#define __KBLEND_TEST_H

#include <qwidget.h>
#include <qimage.h>
#include <knuminput.h>

class KDesatWidget : public QWidget
{
Q_OBJECT
public:
    KDesatWidget(QWidget *parent=0, const char *name=0);

public slots:
    void change(double);

protected:
    void paintEvent(QPaintEvent *ev);
private:
    float desat_value;
    QImage image;
    KDoubleNumInput *slide;
};

#endif
