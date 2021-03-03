#include <kpixmapeffect.h>
#include <qpainter.h>
#include <qapplication.h>

#include <taskmanager.h>

#include "kastaskitem.h"
#include "kastaskpopup.h"
#include "kastasker.h"

const int TITLE_HEIGHT = 13;

KasTaskPopup::KasTaskPopup( KasTaskItem *item, const char *name )
    : KasPopup( item, name )
{
    this->item = item;

    setFont(QFont("Helvetica", 10));
    setMouseTracking( true );

    QString text = item->task()->visibleName();
    if ( item->kasbar()->thumbnailsEnabled() && item->task()->hasThumbnail() ) {
        resize( item->task()->thumbnail().width() + 2, 
		TITLE_HEIGHT + item->task()->thumbnail().height() + 2 );
        titleBg.resize( width(), TITLE_HEIGHT );
    }
    else {
        int w = fontMetrics().width( text ) + 4;
        int h = TITLE_HEIGHT + 1;
        resize( w, h );
        titleBg.resize( w, h );
    }

    KPixmapEffect::gradient( titleBg, 
                             Qt::black, colorGroup().mid(),
                             KPixmapEffect::DiagonalGradient );

    connect( item->task(), SIGNAL( thumbnailChanged() ), SLOT( refresh() ) );
}

KasTaskPopup::~KasTaskPopup()
{
}

void KasTaskPopup::refresh()
{
    QString text = item->task()->visibleName();
    if (  item->kasbar()->thumbnailsEnabled() && item->task()->hasThumbnail() ) {
        resize( item->task()->thumbnail().width() + 2,
		TITLE_HEIGHT + item->task()->thumbnail().height() + 2 );
        titleBg.resize( width(), TITLE_HEIGHT );
    }
    update();
}

void KasTaskPopup::paintEvent( QPaintEvent * )
{
    QPainter p( this );
    p.drawPixmap( 0, 0, titleBg );

    QString text = item->task()->visibleName();

    p.setPen( Qt::white );
    if ( fontMetrics().width( text ) > width() - 4 )
        p.drawText( 1, 1, width() - 4, TITLE_HEIGHT - 1, AlignLeft | AlignVCenter,
                     text );
    else
        p.drawText( 1, 1, width() - 4, TITLE_HEIGHT - 1, AlignCenter, text );

    QPixmap thumb = item->task()->thumbnail();
    if ( !thumb.isNull() )
        p.drawPixmap( 1, TITLE_HEIGHT, thumb );

    //
    // Draw border
    //
    p.setPen( Qt::black );
    p.drawRect( 0, 0, width(), height() );
}

#include "kastaskpopup.moc"
