/* -------------------------------------------------------------

   toplevel.h (part of Klipper - Cut & paste history for KDE)

   (C) by Andrew Stanley-Jones

   Generated with the KDE Application Generator

 ------------------------------------------------------------- */


#ifndef _TOPLEVEL_H_
#define _TOPLEVEL_H_

#include <kapp.h>
#include <kglobalaccel.h>
#include <kmainwindow.h>
#include <kpopupmenu.h>
#include <qintdict.h>
#include <qtimer.h>
#include <qpixmap.h>

class QClipboard;
class KToggleAction;
class URLGrabber;

class TopLevel : public KMainWindow
{
  Q_OBJECT

public:
    TopLevel();
    ~TopLevel();

    KGlobalAccel *globalKeys;

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void readProperties(KConfig *);
    void readConfiguration(KConfig *);
    void writeConfiguration(KConfig *);

protected slots:
    void slotPopupMenu() { showPopupMenu( pQPMmenu ); }
    void showPopupMenu( QPopupMenu * );
    void saveProperties();
    void slotRepeatAction();
    void setURLGrabberEnabled( bool );
    void toggleURLGrabber();

private slots:
    void newClipData();
    void clickedMenu(int);
    void slotConfigure();

private:
    QClipboard *clip;

    QString QSlast;
    KPopupMenu *pQPMmenu;
    KToggleAction *toggleURLGrabAction;
    QIntDict<QString> *pQIDclipData;
    QTimer *pQTcheck;
    QPixmap *pQPpic;
    bool bPopupAtMouse, bClipEmpty, bKeepContents, bURLGrabber, bReplayActionInHistory;
    QString QSempty;
    URLGrabber *myURLGrabber;
    int pSelectedItem;
    int maxClipItems;
    int URLGrabItem;

    void trimClipHistory(int);
};

#endif
