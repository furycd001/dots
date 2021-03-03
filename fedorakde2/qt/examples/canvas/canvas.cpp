#include <qdatetime.h>
#include <qmainwindow.h>
#include <qstatusbar.h>
#include <qmessagebox.h>
#include <qmenubar.h>
#include <qapplication.h>
#include <qkeycode.h>
#include <qpainter.h>
#include <qlabel.h>
#include <qimage.h>
#include <qprogressdialog.h>
#include "canvas.h"

#include <stdlib.h>

// We use a global variable to save memory - all the brushes and pens in
// the mesh are shared.
static QBrush *tb = 0;
static QPen *tp = 0;

class EdgeItem;
class NodeItem;

class EdgeItem: public QCanvasLine
{
public:
    EdgeItem( NodeItem*, NodeItem*, QCanvas *canvas );
    void setFromPoint( int x, int y ) ;
    void setToPoint( int x, int y );
    static int count() { return c; }
    void moveBy(double dx, double dy);
private:
    static int c;
};

static const int imageRTTI = 984376;


class ImageItem: public QCanvasRectangle
{
public:
    ImageItem( QImage img, QCanvas *canvas );
    int rtti () const { return imageRTTI; }
    bool hit( const QPoint&) const;
protected:
    void drawShape( QPainter & );
private:
    QImage image;
    QPixmap pixmap;
};


ImageItem::ImageItem( QImage img, QCanvas *canvas )
    : QCanvasRectangle( canvas ), image(img)
{
    setSize( image.width(), image.height() );

#ifndef _WS_QWS_
    pixmap.convertFromImage(image, OrderedAlphaDither);
#endif
}


void ImageItem::drawShape( QPainter &p )
{
// On Qt/Embedded, we can paint a QImage as fast as a QPixmap,
// but on other platforms, we need to use a QPixmap.
#ifdef _WS_QWS_
    p.drawImage( int(x()), int(y()), image, 0, 0, -1, -1, OrderedAlphaDither );
#else
    p.drawPixmap( int(x()), int(y()), pixmap );
#endif
}

bool ImageItem::hit( const QPoint &p ) const
{
    int ix = p.x()-int(x());
    int iy = p.y()-int(y());
    if ( !image.valid( ix , iy ) )
	return FALSE;
    QRgb pixel = image.pixel( ix, iy );
    return qAlpha( pixel ) != 0;
}

class NodeItem: public QCanvasEllipse
{
public:
    NodeItem( QCanvas *canvas );
    ~NodeItem() {}
    
    void addInEdge( EdgeItem *edge ) { inList.append( edge ); }
    void addOutEdge( EdgeItem *edge ) { outList.append( edge ); }

    void moveBy(double dx, double dy);

    //    QPoint center() { return boundingRect().center(); }
private:
    QList<EdgeItem> inList;
    QList<EdgeItem> outList;
};


int EdgeItem::c = 0;


void EdgeItem::moveBy(double, double)
{
    //nothing
}

EdgeItem::EdgeItem( NodeItem *from, NodeItem *to, QCanvas *canvas )
    : QCanvasLine( canvas )
{
    c++;
    setPen( *tp );
    setBrush( *tb );
    from->addOutEdge( this );
    to->addInEdge( this );
    setPoints( int(from->x()), int(from->y()), int(to->x()), int(to->y()) );
    setZ( 127 );
}

void EdgeItem::setFromPoint( int x, int y )
{
    setPoints( x,y, endPoint().x(), endPoint().y() );
}

void EdgeItem::setToPoint( int x, int y )
{
    setPoints( startPoint().x(), startPoint().y(), x, y );
}



void NodeItem::moveBy(double dx, double dy)
{
    QCanvasEllipse::moveBy( dx, dy );

    QListIterator<EdgeItem> it1( inList );
    EdgeItem *edge;
    while (( edge = it1.current() )) {
	++it1;
	edge->setToPoint( int(x()), int(y()) );
    }
    QListIterator<EdgeItem> it2( outList );
    while (( edge = it2.current() )) {
	++it2;
	edge->setFromPoint( int(x()), int(y()) );
    }
}

NodeItem::NodeItem( QCanvas *canvas )
    : QCanvasEllipse( 6, 6, canvas )
{
    setPen( *tp );
    setBrush( *tb );
    setZ( 128 );
}

FigureEditor::FigureEditor(
	QCanvas& c, QWidget* parent,
	const char* name, WFlags f) :
    QCanvasView(&c,parent,name,f)
{
}

void FigureEditor::contentsMousePressEvent(QMouseEvent* e)
{
    QCanvasItemList l=canvas()->collisions(e->pos());
    for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) {
	if ( (*it)->rtti() == imageRTTI ) {
	    ImageItem *item= (ImageItem*)(*it);
	    if ( !item->hit( e->pos() ) )
		 continue;
	}
	moving = *it;
	moving_start = e->pos();
	return;
    }
    moving = 0;
}

