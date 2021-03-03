// $Id: contactlistview.cpp,v 1.2 2001/06/30 14:21:17 pfeiffer Exp $

#include <qheader.h>
#include <qiconset.h>
#include <qimage.h>
#include <qdragobject.h>
#include <qcombobox.h>

#include <klocale.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <kconfig.h>
#include <kapp.h>

#include "contactentrylist.h"
#include "kaddressbookview.h"

#include "contactlistview.h"

/////////////////////////////////
// DynamicTip Methods

DynamicTip::DynamicTip( ContactListView *parent )
  : QToolTip( parent )
{
    // no explicit initialization needed
}

void DynamicTip::maybeTip( const QPoint &pos )
{
    if (!parentWidget()->inherits( "ContactListView" ))
	return;
    ContactListView *plv = (ContactListView*)parentWidget();
    if (!plv->tooltips())
      return;
    QPoint posVp = plv->viewport()->pos();

    QListViewItem *lvi = plv->itemAt( pos - posVp );
    if (!lvi)
      return;
    ContactListViewItem *plvi = dynamic_cast< ContactListViewItem* >(lvi);
    if (!plvi)
      return;
    ContactEntryList *cel = plvi->parent()->getKAddressBookView()->contactEntryList();
    ContactEntry *ce = cel->find( plvi->entryKey() );
    if (!ce)
      return;
    QString s;
    QRect r = plv->itemRect( lvi );
    r.moveBy( posVp.x(), posVp.y() );
    if (ce->find( "N" ))
      s += i18n( "Name" ) + ": " + *ce->find( "N" );
    if (ce->find( "ORG" ))
      s += '\n' + i18n( "Company" ) + ": " + *ce->find( "ORG" );

    if (ce->find( "X-Notes" )) {
      QString notes = (*ce->find( "X-Notes" )).stripWhiteSpace();
      if ( !notes.isEmpty() ) {
        notes += '\n';
        s += '\n' + i18n( "Notes:" ) + "\n";
        QFontMetrics fm( font() );

        // Begin word wrap code based on QMultiLineEdit code
        int i = 0;
        bool doBreak = false;
        int linew = 0;
        int lastSpace = -1;
        int a = 0;
        int lastw = 0;

        while ( i < int(notes.length()) ) {
          doBreak = FALSE;
          if ( notes[i] != '\n' )
            linew += fm.width( notes[i] );

          if ( lastSpace >= a && notes[i] != '\n' )
            if  (linew >= parentWidget()->width()) {
              doBreak = TRUE;
              if ( lastSpace > a ) {
                i = lastSpace;
                linew = lastw;
              }
              else
                i = QMAX( a, i-1 );
            }

          if ( notes[i] == '\n' || doBreak ) {
            s += notes.mid( a, i - a + (doBreak?1:0) ) +"\n";

            a = i + 1;
            lastSpace = a;
            linew = 0;
          }

          if ( notes[i].isSpace() ) {
            lastSpace = i;
            lastw = linew;
          }
	
          if ( lastSpace <= a ) {
            lastw = linew;
          }

          ++i;
        }
	// End wordwrap code based on QMultiLineEdit code
      }

    }

    tip( r, s );
}

///////////////////////////
// ContactListViewItem Methods

ContactListViewItem::ContactListViewItem( QString entryKey,
				  ContactListView* parent,
				  QStringList* field )
  : QListViewItem( parent ), entryKey_( entryKey ), field( field ),
    parentListView( parent )
{
  refresh();
}

QString ContactListViewItem::entryKey()
{
  return entryKey_;
}

ContactEntry *ContactListViewItem::getEntry()
{
  ContactEntryList *cel = parent()->getKAddressBookView()->contactEntryList();
  ContactEntry *ce = cel->find( entryKey_ );
  if (!ce)  // can only happen to shared address book
    qDebug( "ContactListViewItem::getEntry() Associated ContactEntry not found" );
  return ce;
}

QString ContactListViewItem::key( int column, bool ascending ) const
{
  return QListViewItem::key( column, ascending ).lower();
}

// Some of this is very similar to TrollTechs code,
// I should ask if it's ok or not.
void ContactListViewItem::paintCell ( QPainter * p,
				  const QColorGroup & cg,
				  int column,
				  int width,
				  int align )
{
  if ( !p )
    return;

  QListView *lv = listView();
  int r = lv ? lv->itemMargin() : 1;
  const QPixmap * icon = pixmap( column );
  int marg = lv ? lv->itemMargin() : 1;

  if (!parentListView->backPixmapOn || parentListView->background.isNull()) {
    p->fillRect( 0, 0, width, height(), cg.base() );
    if ( isSelected() &&
	 (column==0 || listView()->allColumnsShowFocus()) ) {
      p->fillRect( r - marg, 0, width - r + marg, height(),
		   cg.brush( QColorGroup::Highlight ) );
      p->setPen( cg.highlightedText() );
    } else {
      p->setPen( cg.text() );
    }
  }
  else {
    QRect rect = parentListView->itemRect( this );
    int cw = 0;
    cw = parentListView->header()->cellPos( column );

    QPixmap* back = &(parentListView->background);
    if (isSelected()) {
      back = &(parentListView->iBackground);
      p->setPen( cg.highlightedText() );
    }
    p->drawTiledPixmap( 0, 0, width, height(),
			*back,
			rect.left() + cw + parentListView->contentsX(),
			rect.top() + parentListView->contentsY() );

    if ( icon ) {
      p->drawPixmap( r, (height()-icon->height())/2, *icon );
      r += icon->width() + listView()->itemMargin();
    }
  }

  QString t = text( column );
  if ( !t.isEmpty() ) {
    p->drawText( r, 0, width-marg-r, height(),
		 align | AlignVCenter, t );
  }

  if (parentListView->underline) {
    p->setPen( parentListView->cUnderline );
    p->drawLine( 0, height() - 1, width, height() - 1 );
  }
}

