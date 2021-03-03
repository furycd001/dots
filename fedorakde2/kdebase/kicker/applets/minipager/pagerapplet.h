/*****************************************************************

Copyright (c) 1996-2000 the kicker authors. See file AUTHORS.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************/

#ifndef __MINIPAGER_H
#define __MINIPAGER_H

#include <qvaluelist.h>
#include <qbutton.h>
#include <qlayout.h>

#include <kpanelapplet.h>
#include <kwin.h>

class KProcess;
class KWinModule;
class KMiniPager;
class QLineEdit;
class KArrowButton;

class KMiniPagerButton : public QButton
{
    Q_OBJECT

public:
    KMiniPagerButton(int desk, KMiniPager *parent=0, const char *name=0);

    void setOn( bool b ) { QButton::setOn( b ); }

signals:
    void buttonSelected(int desk );
    void showMenu( const QPoint&, int );

protected:
    void paintEvent(QPaintEvent *ev);
    void resizeEvent(QResizeEvent *ev);
    void mousePressEvent( QMouseEvent * );
    void dragEnterEvent( QDragEnterEvent* e );
    void dragLeaveEvent( QDragLeaveEvent* e );
    int deskNum;

    bool eventFilter( QObject *, QEvent * );

private slots:
    void slotToggled(bool);
    void slotClicked();
    void slotDragSwitch();

private:
    KMiniPager* pager();
    QLineEdit* lineedit;
    QTimer dragSwitchTimer;
};

class QDesktopPreviewFrame : public QFrame
{
    Q_OBJECT

public:
    QDesktopPreviewFrame(KMiniPager *pPager);

protected:
    void keyPressEvent(QKeyEvent *evt);

private:
    KMiniPager *m_pPager;
};

class KMiniPager : public KPanelApplet
{
    Q_OBJECT

public:
    KMiniPager(const QString& configFile, Type t = Normal, int actions = 0,
               QWidget *parent = 0, const char *name = 0);

    virtual ~KMiniPager();

    int widthForHeight(int height) const;
    int heightForWidth(int width) const;

    KWin::Info* info( WId win );
    KWinModule* kwin() { return kwin_module; }

    enum Mode { Preview = 14, Number, Name };
    Mode mode() const { return m; }

    Orientation orientation() const { return KPanelApplet::orientation(); }


    void preferences();

    void emitRequestFocus(){ emit requestFocus(); }

    void hideDesktopPreview();
protected:

    void createDesktopPreview();
    void destroyDesktopPreview();
    QSize calculateDesktopPreviewFrameSize() const;

public slots:
    void slotSetDesktop(int desktop);
    void slotSetDesktopCount(int count);
    void slotButtonSelected(int desk );
    void slotActiveWindowChanged( WId win );
    void slotWindowAdded( WId );
    void slotWindowRemoved( WId );
    void slotWindowChanged( WId, unsigned int );
    void slotStackingOrderChanged();
    void slotShowMenu( const QPoint&, int );
    void slotDesktopNamesChanged();
    void slotRefresh();

protected:
    void allocateButtons();
    void popupDirectionChange( Direction d );
    void resizeEvent(QResizeEvent*);

protected slots:
    void desktopPreviewProcessExited(KProcess *p);
    void desktopPreview();

private:

    QValueList<KMiniPagerButton*>btnList;
    QGridLayout *layout;
    int curDesk;
    WId active;
    QIntDict<KWin::Info> windows;
    KWinModule* kwin_module;
    Mode m;

    bool bShowDesktopPreviewButton;
    KArrowButton *m_pBtnDesktopPreview;
    QFrame *m_pDesktopPreviewFrame; // a parent frame of virtual dsktop preview
    QBoxLayout *m_pBoxLayout;
    KProcess *m_pPagerProcess;
};

#endif