void FigureEditor::clear()
{
    QCanvasItemList list = canvas()->allItems();
    QCanvasItemList::Iterator it = list.begin();
    for (; it != list.end(); ++it) {
	if ( *it )
	    delete *it;
    }
}

void Main::init()
{
    clear();

    static int r=24;
    srand(++r);

    int i;

    for ( i=0; i<canvas.width() / 56; i++) {
	addButterfly();
    }
    for ( i=0; i<canvas.width() / 85; i++) {
	addHexagon();
    }
    for ( i=0; i<canvas.width() / 128; i++) {
	addLogo();
    }
}

void FigureEditor::contentsMouseMoveEvent(QMouseEvent* e)
{
    if ( moving ) {
	moving->moveBy(e->pos().x() - moving_start.x(),
		       e->pos().y() - moving_start.y());
	moving_start = e->pos();
	canvas()->update();
    }
}



BouncyLogo::BouncyLogo(QCanvas* canvas) :
    QCanvasSprite(0,canvas)
{
    static QCanvasPixmapArray logo("qt-trans.xpm");
    setSequence(&logo);
    setAnimated(TRUE);
    initPos();
}


const int logo_rtti = 1234;

int BouncyLogo::rtti() const
{
    return logo_rtti;
}

void BouncyLogo::initPos()
{
    initSpeed();
    int trial=1000;
    do {
	move(rand()%canvas()->width(),rand()%canvas()->height());
	advance(0);
    } while (trial-- && xVelocity()==0.0 && yVelocity()==0.0);
}

void BouncyLogo::initSpeed()
{
    const double speed = 4.0;
    double d = (double)(rand()%1024) / 1024.0;
    setVelocity( d*speed*2-speed, (1-d)*speed*2-speed );
}

void BouncyLogo::advance(int stage)
{
    switch ( stage ) {
      case 0: {
	double vx = xVelocity();
	double vy = yVelocity();

	if ( vx == 0.0 && vy == 0.0 ) {
	    // stopped last turn
	    initSpeed();
	    vx = xVelocity();
	    vy = yVelocity();
	}

	double nx = x() + vx;
	double ny = y() + vy;

	if ( nx < 0 || nx >= canvas()->width() )
	    vx = -vy;
	if ( ny < 0 || ny >= canvas()->height() )
	    vy = -vy;

	for (int bounce=0; bounce<4; bounce++) {
	    QCanvasItemList l=collisions(FALSE);
	    for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) {
		QCanvasItem *hit = *it;
		if ( hit->rtti()==logo_rtti && hit->collidesWith(this) ) {
		    switch ( bounce ) {
		      case 0:
			vx = -vx;
			break;
		      case 1:
			vy = -vy;
			vx = -vx;
			break;
		      case 2:
			vx = -vx;
			break;
		      case 3:
			// Stop for this turn
			vx = 0;
			vy = 0;
			break;
		    }
		    setVelocity(vx,vy);
		    break;
		}
	    }
	}

	if ( x()+vx < 0 || x()+vx >= canvas()->width() )
	    vx = 0;
	if ( y()+vy < 0 || y()+vy >= canvas()->height() )
	    vy = 0;

	setVelocity(vx,vy);
      } break;
      case 1:
	QCanvasItem::advance(stage);
        break;
    }
}


Main::Main(QCanvas& c, QWidget* parent, const char* name, WFlags f) :
    QMainWindow(parent,name,f),
    canvas(c)
{
    editor = new FigureEditor(canvas,this);
    QMenuBar* menu = menuBar();

    QPopupMenu* file = new QPopupMenu;
    file->insertItem("&Fill canvas", this, SLOT(init()), CTRL+Key_F);
    file->insertItem("&Erase canvas", this, SLOT(clear()), CTRL+Key_E);
    file->insertItem("&New view", this, SLOT(newView()), CTRL+Key_N);
    file->insertSeparator();
    file->insertItem("E&xit", qApp, SLOT(quit()), CTRL+Key_Q);
    menu->insertItem("&File", file);

    QPopupMenu* edit = new QPopupMenu;
    edit->insertItem("Add &Circle", this, SLOT(addCircle()), CTRL+Key_C);
    edit->insertItem("Add &Hexagon", this, SLOT(addHexagon()), CTRL+Key_H);
    edit->insertItem("Add &Polygon", this, SLOT(addPolygon()), CTRL+Key_P);
    edit->insertItem("Add &Text", this, SLOT(addText()), CTRL+Key_T);
    edit->insertItem("Add &Line", this, SLOT(addLine()), CTRL+Key_L);
    edit->insertItem("Add &Rectangle", this, SLOT(addRectangle()), CTRL+Key_R);
    edit->insertItem("Add &Sprite", this, SLOT(addSprite()), CTRL+Key_S);
    edit->insertItem("Create &Mesh", this, SLOT(addMesh()) );
    edit->insertItem("Add &Alpha-blended image", this, SLOT(addButterfly()), CTRL+Key_A);
    edit->insertSeparator();
    edit->insertItem("&Enlarge", this, SLOT(enlarge()), CTRL+Key_Plus);
    edit->insertItem("Shr&ink", this, SLOT(shrink()), CTRL+Key_Minus);
    menu->insertItem("&Edit", edit);

    options = new QPopupMenu;
    dbf_id = options->insertItem("Double buffer", this, SLOT(toggleDoubleBuffer()));
    options->setItemChecked(dbf_id, TRUE);
    menu->insertItem("&Options",options);

    menu->insertSeparator();

    QPopupMenu* help = new QPopupMenu;
    help->insertItem("&About", this, SLOT(help()), Key_F1);
    help->setItemChecked(dbf_id, TRUE);
    menu->insertItem("&Help",help);

    statusBar();

    setCentralWidget(editor);

    init();
}

