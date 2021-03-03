#include <kconfig.h>
#include "kwmthemeclient.h"
#include <kglobal.h>
#include <qlayout.h>
#include <qdrawutil.h>
#include <kpixmapeffect.h>
#include <kstddirs.h>
#include <kglobal.h>
#include <klocale.h>
#include <qbitmap.h>
#include "../../workspace.h"
#include "../../options.h"


using namespace KWinInternal;


static QPixmap stretchPixmap(QPixmap& src, bool stretchVert){
  QPixmap dest;
  QBitmap *srcMask, *destMask;
  int w, h, w2, h2;
  QPainter p;

  if (src.isNull()) return src;

  w = src.width();
  h = src.height();

  if (stretchVert){
    w2 = w;
    for (h2=h; h2<100; h2=h2<<1)
      ;
  }
  else{
    h2 = h;
    for (w2=w; w2<100; w2=w2<<1)
      ;
  }
  if (w2==w && h2==h) return src;

  dest = src;
  dest.resize(w2, h2);

  p.begin(&dest);
  p.drawTiledPixmap(0, 0, w2, h2, src);
  p.end();

  srcMask = (QBitmap*)src.mask();
  if (srcMask){
    destMask = (QBitmap*)dest.mask();
    p.begin(destMask);
    p.drawTiledPixmap(0, 0, w2, h2, *srcMask);
    p.end();
  }
  return dest;
}



enum FramePixmap{FrameTop=0, FrameBottom, FrameLeft, FrameRight, FrameTopLeft,
   FrameTopRight, FrameBottomLeft, FrameBottomRight};

static QPixmap *framePixmaps[8];
static QPixmap *menuPix, *iconifyPix, *closePix, *maxPix, *minmaxPix,
    *pinupPix, *pindownPix;
static KPixmap *aTitlePix = 0;
static KPixmap *iTitlePix = 0;
static KPixmapEffect::GradientType grType;
static int maxExtent, titleAlign;
static bool titleGradient = true;
static bool pixmaps_created = false;
static bool titleSunken = false;
static bool titleTransparent;

