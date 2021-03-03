/*****************************************************************

Copyright (c) 2001 the kicker authors. See file AUTHORS.

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

#ifndef __container_panel_h__
#define __container_panel_h__

#include <netwm_def.h>
#include <qframe.h>
#include <qlist.h>

#include "global.h"
#include "containerareabox.h"

class QPopupMenu;
class QTimer;
class QBoxLayout;
class QVBox;
class KArrowButton;
class KConfig;
class PanelOpMenu;
class PopupWidgetFilter;

class PanelSettings
{
public:
    PanelSettings();
    void readConfig( KConfig* c );
    void writeConfig( KConfig* c );

    // Configuration settings
    Position _position;
    int      _HBwidth;
    bool     _showLeftHB;
    bool     _showRightHB;
    bool     _autoHide;
    bool     _autoHideSwitch;
    int      _autoHideDelay;
    bool     _hideAnim;
    bool     _autoHideAnim;
    int      _hideAnimSpeed;
    int      _autoHideAnimSpeed;
    bool     _showToolTips;
    int      _sizePercentage;
    bool     _expandSize;
};

class PanelContainer : public QFrame
{
    Q_OBJECT

public:
    PanelContainer(QWidget *parent, const char *name);
    virtual ~PanelContainer();

    Position position() const { return _settings._position; }
    void setPosition(Position p);

    enum UserHidden{ Unhidden, LeftTop, RightBottom };
    QRect initialGeometry( Position p, bool autoHidden = false, UserHidden userHidden = Unhidden );

    virtual QSize sizeHint( Position p, QSize maxSize );
    bool eventFilter( QObject *, QEvent * );

    virtual void readConfig() {};
    virtual void writeConfig() {};
    void readConfig(KConfig* config);
    void writeConfig(KConfig* config);

    static void readContainerConfig();
    static void writeContainerConfig();

signals:
    void positionChange( Position pos );

protected:
    virtual PanelSettings defaultSettings();
    QVBox* containerAreaBox() { return _containerAreaBox; }
    bool autoHidden() const { return _autoHidden; };
    UserHidden userHidden() const { return _userHidden; };
    Orientation orientation() const;
    virtual void resetLayout();
    virtual void autoHide(bool hide);
    QPoint getPopupPosition(QPopupMenu *menu, QPoint eventpos);
    virtual QRect workArea();

    static QList<PanelContainer> _containers;

protected slots:
    void moveMe();
    void showPanelMenu( QPoint pos );
    void updateLayout();
    void showScrollButtons(bool);
    virtual void scrollLeftUp() {};
    virtual void scrollRightDown() {};

private slots:
    void autoHideTimeout();
    void hideLeft();
    void hideRight();
    void animatedHide(bool left);
    void updateWindowManager();
    void currentDesktopChanged(int);
    void strutChanged();
    void blockUserInput( bool block );
    void maybeStartAutoHideTimer();
    void stopAutoHideTimer();

private:
    QSize initialSize( Position p );
    QPoint initialLocation( Position p, QSize s, bool autohidden = false, UserHidden userHidden = Unhidden );

    PanelSettings    _settings;

    // State variables
    bool             _autoHidden;
    UserHidden       _userHidden;
    bool             _block_user_input;
    bool             _faked_activation;
    QPoint           _last_lmb_press;
    bool             _in_autohide;

    // Misc objects
    PanelOpMenu      *_opMnu;
    QTimer           *_autohideTimer;
    NETStrut         _strut;
    PopupWidgetFilter *_popupWidgetFilter;

    // Widgets
    ContainerAreaBox *_containerAreaBox;
    KArrowButton     *_ltHB; // Left Hide Button
    KArrowButton     *_rbHB; // Right Hide Button
    KArrowButton     *_luSB; // Left Scroll Button
    KArrowButton     *_rdSB; // Right Scroll Button
    QBoxLayout       *_layout;

};

class PopupWidgetFilter : public QObject
{
    Q_OBJECT

public:
    PopupWidgetFilter( QObject *parent );
    ~PopupWidgetFilter() {};
    bool eventFilter( QObject *obj, QEvent* e );
signals:
    void popupWidgetHiding();
};

#endif