ContactListView *ContactListViewItem::parent()
{
  return parentListView;
}


void ContactListViewItem::refresh()
{
  ContactEntry *ce = getEntry();
  if (!ce)
    return;
  for ( uint i = 0; i < field->count(); i++ ) {
    if ((*field)[i] == "X-FileAs")
      if (ce->find( "X-Notes" ))
	setPixmap( i, QPixmap( "abentry" ));
      else {
	setPixmap( i, QPixmap( "group" ));
      }
    if (ce->find( (*field)[i] ))
      setText( i, *(ce->find( (*field)[i] )));
    else
      setText( i, "" );
  }
}
ContactListView::ContactListView( KAddressBookView *parent, const char *name )
  : QListView( parent, name ),
    pabWidget( parent ),
    oldColumn( 0 )
{
  setAcceptDrops( true );
  viewport()->setAcceptDrops( true );
  setAllColumnsShowFocus( true );
  setShowSortIndicator(true);
  setSelectionMode( Extended );
  up = new QIconSet( BarIcon("abup" ), QIconSet::Small );
  down = new QIconSet( BarIcon("abdown" ), QIconSet::Small );
  new DynamicTip( this );
  readConfig();
  loadBackground();
}

KAddressBookView* ContactListView::getKAddressBookView()
{
  return pabWidget;
}

ContactListViewItem *ContactListView::getItem( QString entryKey )
{
  QListViewItem *item = firstChild();
  ContactListViewItem *plvi;
  while (item) {
    plvi = dynamic_cast< ContactListViewItem* >(item);
    if (plvi && (plvi->entryKey() == entryKey))
      return plvi;
    item = item->nextSibling();
  }
  return 0;
}

void ContactListView::loadBackground()
{
  kdDebug() << "Image format " << QPixmap::imageFormat( backPixmap ) << endl;
  if (backPixmapOn && QPixmap::imageFormat( backPixmap )) {
    background = QPixmap( backPixmap );
    QImage invertedBackground( backPixmap );

    /* Thin lines
    const int multa = 6;
    const int multb = 1;
    const int multc = multa + multb;
    invertedBackground.convertDepth( 32 );
    for (int y = 17; y < invertedBackground.height(); y += 18)
      for( int x = 0; x < invertedBackground.width(); ++x ) {
	QRgb a = invertedBackground.pixel( x, y );
	a = ((qRed(a)*multa + (0xFF - qRed(a))*multb)/multc << 16) |
		    ((qGreen(a)*multa + (0xFF - qGreen(a))*multb)/multc << 8) |
		    (qBlue(a)*multa + (0xFF - qBlue(a))*multb)/multc;
	invertedBackground.setPixel( x, y, a  ); //qRed(a) << 16 );
      }
    background.convertFromImage( invertedBackground );
    */

    invertedBackground.invertPixels();
    iBackground.convertFromImage( invertedBackground );
  }
  else {
    background = QPixmap();
    iBackground = QPixmap();
  }
}

void ContactListView::saveConfig()
{
  KConfig *config = kapp->config();

  config->setGroup("ListView");
  config->writeEntry( "sortColumn", header()->mapToActual( column ));
  config->writeEntry( "sortDirection", ascending );

  config->writeEntry( "backPixmapOn", backPixmapOn );
  config->writeEntry( "backPixmap", backPixmap );

  config->writeEntry( "underline", underline );
  config->writeEntry( "autUnderline", autUnderline );
  config->writeEntry( "cUnderline", cUnderline );

  config->writeEntry( "tooltips", tooltips_ );
}

void ContactListView::readConfig()
{
  KConfig *config = kapp->config();

  config->setGroup("ListView");
  column = config->readNumEntry( "sortColumn", 0 );
  ascending = config->readBoolEntry( "sortDirection", true );

  backPixmapOn = config->readBoolEntry( "backPixmapOn", false );
  backPixmap = config->readEntry( "backPixmap", "" );

  underline = config->readBoolEntry( "underline", true );
  autUnderline = config->readBoolEntry( "autUnderline", true );
  cUnderline = config->readColorEntry( "cUnderline" );
  if (autUnderline)
    cUnderline = kapp->palette().normal().background();
  tooltips_ = config->readBoolEntry( "tooltips", true );
}