void Main::newView()
{
    // Open a new view... have it delete when closed.
    Main *m = new Main(canvas, 0, 0, WDestructiveClose);
    qApp->setMainWidget(m);
    m->show();
    qApp->setMainWidget(0);
}

void Main::clear()
{
    editor->clear();
}

void Main::help()
{
    static QMessageBox* about = new QMessageBox( "Qt Canvas Example",
	    "<h3>The QCanvas classes example</h3>"
	    "<ul>"
		"<li> Press CTRL-S for some sprites."
		"<li> Press CTRL-C for some circles."
		"<li> Press CTRL-L for some lines."
		"<li> Drag the objects around."
		"<li> Read the code!"
	    "</ul>", QMessageBox::Information, 1, 0, 0, this, 0, FALSE );
    about->setButtonText( 1, "Dismiss" );
    about->show();
}

void Main::aboutQt()
{
    QMessageBox::aboutQt( this, "Qt Canvas Example" );
}

void Main::toggleDoubleBuffer()
{
    bool s = !options->isItemChecked(dbf_id);
    options->setItemChecked(dbf_id,s);
    canvas.setDoubleBuffering(s);
}

void Main::enlarge()
{
    canvas.resize(canvas.width()*4/3, canvas.height()*4/3);
}

void Main::shrink()
{
    canvas.resize(canvas.width()*3/4, canvas.height()*3/4);
}

void Main::addSprite()
{
    QCanvasItem* i = new BouncyLogo(&canvas);
    i->setZ(rand()%256);
    i->show();
}

QString butterfly_fn;
QString logo_fn;


void Main::addButterfly()
{
    if ( butterfly_fn.isEmpty() )
	return;
    static QImage *img;
    if ( !img ) {
	img = new QImage[4];
	img[0].load( butterfly_fn );
	img[1] = img[0].smoothScale( int(img[0].width()*0.75),
		int(img[0].height()*0.75) );
	img[2] = img[0].smoothScale( int(img[0].width()*0.5),
		int(img[0].height()*0.5) );
	img[3] = img[0].smoothScale( int(img[0].width()*0.25),
		int(img[0].height()*0.25) );
    }
    QCanvasPolygonalItem* i = new ImageItem(img[rand()%4],&canvas);
    i->move(rand()%(canvas.width()-img->width()),
	    rand()%(canvas.height()-img->height()));
    i->setZ(rand()%256+250);
    i->show();
}

void Main::addLogo()
{
    if ( logo_fn.isEmpty() )
	return;
    static QImage *img;
    if ( !img ) {
	img = new QImage[4];
	img[0].load( logo_fn );
	img[1] = img[0].smoothScale( int(img[0].width()*0.75),
		int(img[0].height()*0.75) );
	img[2] = img[0].smoothScale( int(img[0].width()*0.5),
		int(img[0].height()*0.5) );
	img[3] = img[0].smoothScale( int(img[0].width()*0.25),
		int(img[0].height()*0.25) );
    }
    QCanvasPolygonalItem* i = new ImageItem(img[rand()%4],&canvas);
    i->move(rand()%(canvas.width()-img->width()),
	    rand()%(canvas.height()-img->width()));
    i->setZ(rand()%256+256);
    i->show();
}



void Main::addCircle()
{
    QCanvasPolygonalItem* i = new QCanvasEllipse(50,50,&canvas);
    i->setBrush( QColor(rand()%32*8,rand()%32*8,rand()%32*8) );
    i->move(rand()%canvas.width(),rand()%canvas.height());
    i->setZ(rand()%256);
    i->show();
}

