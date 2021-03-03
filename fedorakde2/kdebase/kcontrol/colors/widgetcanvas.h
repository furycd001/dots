//
// A special widget which draws a sample of KDE widgets
// It is used to preview color schemes
//
// Copyright (c)  Mark Donohoe 1998
//

#ifndef __WIDGETCANVAS_H__
#define __WIDGETCANVAS_H__

#include <qpixmap.h>
#include <qdrawutil.h>
#include <qcolor.h>
#include <qpainter.h>
#include <qscrollbar.h>
#include <qevent.h>
#include <qmap.h>

#include <kapp.h>
#include <klocale.h>
#include <kcharsets.h>
#include <kpixmap.h>

#define MAX_HOTSPOTS   28
#define SCROLLBAR_SIZE 16

// These defines define the order of the colors in the combo box.
#define CSM_Standard_background		0
#define CSM_Standard_text		1
#define CSM_Select_background		2
#define CSM_Select_text			3
#define CSM_Link			4
#define CSM_Followed_Link		5
#define CSM_Background			6
#define CSM_Text			7
#define CSM_Button_background		8
#define CSM_Button_text			9
#define CSM_Active_title_bar		10
#define CSM_Active_title_text		11
#define CSM_Active_title_blend		12
#define CSM_Active_title_button		13
#define CSM_Inactive_title_bar		14
#define CSM_Inactive_title_text		15
#define CSM_Inactive_title_blend	16
#define CSM_Inactive_title_button	17
#define CSM_Alternate_background    18
#define CSM_LAST			19


class HotSpot
{
public:
    HotSpot() {}
    HotSpot( const QRect &r, int num )
	: rect(r), number(num) {}

    QRect rect;
    int number;
};

class WidgetCanvas : public QWidget
{
    Q_OBJECT

public:
    WidgetCanvas( QWidget *parent=0, const char *name=0 );
    void drawSampleWidgets();
    void resetTitlebarPixmaps(const QColor &active,
			      const QColor &inactive);
    void addToolTip( int area, const QString & );
    QPixmap smplw;
    
    QColor iaTitle;
    QColor iaTxt;
    QColor iaBlend;
    QColor aTitle;
    QColor aTxt;
    QColor aBlend;
    QColor back;
    QColor txt;
    QColor select;
    QColor selectTxt;
    QColor window;
    QColor windowTxt;
    QColor button;
    QColor buttonTxt;
    QColor aTitleBtn;
    QColor iTitleBtn;
    QColor link;
    QColor visitedLink;
	QColor alternateBackground;

    int contrast;

signals:
    void widgetSelected( int );
    void colorDropped( int, const QColor&);
	
protected:
    virtual void paintEvent( QPaintEvent * );
    virtual void mousePressEvent( QMouseEvent * );
    virtual void mouseMoveEvent( QMouseEvent * );
    virtual void resizeEvent( QResizeEvent * );
    virtual void showEvent( QShowEvent * );
    virtual void dropEvent( QDropEvent *);
    virtual void dragEnterEvent( QDragEnterEvent *);
    void paletteChange( const QPalette & );

    QMap<int,QString> tips;
    HotSpot hotspots[MAX_HOTSPOTS];
    int currentHotspot;
};

#endif