static void create_pixmaps()
{
    const char *keys[] = {"wm_top", "wm_bottom", "wm_left", "wm_right",
    "wm_topleft", "wm_topright", "wm_bottomleft", "wm_bottomright"};

    if(pixmaps_created)
        return;
    pixmaps_created = true;
    
    KConfig *config = KGlobal::config();
    config->setGroup("General");
    QString tmpStr;

    for(int i=0; i < 8; ++i)
    {
        framePixmaps[i] = new QPixmap(locate("appdata",
                                      "pics/"+config->readEntry(keys[i], " ")));
        if(framePixmaps[i]->isNull())
            qWarning("Unable to load frame pixmap for %s", keys[i]);
    }

    *framePixmaps[FrameTop] = stretchPixmap(*framePixmaps[FrameTop], false);
    *framePixmaps[FrameBottom] = stretchPixmap(*framePixmaps[FrameBottom], false);
    *framePixmaps[FrameLeft] = stretchPixmap(*framePixmaps[FrameLeft], true);
    *framePixmaps[FrameRight] = stretchPixmap(*framePixmaps[FrameRight], true);

    maxExtent = framePixmaps[FrameTop]->height();
    if(framePixmaps[FrameBottom]->height() > maxExtent)
        maxExtent = framePixmaps[FrameBottom]->height();
    if(framePixmaps[FrameLeft]->width() > maxExtent)
        maxExtent = framePixmaps[FrameLeft]->width();
    if(framePixmaps[FrameRight]->width() > maxExtent)
        maxExtent = framePixmaps[FrameRight]->width();

    maxExtent++;

    menuPix = new QPixmap(locate("appdata",
                                 "pics/"+config->readEntry("menu", " ")));
    iconifyPix = new QPixmap(locate("appdata",
                                    "pics/"+config->readEntry("iconify", " ")));
    maxPix = new QPixmap(locate("appdata",
                                "pics/"+config->readEntry("maximize", " ")));
    minmaxPix = new QPixmap(locate("appdata",
                                   "pics/"+config->readEntry("maximizedown", " ")));
    closePix = new QPixmap(locate("appdata",
                                  "pics/"+config->readEntry("close", " ")));
    pinupPix = new QPixmap(locate("appdata",
                                  "pics/"+config->readEntry("pinup", " ")));
    pindownPix = new QPixmap(locate("appdata",
                                    "pics/"+config->readEntry("pindown", " ")));
    if(menuPix->isNull())                           
        menuPix->load(locate("appdata", "pics/menu.png"));
    if(iconifyPix->isNull())
        iconifyPix->load(locate("appdata", "pics/iconify.png"));
    if(maxPix->isNull())
        maxPix->load(locate("appdata", "pics/maximize.png"));
    if(minmaxPix->isNull())
        minmaxPix->load(locate("appdata", "pics/maximizedown.png"));
    if(closePix->isNull())
        closePix->load(locate("appdata", "pics/close.png"));
    if(pinupPix->isNull())
        pinupPix->load(locate("appdata", "pics/pinup.png"));
    if(pindownPix->isNull())
        pindownPix->load(locate("appdata", "pics/pindown.png"));
    
    tmpStr = config->readEntry("TitleAlignment");
    if(tmpStr == "right")
        titleAlign = Qt::AlignRight | Qt::AlignVCenter;
    else if(tmpStr == "middle")
        titleAlign = Qt::AlignCenter;
    else
        titleAlign = Qt::AlignLeft | Qt::AlignVCenter;
    titleSunken = config->readBoolEntry("TitleFrameShaded", true);
    // titleSunken = true; // is this fixed?
    titleTransparent = config->readBoolEntry("PixmapUnderTitleText", true);

    tmpStr = config->readEntry("TitlebarLook");
    if(tmpStr == "shadedVertical"){
        aTitlePix = new KPixmap;
        aTitlePix->resize(32, 20);
        KPixmapEffect::gradient(*aTitlePix,
                                options->color(Options::TitleBar, true),
                                options->color(Options::TitleBlend, true),
                                KPixmapEffect::VerticalGradient);
        iTitlePix = new KPixmap;
        iTitlePix->resize(32, 20);
        KPixmapEffect::gradient(*iTitlePix,
                                options->color(Options::TitleBar, false),
                                options->color(Options::TitleBlend, false),
                                KPixmapEffect::VerticalGradient);
        titleGradient = false; // we can just tile this

    }
    else if(tmpStr == "shadedHorizontal")
        grType = KPixmapEffect::HorizontalGradient;
    else if(tmpStr == "shadedDiagonal")
        grType = KPixmapEffect::DiagonalGradient;
    else if(tmpStr == "shadedCrossDiagonal")
        grType = KPixmapEffect::CrossDiagonalGradient;
    else if(tmpStr == "shadedPyramid")
        grType = KPixmapEffect::PyramidGradient;
    else if(tmpStr == "shadedRectangle")
        grType = KPixmapEffect::RectangleGradient;
    else if(tmpStr == "shadedPipeCross")
        grType = KPixmapEffect::PipeCrossGradient;
    else if(tmpStr == "shadedElliptic")
        grType = KPixmapEffect::EllipticGradient;
    else{
        titleGradient = false;
        tmpStr = config->readEntry("TitlebarPixmapActive", "");
        if(!tmpStr.isEmpty()){
            aTitlePix = new KPixmap;
            aTitlePix->load(locate("appdata", "pics/" + tmpStr));
        }
        else
            aTitlePix = NULL;
        tmpStr = config->readEntry("TitlebarPixmapInactive", "");
        if(!tmpStr.isEmpty()){
            iTitlePix = new KPixmap;
            iTitlePix->load(locate("appdata", "pics/" + tmpStr));
        }
        else
            iTitlePix = NULL;
    }
}