void Main::addHexagon()
{
    QCanvasPolygon* i = new QCanvasPolygon(&canvas);
    const int size = canvas.width() / 25;
    QPointArray pa(6);
    pa[0] = QPoint(2*size,0);
    pa[1] = QPoint(size,-size*173/100);
    pa[2] = QPoint(-size,-size*173/100);
    pa[3] = QPoint(-2*size,0);
    pa[4] = QPoint(-size,size*173/100);
    pa[5] = QPoint(size,size*173/100);
    i->setPoints(pa);
    i->setBrush( QColor(rand()%32*8,rand()%32*8,rand()%32*8) );
    i->move(rand()%canvas.width(),rand()%canvas.height());
    i->setZ(rand()%256);
    i->show();
}

void Main::addPolygon()
{
    QCanvasPolygon* i = new QCanvasPolygon(&canvas);
    const int size = canvas.width()/2;
    QPointArray pa(6);
    pa[0] = QPoint(0,0);
    pa[1] = QPoint(size,size/5);
    pa[2] = QPoint(size*4/5,size);
    pa[3] = QPoint(size/6,size*5/4);
    pa[4] = QPoint(size*3/4,size*3/4);
    pa[5] = QPoint(size*3/4,size/4);
    i->setPoints(pa);
    i->setBrush( QColor(rand()%32*8,rand()%32*8,rand()%32*8) );
    i->move(rand()%canvas.width(),rand()%canvas.height());
    i->setZ(rand()%256);
    i->show();
}

void Main::addText()
{
    QCanvasText* i = new QCanvasText(&canvas);
    i->setText("QCanvasText");
    i->move(rand()%canvas.width(),rand()%canvas.height());
    i->setZ(rand()%256);
    i->setFont(QFont("helvetica",24));
    i->show();
}

void Main::addLine()
{
    QCanvasLine* i = new QCanvasLine(&canvas);
    i->setPoints( rand()%canvas.width(), rand()%canvas.height(),
                  rand()%canvas.width(), rand()%canvas.height() );
    i->setPen( QPen(QColor(rand()%32*8,rand()%32*8,rand()%32*8), 6) );
    i->setZ(rand()%256);
    i->show();
}

void Main::addMesh()
{
    int x0 = 0;
    int y0 = 0;

    if ( !tb ) tb = new QBrush( Qt::red );
    if ( !tp ) tp = new QPen( Qt::black );

    int nodecount = 0;

    int w = canvas.width();
    int h = canvas.height();

    const int dist = 30;
    int rows = h / dist;
    int cols = w / dist;

#ifndef QT_NO_PROGRESSDIALOG
    QProgressDialog progress( "Creating mesh...", "Abort", rows,
			      this, "progress", TRUE );
#endif

    QArray<NodeItem*> lastRow(cols);
    for ( int j = 0; j < rows; j++ ) {
	int n = j%2 ? cols-1 : cols;
	NodeItem *prev = 0;
	for ( int i = 0; i < n; i++ ) {
	    NodeItem *el = new NodeItem( &canvas );
	    nodecount++;
	    int r = rand();
	    int xrand = r %20;
	    int yrand = (r/20) %20;
	    el->move( xrand + x0 + i*dist + (j%2 ? dist/2 : 0 ),
		      yrand + y0 + j*dist );

	    if ( j > 0 ) {
		if ( i < cols-1 )
		    (new EdgeItem( lastRow[i], el, &canvas ))->show();
		if ( j%2 )
		    (new EdgeItem( lastRow[i+1], el, &canvas ))->show();
		else if ( i > 0 )
		    (new EdgeItem( lastRow[i-1], el, &canvas ))->show();
	    }
	    if ( prev ) {
		(new EdgeItem( prev, el, &canvas ))->show();
	    }
	    if ( i > 0 ) lastRow[i-1] = prev;
	    prev = el;
	    el->show();
	}
	lastRow[n-1]=prev;
#ifndef QT_NO_PROGRESSDIALOG
	progress.setProgress( j );
	if ( progress.wasCancelled() )
	    break;
#endif
    }
#ifndef QT_NO_PROGRESSDIALOG
    progress.setProgress( rows );
#endif
    // qDebug( "%d nodes, %d edges", nodecount, EdgeItem::count() );
}

void Main::addRectangle()
{
    QCanvasPolygonalItem *i = new QCanvasRectangle( rand()%canvas.width(),rand()%canvas.height(),
			    canvas.width()/5,canvas.width()/5,&canvas);
    int z = rand()%256;
    i->setBrush( QColor(z,z,z) );
    i->setPen( QPen(QColor(rand()%32*8,rand()%32*8,rand()%32*8), 6) );
    i->setZ(z);
    i->show();
}

