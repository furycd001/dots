// $Id: ksplash.cpp,v 1.30.2.1 2001/08/17 18:42:36 waba Exp $

#include <stdlib.h>
#include <unistd.h>

#include <qpixmap.h>
#include <qtimer.h>
#include <qapplication.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpainter.h>

#include <ksplash.h>
#include <kapp.h>
#include <dcopclient.h>
#include <kstddirs.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>
#include <kprogress.h>
#include <kwin.h>
#include <kdesktopwidget.h>

#include "ksplash.h"
#include "ksplash.moc"

static const char *version = "0.3";
static const char *description = I18N_NOOP( "The KDE Splash Screen." );

static KCmdLineOptions options[] =
{
  { "test",   "Run in test mode.", 0 },
  { 0, 0, 0}
};

KSplash::KSplash( const char* name )
    : QVBox( 0, name, WStyle_Customize | WStyle_StaysOnTop |
	     WStyle_NoBorderEx ), DCOPObject( name )
{
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    
    testmode = args->isSet("test");

    args->clear();

    KWin::setState( winId(), NET::StaysOnTop | NET::SkipTaskbar | NET::SkipPager );
    setFrameStyle( QFrame::NoFrame );

    QString resource_prefix = "ksplash/pics/";
    if (QPixmap::defaultDepth() <= 8)  
    	resource_prefix = "ksplash/pics/locolor/";

    bar_active_pm = new QPixmap( locate("data", resource_prefix + "splash_active_bar.png") );
    bar_inactive_pm = new QPixmap( locate("data", resource_prefix + "splash_inactive_bar.png") );
    bar_blink1 = new QPixmap();
    bar_blink2 = new QPixmap();

    top_label = new QLabel( this );
    top_label->installEventFilter( this );
    top_label->setPixmap(QPixmap(locate("data", resource_prefix + "splash_top.png")));

    bar_label = new QLabel( this );
    bar_label->setPixmap(*bar_inactive_pm);
    bar_label->setBackgroundMode(NoBackground);

    bottom_label = new QLabel( this );
    bottom_label->installEventFilter( this );
    bottom_label->setPalette(QPalette(black));
    bottom_label->setPixmap(QPixmap(locate("data", resource_prefix + "splash_bottom.png")));

    status_label = new QLabel(bottom_label);
    status_label->installEventFilter( this );
    status_label->setAutoMask( true );

    progress = new KProgress(status_label);
    progress->installEventFilter( this );
    progress->hide();

    blink_timer = new QTimer( this );
    connect( blink_timer, SIGNAL( timeout() ), this, SLOT( blink() ) );

    state = 0;
    updateState();
    bottom_label->setFixedHeight(status_label->sizeHint().height()+4);

    QSize sh = sizeHint();
    KDesktopWidget *desktop = KApplication::desktop();
    QRect rect = desktop->screenGeometry(desktop->primaryScreen());
    move(rect.x() + (rect.width() - sh.width())/2,
         rect.y() + (rect.height() - sh.height())/2);

    setFixedSize(sh);

    close_timer = new QTimer( this );
    connect( close_timer, SIGNAL( timeout() ), this, SLOT( close() ) );
    close_timer->start( 60000, TRUE );

    QTimer::singleShot( 250, this, SLOT( tryDcop() ) );
}

void KSplash::upAndRunning( QString s )
{
    bool update = true;
    if ( close_timer->isActive() )
	close_timer->start( 60000, TRUE );

    if ( s == "dcop" ) {
	    if (state > 1) return;
	    else state = 1;
	}
    else if ( s == "kded" ) {
	    if (state > 2) return;
	    else state = 2;
	}
    else if ( s == "kcminit2" ) {
	    if (state > 3) return;
	    else state = 3;
	}
    else if ( s == "wm started" ) {
	    if (state > 4) return;
	    else state = 4;
	}
    else if ( s == "kdesktop" ) {
	    if (state > 5) return;
	    else state = 5;
	}
    else if ( s == "kicker" ) {
	    if (state > 6) return;
	    else state = 6;
	}
    else if ( s == "session ready" ) {
        if (state > 8) return;
        else state = 8;
	    close_timer->stop();
        QTimer::singleShot(1000, this, SLOT( close() ) );
	}
    else
	update = false;

    if(update)
	updateState();
}