static void delete_pixmaps()
{
   for(int i=0; i < 8; ++i)
        delete framePixmaps[i];

   delete menuPix;
   delete iconifyPix;
   delete closePix;
   delete maxPix;
   delete minmaxPix;
   delete pinupPix;
   delete pindownPix;
   delete aTitlePix;
   aTitlePix = 0;
   delete iTitlePix;
   iTitlePix = 0;

   titleGradient = true;
   pixmaps_created = false;
   titleSunken = false;
}

void KWMThemeClient::slotReset()
{

}

void MyButton::drawButtonLabel(QPainter *p)
{
    if(pixmap()){
	// If we have a theme who's button covers the entire width or
	// entire height, we shift down/right by 1 pixel so we have
	// some visual notification of button presses. i.e. for MGBriezh
	int offset = (isDown() && ((pixmap()->width() >= width()) || 
                         (pixmap()->height() >= height()))) ? 1 : 0;
        style().drawItem(p, offset, offset, width(), height(), 
                         AlignCenter, colorGroup(),
                         true, pixmap(), QString::null);
    }
}

KWMThemeClient::KWMThemeClient( Workspace *ws, WId w, QWidget *parent,
                            const char *name )
    : Client( ws, w, parent, name, WResizeNoErase | WNorthWestGravity)
{
    stickyBtn = maxBtn = mnuBtn = 0;
    connect(options, SIGNAL(resetClients()), this, SLOT(slotReset())); 
    layout = new QGridLayout(this);
    layout->addColSpacing(0, maxExtent);
    layout->addColSpacing(2, maxExtent);

    layout->addRowSpacing(0, maxExtent);

    layout->addItem(new QSpacerItem(1, 1, QSizePolicy::Fixed,
                                    QSizePolicy::Expanding));

    layout->addWidget(windowWrapper(), 2, 1);
    // Without the next line, shading flickers
    layout->addItem( new QSpacerItem(0, 0, QSizePolicy::Fixed, QSizePolicy::Expanding) );
    layout->addRowSpacing(3, maxExtent);
    layout->setRowStretch(2, 10);
    layout->setColStretch(1, 10);
    
    QHBoxLayout* hb = new QHBoxLayout;
    layout->addLayout( hb, 1, 1 );

    KConfig *config = KGlobal::config();
    config->setGroup("Buttons");
    QString val;
    MyButton *btn;
    int i;
    static const char *defaultButtons[]={"Menu","Sticky","Off","Iconify",
        "Maximize","Close"};
    static const char keyOffsets[]={"ABCDEF"};
    for(i=0; i < 6; ++i){
        if(i == 3){
            titlebar = new QSpacerItem(10, 20, QSizePolicy::Expanding,
                               QSizePolicy::Minimum );
            hb->addItem( titlebar );
        }
        QString key("Button");
        key += QChar(keyOffsets[i]);
        val = config->readEntry(key, defaultButtons[i]);
        if(val == "Menu"){
            mnuBtn = new MyButton(this, "menu", i18n("Menu"));
            iconChange();
            hb->addWidget(mnuBtn);
            mnuBtn->setFixedSize(20, 20);
            connect(mnuBtn, SIGNAL(pressed()), this,
                    SLOT(menuButtonPressed()));
        }
        else if(val == "Sticky"){
            stickyBtn = new MyButton(this, "sticky", i18n("Sticky"));
            if (isSticky())
                stickyBtn->setPixmap(*pindownPix);
            else
                stickyBtn->setPixmap(*pinupPix);
            connect(stickyBtn, SIGNAL( clicked() ), this, SLOT(toggleSticky()));
            hb->addWidget(stickyBtn);
            stickyBtn->setFixedSize(20, 20);
        }
        else if((val == "Iconify") && isMinimizable()){
            btn = new MyButton(this, "iconify", i18n("Minimize"));
            btn->setPixmap(*iconifyPix);
            connect(btn, SIGNAL(clicked()), this, SLOT(iconify()));
            hb->addWidget(btn);
            btn->setFixedSize(20, 20);
        }
        else if((val == "Maximize") && isMaximizable()){
            maxBtn = new MyButton(this, "max", i18n("Maximize"));
            maxBtn->setPixmap(*maxPix);
            connect(maxBtn, SIGNAL(clicked()), this, SLOT(maximize()));
            hb->addWidget(maxBtn);
            maxBtn->setFixedSize(20, 20);
        }
        else if(val == "Close"){
            btn = new MyButton(this, "close", i18n("Close"));
            btn->setPixmap(*closePix);
            connect(btn, SIGNAL(clicked()), this, SLOT(closeWindow()));
            hb->addWidget(btn);
            btn->setFixedSize(20, 20);
        }
        else{
            if((val != "Off") && 
               ((val == "Iconify") && !isMinimizable()) &&
               ((val == "Maximize") && !isMaximizable()))
                qWarning("KWin: Unrecognized button value: %s", val.latin1());

        }
    }
    if(titleGradient){
        aGradient = new KPixmap;
        iGradient = new KPixmap;
    }
    else{
        aGradient = 0;
        iGradient = 0;
    }
    setBackgroundMode(NoBackground);
}

