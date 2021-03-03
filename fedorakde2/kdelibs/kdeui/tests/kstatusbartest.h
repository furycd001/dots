#ifndef test_kstatusbar_h
#define test_kstatusbar_h

#include <kmenubar.h>
#include <qpopupmenu.h>
#include <kstatusbar.h>
#include <kmainwindow.h>

class QMultiLineEdit;

class testWindow  : public KMainWindow
{
    Q_OBJECT

public:
    testWindow (QWidget *parent=0, const char *name=0);
    ~testWindow ();
    
public slots:
    void slotPress(int i);
    void slotClick(int i);
    void slotMenu(int i);
    
protected:
    QPopupMenu *fileMenu;
    QPopupMenu *smenu;
    KMenuBar *menuBar;
    KStatusBar *statusbar;
    bool insert;
    QMultiLineEdit *widget;
};
#endif