// untested, changing kde color scheme isn't affecting qt 2.0 based apps
void ContactListView::backgroundColorChange( const QColor &color )
{
  if (autUnderline)
    cUnderline = kapp->palette().normal().background();
  QListView::backgroundColorChange( color );
}

void ContactListView::paintEmptyArea( QPainter * p, const QRect & rect )
{
  if (backPixmapOn && !background.isNull())
    p->drawTiledPixmap( rect.left(), rect.top(), rect.width(), rect.height(),
			background,
			rect.left() + contentsX(),
			rect.top() + contentsY() );
  else
    p->fillRect( rect, colorGroup().base() );
  //    p->fillRect( rect, QColor( 255, 0, 0) );
}

// It should be note that QListView supplies outstanding incremental
// searching, just give it focus and try!
// This class is pretty much just eye candy :-)
// (might be useful for people who can't type quickly or
// don't realize how good the built in incremental searching is)
void ContactListView::incSearch( const QString &search )
{
  if (search.isEmpty())
    return;

  QString value = search.lower();
  QListViewItem *citem = currentItem();

  if (ascending) {
    if (!citem)
      citem = firstChild();
    if (!citem)
      return;

    QListViewItem *ib = citem->itemAbove();
    while (ib && //(citem->key( column, ascending ).find( value ) != 0) &&
	   ((ib->key( column, ascending ) > value) ||
	    (ib->key( column, ascending ).find( value ) == 0))) {
      citem = ib;
      ib = ib->itemAbove();
    }

    ib = citem->itemBelow();
    while (ib && (citem->key( column, ascending ).find( value ) != 0) &&
	   ((ib->key( column, ascending ) < value) ||
	    (ib->key( column, ascending ).find( value ) == 0))) {
      kdDebug() << ib->key( column, ascending ) << endl;
      citem = ib;
      ib = ib->itemBelow();
    }
  }
  else {
    if (!citem)
      citem = firstChild();
    if (!citem)
      return;

    QListViewItem *ib = citem->itemAbove();
    while (ib && //(citem->key( column, ascending ).find( value ) != 0) &&
	   ((ib->key( column, ascending ) < value) ||
	    (ib->key( column, ascending ).find( value ) == 0))) {
      citem = ib;
      ib = ib->itemAbove();
    }

    ib = citem->itemBelow();
    while (ib && (citem->key( column, ascending ).find( value ) != 0) &&
	   ((ib->key( column, ascending ) > value) ||
	    (ib->key( column, ascending ).find( value ) == 0))) {
      kdDebug() << ib->key( column, ascending ) << endl;
      citem = ib;
      ib = ib->itemBelow();
    }
  }


  clearSelection();
  setSelected( citem, true );
  setCurrentItem( citem );
  ensureItemVisible( citem );
}

void ContactListView::setSorting( int column )
{
  this->column = column;
  this->ascending = true;

  oldColumn = column;
  QListView::setSorting( column, ascending );
}

void ContactListView::setSorting( int column, bool ascending )
{
  this->column = column;
  this->ascending = ascending;

  QString a,b = "descending";
  a.setNum( column );
  if (ascending)
    b = "ascending";

  oldColumn = column;
  QListView::setSorting( column, ascending );

  if (column != -1)
    pabWidget->cbField->setCurrentItem( column );
}

void ContactListView::resort()
{
  int col = column;
  setSorting( -1, ascending );
  setSorting( col, ascending );
}

bool ContactListView::tooltips()
{
  return tooltips_;
}

void ContactListView::contentsMousePressEvent(QMouseEvent* e)
{
  presspos = e->pos();
  QListView::contentsMousePressEvent(e);
}


// To initiate a drag operation
void ContactListView::contentsMouseMoveEvent( QMouseEvent *e )
{
  if ((e->state() & LeftButton) && (e->pos() - presspos).manhattanLength() > 4 ) {
    QDragObject *drobj;
    drobj = new QTextDrag( pabWidget->selectedEmails(), this );
    drobj->dragCopy();
  }
  else
    QListView::contentsMouseMoveEvent( e );
}

void ContactListView::contentsDragEnterEvent( QDragEnterEvent *e )
{
  if ( !QUriDrag::canDecode(e) ) {
    e->ignore();
    return;
  }
  e->accept();
}

void ContactListView::contentsDropEvent( QDropEvent *e )
{
  QStrList strings;
  if ( QUriDrag::decode( e, strings ) ) {
    QString m("Full URLs:\n");
    for (const char* u=strings.first(); u; u=strings.next())
      if (u && (KURL::decode_string(u).find( "mailto:" ) == 0)) {
	pabWidget->addEmail( KURL::decode_string(u).mid(7) );
	return;
      }
  }
}

void ContactListView::keyPressEvent( QKeyEvent *e )
{
  if (e->key() == Key_Delete)
    pabWidget->clear();
  QListView::keyPressEvent( e );
}

#include "contactlistview.moc"