void KWMThemeClient::drawTitle(QPainter &dest)
{
    QRect titleRect = titlebar->geometry();
    QRect r(0, 0, titleRect.width(), titleRect.height());
    QPixmap buffer;

    if(buffer.width() == r.width())
        return;

    buffer.resize(r.size());
    QPainter p;
    p.begin(&buffer);

    if(titleSunken){
        qDrawShadeRect(&p, r, options->colorGroup(Options::Frame, isActive()),
                       true, 1, 0);
        r.setRect(r.x()+1, r.y()+1, r.width()-2, r.height()-2);
    }
    
    KPixmap *fill = isActive() ? aTitlePix : iTitlePix;
    if(fill)
        p.drawTiledPixmap(r, *fill);
    else if(titleGradient){
        fill = isActive() ? aGradient : iGradient;
        if(fill->width() != r.width()){
            fill->resize(r.width(), 20);
            KPixmapEffect::gradient(*fill,
                                    options->color(Options::TitleBar, isActive()),
                                    options->color(Options::TitleBlend, isActive()),
                                    grType);
        }
        p.drawTiledPixmap(r, *fill);
    }
    else{
        p.fillRect(r, options->colorGroup(Options::TitleBar, isActive()).
                   brush(QColorGroup::Button));
    }
    p.setFont(options->font(isActive()));
    p.setPen(options->color(Options::Font, isActive()));
    // Add left & right margin
    r.setLeft(r.left()+5);
    r.setRight(r.right()-5);
    p.drawText(r, titleAlign, caption());
    p.end();

    dest.drawPixmap(titleRect.x(), titleRect.y(), buffer);
}


void KWMThemeClient::resizeEvent( QResizeEvent* e)
{
    Client::resizeEvent( e );
    doShape();
    repaint();
}

void KWMThemeClient::captionChange( const QString& )
{
    repaint( titlebar->geometry(), false );
}

