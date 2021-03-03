/* 
 * $Id: b2client.h,v 1.16.2.1 2001/08/24 11:14:27 gallium Exp $
 *
 * B-II KWin Client
 *
 * Changes:
 * 	Customizable button positions by Karol Szwed <gallium@kde.org>
 */ 

#ifndef __B2CLIENT_H
#define __B2CLIENT_H

#include <qvariant.h>
#include <qbitmap.h>
#include <kpixmap.h>
#include "../../client.h"
#include "../../kwinbutton.h"

class QSpacerItem;
class QHBoxLayout;
class QGridLayout;

namespace KWinInternal {

class B2Button : public KWinInternal::KWinButton
{
    Q_OBJECT
public:
    B2Button(Client *_client=0, QWidget *parent=0, const QString& tip=NULL)
        : KWinButton(parent, 0, tip) 
    { 
        client = _client;
        useMiniIcon = false;
        setFixedSize(16,16); 
    };

    void setBg(const QColor &c){bg = c;}
    void setPixmaps(KPixmap *pix, KPixmap *pixDown, KPixmap *iPix,
                    KPixmap *iPixDown);
    void setPixmaps(int button_id);
    void setToggle(){setToggleType(Toggle);}
    void setActive(bool on){setOn(on);}
    void setUseMiniIcon(){useMiniIcon = true;}
    QSize sizeHint() const;
    QSizePolicy sizePolicy() const;
protected:
    virtual void drawButton(QPainter *p);
    void drawButtonLabel(QPainter *){;}
    
    bool useMiniIcon;
    KPixmap *pNorm, *pDown, *iNorm, *iDown;
    QColor bg; //only use one color (the rest is pixmap) so forget QPalette ;)
    
    void mousePressEvent( QMouseEvent* e );
    void mouseReleaseEvent( QMouseEvent* e );

public:
    int last_button;    
    Client* client;
};

class B2Client;

class B2Titlebar : public QWidget
{
    Q_OBJECT
public:
    B2Titlebar(B2Client *parent);
    ~B2Titlebar(){;}
    bool isFullyObscured() const {return isfullyobscured;}
    void recalcBuffer();
    QSpacerItem *captionSpacer;
protected:
    void paintEvent( QPaintEvent* );
    bool x11Event(XEvent *e);
    void mouseDoubleClickEvent( QMouseEvent * );
    void mousePressEvent( QMouseEvent * );
    void mouseReleaseEvent( QMouseEvent * );
    void mouseMoveEvent(QMouseEvent *);
    void resizeEvent(QResizeEvent *ev);
    
    QString oldTitle;
    KPixmap titleBuffer;
    bool set_x11mask;
    bool isfullyobscured;
    bool shift_move;
    QPoint moveOffset;
    B2Client *client;
};

class B2Client : public KWinInternal::Client
{
    Q_OBJECT
    friend class B2Titlebar;
public:
    B2Client( Workspace *ws, WId w, QWidget *parent=0, const char *name=0 );
    ~B2Client(){;}
    void unobscureTitlebar();
    void titleMoveAbs(int new_ofs);
    void titleMoveRel(int xdiff);
protected:
    void resizeEvent( QResizeEvent* );
    void paintEvent( QPaintEvent* );
    void showEvent( QShowEvent* );
    void windowWrapperShowEvent( QShowEvent* );
    void captionChange( const QString& name );
    void stickyChange(bool on);
    void activeChange(bool on);
    void maximizeChange(bool m);
    void iconChange();
    void doShape();
    MousePosition mousePosition( const QPoint& p ) const;
private slots:
    void menuButtonPressed();
    void slotReset();
    void maxButtonClicked();
private:
    void addButtons(const QString& s, const QString tips[], 
                    B2Titlebar* tb, QHBoxLayout* titleLayout);
    void positionButtons();
    void calcHiddenButtons();
    enum ButtonType{BtnMenu=0, BtnSticky, BtnIconify, BtnMax, BtnClose,
        BtnHelp, BtnCount};
    B2Button* button[BtnCount];
    QGridLayout *g;
    int bar_x_ofs;
    B2Titlebar *titlebar;
    int in_unobs;
};

};

#endif
