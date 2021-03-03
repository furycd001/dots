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

#ifndef __panelbutton_h__
#define __panelbutton_h__

#include "../ui/kickermenumanager.h"
#include "panelbuttonbase.h"
#include "global.h"

class KPropertiesDialog;
class PanelBrowserMenu;
class PanelKMenu;
class PanelRecentMenu;
class PanelKonsoleMenu;
class KWindowListMenu;
class PanelServiceMenu;
class KConfig;
class KBookmarkMenu;
class KBookmarkOwner;
class KActionCollection;

class PanelButton: public PanelButtonBase
{
    Q_OBJECT

public:
    PanelButton(QWidget * parent, const char *name = 0);

    QPoint getPopupPosition(QPopupMenu *menu);

    virtual void saveConfiguration(KConfig* /*config*/,const QString& /*group*/) {}

    virtual void properties() {};
    virtual void configure() {}

signals:
    void requestSave();

protected slots:
    void slotIconChanged(int group);
};

/**
 * Base class for panelbuttons which can popup a menu
 */
class PanelPopupButton : public PanelButton
{
    Q_OBJECT

public:
    PanelPopupButton(QWidget *parent=0, const char *name=0);

    void setPopup(QPopupMenu *);
    QPopupMenu *popup() const;

    bool eventFilter(QObject *, QEvent *);

protected:
    virtual void initPopup() {};

protected slots:
    virtual void slotExecMenu();

private:
    QPopupMenu *_popup;
    bool _pressedDuringPopup;
};

/**
 * Simple URL button (applnks, files, whatever)
 */
class PanelURLButton : public PanelButton
{
    Q_OBJECT

public:
    PanelURLButton(const QString &url, QWidget *parent=0,
		   const char *name=0);

    void saveConfiguration(KConfig* config, const QString& group);

    virtual void properties();
    virtual void configure();

protected slots:
    void slotExec();
    void updateURL();

protected:
    virtual void dropEvent(QDropEvent *);
    virtual void dragEnterEvent(QDragEnterEvent *);
    virtual void resizeEvent(QResizeEvent*);
    virtual void  mouseMoveEvent(QMouseEvent *);
    virtual void  mousePressEvent(QMouseEvent *);
    void setToolTip();

    QString urlStr;
    QPoint last_lmb_press;
    KPropertiesDialog *pDlg;
    bool local;
};

/**
 * Button that contains a Browser directory menu
 */
class PanelBrowserButton : public PanelPopupButton
{
    Q_OBJECT

public:
    PanelBrowserButton(const QString& icon, const QString &startDir, QWidget *parent=0,
		       const char *name=0);
    ~PanelBrowserButton(){;}

    void saveConfiguration(KConfig* config, const QString& group);

    virtual void properties();
    virtual void configure();

protected:
    virtual void initPopup();
    virtual void resizeEvent(QResizeEvent*);

    PanelBrowserMenu *topMenu;
    QString _icon;
};

/**
 * Button that contains a Service menu
 */
class PanelServiceMenuButton : public PanelPopupButton
{
    Q_OBJECT

public:
    PanelServiceMenuButton(const QString &label, const QString& relPath, QWidget *parent=0,
			   const char *name=0);
    ~PanelServiceMenuButton(){;}

    void saveConfiguration(KConfig* config, const QString& group);

    virtual void configure();

protected:
    virtual void initPopup();
    virtual void resizeEvent(QResizeEvent*);

    PanelServiceMenu *topMenu;
};

/**
 * Button that contains the PanelKMenu and client menu manager.
 */
class PanelKButton : public PanelPopupButton
{
    Q_OBJECT

public:
    PanelKButton(QWidget *parent=0, const char *name=0);
    ~PanelKButton();

    virtual void configure();
    virtual void properties();

protected slots:
    void slotAccelActivated();
    void slotExecMenuAt(int x, int y);
    void slotRelease();

protected:
    virtual void initPopup();
    virtual void resizeEvent(QResizeEvent*);

    PanelKMenu *topMenu;
    KickerMenuManager* menuMgr;
};


/**
 * Button that shows the deskop
 */
class PanelDesktopButton : public PanelButton
{
    Q_OBJECT

public:
    PanelDesktopButton(QWidget *parent=0, const char *name=0);

    virtual void configure();

protected slots:
    void slotCurrentDesktopChanged(int);
    void slotWindowChanged(WId w, unsigned int dirty);
    void slotShowDesktop( bool );

protected:
    virtual void dragEnterEvent( QDragEnterEvent *ev );
    virtual void dropEvent( QDropEvent *ev );
    virtual void resizeEvent(QResizeEvent*);

private:
    QValueList<WId>iconifiedList;

};

/**
 * Button that contains a non-KDE application
 */
class PanelExeButton : public PanelButton
{
    Q_OBJECT

public:
    PanelExeButton(const QString &filePath, const QString &icon,
		   const QString &cmdLine, bool inTerm, QWidget *parent=0,
		   const char *name=0);
    PanelExeButton(const QString &configData, QWidget *parent=0,
		   const char *name=0);


    void saveConfiguration(KConfig* config, const QString& group);

    virtual void properties();
    virtual void configure();

protected slots:
    void slotExec();

protected:
    virtual void resizeEvent(QResizeEvent*);
    virtual void dropEvent(QDropEvent *ev);
    virtual void dragEnterEvent(QDragEnterEvent *ev);

    QString pathStr, iconStr, cmdStr;
    bool term;
};

/**
 * Button that contains a windowlist menu
 */
class PanelWindowListButton : public PanelPopupButton
{
    Q_OBJECT

public:
    PanelWindowListButton(QWidget *parent=0, const char *name=0);
    ~PanelWindowListButton();

    virtual void configure();

protected:
    virtual void initPopup();
    virtual void resizeEvent(QResizeEvent*);

    KWindowListMenu *topMenu;
};

/**
 * Button that contains a bookmark menu
 */
class PanelBookmarksButton : public PanelPopupButton
{
    Q_OBJECT

public:
    PanelBookmarksButton(QWidget *parent=0, const char *name=0);
    ~PanelBookmarksButton();

    virtual void configure();

protected:
    virtual void initPopup();
    virtual void resizeEvent(QResizeEvent*);

    QPopupMenu 		       *bookmarkParent;
    KBookmarkMenu              *bookmarkMenu;
    KActionCollection          *actionCollection;
    KBookmarkOwner             *bookmarkOwner;
};

/**
 * Button that contains a recent documents menu
 */
class PanelRecentDocumentsButton : public PanelPopupButton
{
    Q_OBJECT

public:
    PanelRecentDocumentsButton(QWidget *parent=0, const char *name=0);
    ~PanelRecentDocumentsButton();

    virtual void configure();

protected:
    virtual void initPopup();
    virtual void resizeEvent(QResizeEvent*);

    PanelRecentMenu 		*recentMenu;
};

class PanelKonsoleButton : public PanelButton
{
    Q_OBJECT

public:
    PanelKonsoleButton(QWidget *parent=0, const char *name=0);
    ~PanelKonsoleButton();

    virtual void configure();

protected slots:
    void slotStartTimer();
    void slotStopTimer();
    void slotExec();
    void slotDelayedPopup();

protected:
    virtual void resizeEvent(QResizeEvent*);

    PanelKonsoleMenu *konsoleMenu;
    QTimer *menuTimer;
};

#endif // __panelbutton_h__