void KWMThemeClient::paintEvent( QPaintEvent *)
{
    QPainter p;
    p.begin(this);
    int x,y;
    // first the corners
    int w1 = framePixmaps[FrameTopLeft]->width();
    int h1 = framePixmaps[FrameTopLeft]->height();
    if (w1 > width()/2) w1 = width()/2;
    if (h1 > height()/2) h1 = height()/2;
    p.drawPixmap(0,0,*framePixmaps[FrameTopLeft],
		 0,0,w1, h1);
    int w2 = framePixmaps[FrameTopRight]->width();
    int h2 = framePixmaps[FrameTopRight]->height();
    if (w2 > width()/2) w2 = width()/2;
    if (h2 > height()/2) h2 = height()/2;
    p.drawPixmap(width()-w2,0,*framePixmaps[FrameTopRight],
		 framePixmaps[FrameTopRight]->width()-w2,0,w2, h2);

    int w3 = framePixmaps[FrameBottomLeft]->width();
    int h3 = framePixmaps[FrameBottomLeft]->height();
    if (w3 > width()/2) w3 = width()/2;
    if (h3 > height()/2) h3 = height()/2;
    p.drawPixmap(0,height()-h3,*framePixmaps[FrameBottomLeft],
		 0,framePixmaps[FrameBottomLeft]->height()-h3,w3, h3);

    int w4 = framePixmaps[FrameBottomRight]->width();
    int h4 = framePixmaps[FrameBottomRight]->height();
    if (w4 > width()/2) w4 = width()/2;
    if (h4 > height()/2) h4 = height()/2;
    p.drawPixmap(width()-w4,height()-h4,*(framePixmaps[FrameBottomRight]),
		 framePixmaps[FrameBottomRight]->width()-w4,
		 framePixmaps[FrameBottomRight]->height()-h4,
		 w4, h4);

    QPixmap pm;
    QWMatrix m;
    int n,s,w;
    //top
    pm = *framePixmaps[FrameTop];

    if (pm.width() > 0){
    s = width()-w2-w1;
    n = s/pm.width();
    w = n>0?s/n:s;
    m.reset();
    m.scale(w/(float)pm.width(), 1);
    pm = pm.xForm(m);

    x = w1;
    while (1){
      if (pm.width() < width()-w2-x){
	p.drawPixmap(x,maxExtent-pm.height()-1,
		     pm);
	x += pm.width();
      }
      else {
	p.drawPixmap(x,maxExtent-pm.height()-1,
		     pm,
		     0,0,width()-w2-x,pm.height());
	break;
      }
    }
    }

    //bottom
    pm = *framePixmaps[FrameBottom];

    if (pm.width() > 0){
    s = width()-w4-w3;
    n = s/pm.width();
    w = n>0?s/n:s;
    m.reset();
    m.scale(w/(float)pm.width(), 1);
    pm = pm.xForm(m);

    x = w3;
    while (1){
      if (pm.width() < width()-w4-x){
	p.drawPixmap(x,height()-maxExtent+1,pm);
	x += pm.width();
      }
      else {
	p.drawPixmap(x,height()-maxExtent+1,pm,
		     0,0,width()-w4-x,pm.height());
	break;
      }
    }
    }

    //left
    pm = *framePixmaps[FrameLeft];

    if (pm.height() > 0){
    s = height()-h3-h1;
    n = s/pm.height();
    w = n>0?s/n:s;
    m.reset();
    m.scale(1, w/(float)pm.height());
    pm = pm.xForm(m);

    y = h1;
    while (1){
      if (pm.height() < height()-h3-y){
	p.drawPixmap(maxExtent-pm.width()-1, y,
		     pm);
	y += pm.height();
      }
      else {
	p.drawPixmap(maxExtent-pm.width()-1, y,
		     pm,
		     0,0, pm.width(),
		     height()-h3-y);
	break;
      }
    }
    }

    //right
    pm = *framePixmaps[FrameRight];

    if (pm.height() > 0){
    s = height()-h4-h2;
    n = s/pm.height();
    w = n>0?s/n:s;
    m.reset();
    m.scale(1, w/(float)pm.height());
    pm = pm.xForm(m);

    y = h2;
    while (1){
      if (pm.height() < height()-h4-y){
	p.drawPixmap(width()-maxExtent+1, y,
		     pm);
	y += pm.height();
      }
      else {
	p.drawPixmap(width()-maxExtent+1, y,
		     pm,
		     0,0, pm.width(),
		     height()-h4-y);
	break;
      }
    }
    }
    drawTitle(p);

    QColor c = colorGroup().background();

    // KWM evidently had a 1 pixel border around the client window. We
    // emulate it here, but should be removed at some point in order to
    // seamlessly mesh widget themes
    p.setPen(c);
    p.drawRect(maxExtent-1, maxExtent-1, width()-(maxExtent-1)*2,
               height()-(maxExtent-1)*2);

    // We fill the area behind the wrapped widget to ensure that
    // shading animation is drawn as smoothly as possible
    QRect r(layout->cellGeometry(2, 1));
    p.fillRect( r.x(), r.y(), r.width(), r.height(), c);
    p.end();
}