void KSplash::autoMode()
{
   state++;
   if (state == 9)
   {
      close();
      return;
   }
   updateState();
   QTimer::singleShot(2000, this, SLOT(autoMode()));
}

void KSplash::setMaxProgress(int m)
{
    if (m < 1)
       m = 1;
    progress->setRange(0, m );
}

void KSplash::setProgress(int p)
{
    progress->setValue( progress->maxValue() - p );
}

void KSplash::tryDcop()
{
    disconnect( kapp->dcopClient(), SIGNAL( attachFailed(const QString&) ),
		kapp, SLOT( dcopFailure(const QString&) ) );

    if ( kapp->dcopClient()->isAttached() )
	return;

    if ( kapp->dcopClient()->attach() ) {
	upAndRunning( "dcop" );
	kapp->dcopClient()->registerAs( "ksplash", false );
	kapp->dcopClient()->setDefaultObject( name() );
        if (testmode) 
           QTimer::singleShot(1000, this, SLOT(autoMode()));
    }
    else {
	QTimer::singleShot(100, this, SLOT( tryDcop() ) );
    }
}


int main(int argc, char* argv[])
{
    KAboutData aboutData( "ksplash", I18N_NOOP("The KDE Splash Screen"),
			  version, description, KAboutData::License_BSD,
			  "(C) 2000,2001, The KDE Developers");
    aboutData.addAuthor("Matthias Ettrich",0, "ettrich@kde.org");
    aboutData.addAuthor("Matthias Elter",0, "elter@kde.org");

    KCmdLineArgs::init(argc, argv, &aboutData);
    KCmdLineArgs::addCmdLineOptions(options);
    (void) KCmdLineArgs::parsedArgs(); // Handle cmd line args.

    if (fork())
	exit(0);

    KApplication a(false, true);
    KSplash ksplash( "ksplash" );
    ksplash.show();

    a.setTopWidget( &ksplash );
    a.setMainWidget( &ksplash );
    return a.exec();
}

bool KSplash::eventFilter( QObject *, QEvent * e )
{
    if ( e->type() == QEvent::MouseButtonRelease ) {
	if ( geometry().contains( ( (QMouseEvent*)e)->globalPos() ) ) {
	    close();
	    return TRUE;
	}
    }
    return FALSE;
}

void KSplash::resizeEvent(QResizeEvent*)
{
    status_label->resize(bottom_label->width() - 7, bottom_label->height() - 4);
    status_label->move(5, 0);
    progress->resize(120, status_label->height());
    progress->move(status_label->width() - progress->width() - 4, 0);
}

void KSplash::updateState()
{
    QString text;
    switch(state) {
    case 0:
	text = i18n("Setting up interprocess communication.");
	break;
    case 1:
	text = i18n("Initializing system services.");
	break;
    case 2:
	text = i18n("Initializing peripherals.");
	break;
    case 3:
	text = i18n("Loading the window manager.");
	break;
    case 4:
        text = i18n("Loading the desktop.");
        break;
    case 5:
        text = i18n("Loading the panel.");
        break;
    case 6:
	text = i18n("Restoring session...");
        progress->show();
	break;
    default:
	text = i18n("KDE is up and running.");
	break;
    }
    status_label->setText(text);

    *bar_blink1 = makePixmap(state);
    *bar_blink2 = makePixmap(state+1);
    bar_label->setPixmap(*bar_blink2);
    blink_timer->stop();
    if (state < 7)
       blink_timer->start(400);
}

QPixmap KSplash::makePixmap(int _state)
{
    int offset = _state * 58;
    if (_state == 3)
       offset += 8;
    if (_state == 6)
       offset -=8;
    QPixmap pm(*bar_active_pm);
    QPainter p(&pm);
    p.drawPixmap(offset, 0, *bar_inactive_pm, offset, 0);

    return pm;
}

void KSplash::blink()
{
    QPixmap *tmp = bar_blink1;
    bar_blink1 = bar_blink2;
    bar_blink2 = tmp;
    bar_label->setPixmap(*bar_blink2);
}
