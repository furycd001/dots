#ifndef KSPLASH_H
#define KSPLASH_H

#include <config.h>
#include <qvbox.h>
#include <qlabel.h>
#include <dcopobject.h>

class KProgress;
class QTimer;
class QPixmap;
class QLabel;

class KSplash : public QVBox, public DCOPObject
{
    Q_OBJECT
    K_DCOP

public:
    KSplash( const char* name );
    ~KSplash() {}

k_dcop:
    ASYNC upAndRunning( QString );
    ASYNC setMaxProgress(int);
    ASYNC setProgress(int);

protected slots:
    void tryDcop();
    void autoMode();
    void resizeEvent(QResizeEvent*);
    void blink();

protected:
    void updateState();
    bool eventFilter( QObject *, QEvent * );
    QPixmap makePixmap(int _state);

private:
    KProgress *progress;
    QPixmap *bar_active_pm;
    QPixmap *bar_inactive_pm;
    QPixmap *bar_blink1;
    QPixmap *bar_blink2;
    QLabel  *top_label;
    QLabel  *bar_label;
    QLabel  *bottom_label;
    QLabel  *status_label;
    QTimer* close_timer;
    QTimer* blink_timer;
    int state;
    bool testmode;
};


#endif