void KWMThemeClient::doShape()
{

    QBitmap shapemask(width(), height());
    shapemask.fill(color0);
    QPainter p;
    p.begin(&shapemask);
    p.setBrush(color1);
    p.setPen(color1);
    int x,y;
    // first the corners
    int w1 = framePixmaps[FrameTopLeft]->width();
    int h1 = framePixmaps[FrameTopLeft]->height();
    if (w1 > width()/2) w1 = width()/2;
    if (h1 > height()/2) h1 = height()/2;
    p.drawPixmap(0,0,*framePixmaps[FrameTopLeft]->mask(),
		 0,0,w1, h1);
    int w2 = framePixmaps[FrameTopRight]->width();
    int h2 = framePixmaps[FrameTopRight]->height();
    if (w2 > width()/2) w2 = width()/2;
    if (h2 > height()/2) h2 = height()/2;
    p.drawPixmap(width()-w2,0,*framePixmaps[FrameTopRight]->mask(),
		 framePixmaps[FrameTopRight]->width()-w2,0,w2, h2);

    int w3 = framePixmaps[FrameBottomLeft]->width();
    int h3 = framePixmaps[FrameBottomLeft]->height();
    if (w3 > width()/2) w3 = width()/2;
    if (h3 > height()/2) h3 = height()/2;
    p.drawPixmap(0,height()-h3,*framePixmaps[FrameBottomLeft]->mask(),
		 0,framePixmaps[FrameBottomLeft]->height()-h3,w3, h3);

    int w4 = framePixmaps[FrameBottomRight]->width();
    int h4 = framePixmaps[FrameBottomRight]->height();
    if (w4 > width()/2) w4 = width()/2;
    if (h4 > height()/2) h4 = height()/2;
    p.drawPixmap(width()-w4,height()-h4,*framePixmaps[FrameBottomRight]->mask(),
		 framePixmaps[FrameBottomRight]->width()-w4,
		 framePixmaps[FrameBottomRight]->height()-h4,
		 w4, h4);

    QPixmap pm;
    QWMatrix m;
    int n,s,w;
    //top
    pm = *framePixmaps[FrameTop]->mask();

    s = width()-w2-w1;
    n = s/pm.width();
    w = n>0?s/n:s;
    m.reset();
    m.scale(w/(float)pm.width(), 1);
    pm = pm.xForm(m);

    x = w1;
    while (1){
      if (pm.width() < width()-w2-x){
	p.drawPixmap(x,maxExtent-pm.height()-1,
		     pm);
	x += pm.width();
      }
      else {
	p.drawPixmap(x,maxExtent-pm.height()-1,
		     pm,
		     0,0,width()-w2-x,pm.height());
	break;
      }
    }

    //bottom
    pm = *framePixmaps[FrameBottom]->mask();

    s = width()-w4-w3;
    n = s/pm.width();
    w = n>0?s/n:s;
    m.reset();
    m.scale(w/(float)pm.width(), 1);
    pm = pm.xForm(m);

    x = w3;
    while (1){
      if (pm.width() < width()-w4-x){
	p.drawPixmap(x,height()-maxExtent+1,pm);
	x += pm.width();
      }
      else {
	p.drawPixmap(x,height()-maxExtent+1,pm,
		     0,0,width()-w4-x,pm.height());
	break;
      }
    }

    //left
    pm = *framePixmaps[FrameLeft]->mask();

    s = height()-h3-h1;
    n = s/pm.height();
    w = n>0?s/n:s;
    m.reset();
    m.scale(1, w/(float)pm.height());
    pm = pm.xForm(m);

    y = h1;
    while (1){
      if (pm.height() < height()-h3-y){
	p.drawPixmap(maxExtent-pm.width()-1, y,
		     pm);
	y += pm.height();
      }
      else {
	p.drawPixmap(maxExtent-pm.width()-1, y,
		     pm,
		     0,0, pm.width(),
		     height()-h3-y);
	break;
      }
    }

    //right
    pm = *framePixmaps[FrameRight]->mask();

    s = height()-h4-h2;
    n = s/pm.height();
    w = n>0?s/n:s;
    m.reset();
    m.scale(1, w/(float)pm.height());
    pm = pm.xForm(m);

    y = h2;
    while (1){
      if (pm.height() < height()-h4-y){
	p.drawPixmap(width()-maxExtent+1, y,
		     pm);
	y += pm.height();
      }
      else {
	p.drawPixmap(width()-maxExtent+1, y,
		     pm,
		     0,0, pm.width(),
		     height()-h4-y);
	break;
      }
    }
    p.fillRect(maxExtent-1, maxExtent-1, width()-2*maxExtent+2, height()-2*maxExtent+2, color1);
    setMask(shapemask);
}


