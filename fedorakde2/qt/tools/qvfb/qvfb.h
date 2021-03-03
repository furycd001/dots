/****************************************************************************
**
** Qt/Embedded virtual framebuffer
**
** Created : 20000605
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of the Qt GUI Toolkit.
**
** Licensees holding valid Qt Professional Edition licenses may use this
** file in accordance with the Qt Professional Edition License Agreement
** provided with the Qt Professional Edition.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
** information about the Professional Edition licensing.
**
*****************************************************************************/

#include <qmainwindow.h>

class QVFbView;
class QVFbRateDialog;
class QPopupMenu;
class QMenuData;
class QFileDialog;
class Config;

class QVFb: public QMainWindow
{
    Q_OBJECT
public:
    QVFb( int display_id, int w, int h, int d, const QString &skin, QWidget *parent = 0,
		const char *name = 0, uint wflags = 0 );
    ~QVFb();

    void enableCursor( bool e );
    void popupMenu();

    QSize sizeHint() const;

protected slots:
    void saveImage();
    void toggleAnimation();
    void toggleCursor();
    void changeRate();
    void about();

    void configure();

    void setZoom(double);
    void setZoom1();
    void setZoom2();
    void setZoom3();
    void setZoom4();
    void setZoomHalf();

protected:
    void createMenu(QMenuData *menu);

private:
    void init( int display_id, int w, int h, int d, const QString &skin );
    QVFbView *view;
    QVFbRateDialog *rateDlg;
    QFileDialog* imagesave;
    QPopupMenu *viewMenu;
    int cursorId;
    Config* config;
    QString currentSkin;

private slots:
    void setGamma400(int n);
    void setR400(int n);
    void setG400(int n);
    void setB400(int n);
    void updateGammaLabels();
};