void KWMThemeClient::showEvent(QShowEvent *ev)
{
    Client::showEvent(ev);
    doShape();
    repaint(false);
}

void KWMThemeClient::windowWrapperShowEvent( QShowEvent* )
{
    doShape();
}                                                                               

void KWMThemeClient::mouseDoubleClickEvent( QMouseEvent * e )
{
    if (titlebar->geometry().contains( e->pos() ) )
        setShade( !isShade() );
}

void KWMThemeClient::stickyChange(bool on)
{
    if (stickyBtn) {
       stickyBtn->setPixmap(on ? *pindownPix : *pinupPix);
       stickyBtn->setTipText(on ? i18n("Un-Sticky") : i18n("Sticky") );
    }
}

void KWMThemeClient::maximizeChange(bool m)
{
    if (maxBtn) {
       maxBtn->setPixmap(m ? *minmaxPix : *maxPix);
       maxBtn->setTipText(m ? i18n("Restore") : i18n("Maximize"));
    }
}

Client::MousePosition KWMThemeClient::mousePosition(const QPoint &p) const
{
    MousePosition m = Client::mousePosition(p);
    // corners
    if(p.y() < framePixmaps[FrameTop]->height() &&
       p.x() < framePixmaps[FrameLeft]->width()){
        m = TopLeft;
    }
    else if(p.y() < framePixmaps[FrameTop]->height() &&
            p.x() > width()-framePixmaps[FrameRight]->width()){
        m = TopRight;
    }
    else if(p.y() > height()-framePixmaps[FrameBottom]->height() &&
            p.x() < framePixmaps[FrameLeft]->width()){
        m = BottomLeft;
    }
    else if(p.y() > height()-framePixmaps[FrameBottom]->height() &&
            p.x() > width()-framePixmaps[FrameRight]->width()){
        m = BottomRight;
    } // edges
    else if(p.y() < framePixmaps[FrameTop]->height())
        m = Top;
    else if(p.y() > height()-framePixmaps[FrameBottom]->height())
        m = Bottom;
    else if(p.x()  < framePixmaps[FrameLeft]->width())
        m = Left;
    else if(p.x() > width()-framePixmaps[FrameRight]->width())
        m = Right;
    return(m);
}

void KWMThemeClient::menuButtonPressed()
{
    static QTime* t = 0;
    static KWMThemeClient* tc = 0;
    if ( !t )
        t = new QTime;

    if ( tc != this || t->elapsed() > QApplication::doubleClickInterval() ){
        mnuBtn->setDown(false); // will stay down if I don't do this
        workspace()->clientPopup(this)->
            popup(mnuBtn->mapToGlobal(mnuBtn->rect().bottomLeft()));
    }
    else {
        mnuBtn->setPopup( 0 );
        closeWindow();
    }
    t->start();
    tc = this;
}

void KWMThemeClient::iconChange()
{
    if(mnuBtn){
        if(miniIcon().isNull()){
            mnuBtn->setPixmap(*menuPix);
        }
        else{
            mnuBtn->setPixmap(miniIcon());
        }
    }
}



void KWMThemeClient::init()
{
    //
}

extern "C"
{
    Client *allocate(Workspace *ws, WId w)
    {
        return(new KWMThemeClient(ws, w));
    }
    void init()
    {
       create_pixmaps();
    }
    void reset()
    {
       delete_pixmaps();
       create_pixmaps();
    }
    void deinit()
    {
       delete_pixmaps();
    }
}

#include "kwmthemeclient.moc"
