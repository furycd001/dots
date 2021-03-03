/*****************************************************************
ksmserver - the KDE session management server

Copyright (C) 2000 Matthias Ettrich <ettrich@kde.org>
******************************************************************/

#include <config.h>

#include "shutdown.h"
#include <qapplication.h>
#include <qlayout.h>
#include <qgroupbox.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qvbox.h>
#include <qpainter.h>
#include <qtimer.h>

#include <klocale.h>
#include <kapp.h>
#include <kdebug.h>
#include <kwin.h>

#include <X11/Xlib.h>

#include "shutdown.moc"

KSMShutdownFeedback * KSMShutdownFeedback::s_pSelf = 0L;

KSMShutdownFeedback::KSMShutdownFeedback()
 : QWidget( 0L )
{
    setBackgroundMode( QWidget::NoBackground );
    setGeometry( QApplication::desktop()->geometry() );
    showFullScreen();
    KWin::setState( winId(), NET::StaysOnTop );
    KWin::setOnAllDesktops( winId(), TRUE );

    QPainter p;
    QBrush b( Qt::Dense4Pattern );
    p.begin( this );
    p.fillRect( rect(), b);
    p.end();
}

void KSMShutdownFeedback::start() //static
{
    (void) self();
}

void KSMShutdownFeedback::keyPressEvent( QKeyEvent * event )
{
    if ( event->key() == Key_Escape )
    {
        kdDebug() << "Esc pressed -> aborting shutdown" << endl;
        emit aborted();
    }
    else
        QWidget::keyPressEvent( event );
}

//////

KSMShutdownDlg::KSMShutdownDlg( QWidget* parent )
    : QDialog( parent, 0, TRUE, WStyle_Customize | WStyle_NoBorderEx | WStyle_StaysOnTop ) //WType_Popup )
{
    QVBoxLayout* vbox = new QVBoxLayout( this );
    QFrame* frame = new QFrame( this );
    frame->setFrameStyle( QFrame::StyledPanel | QFrame::Raised );
    frame->setLineWidth( style().defaultFrameWidth() );
    vbox->addWidget( frame );
    vbox = new QVBoxLayout( frame, 15, 5 );

    QLabel* label = new QLabel(i18n(
         "<center><b><big><big>End KDE Session?</big></big></b></center>"),
	 frame );
    vbox->addWidget( label );
    vbox->addStretch();

    checkbox = new QCheckBox( i18n("&Restore session when logging in next time"), frame );
    vbox->addWidget( checkbox, 0, AlignRight  );
    vbox->addStretch();

    QHBoxLayout* hbox = new QHBoxLayout( vbox );
    hbox->addStretch();
    QPushButton* yes = new QPushButton(i18n("&Logout"), frame );
    connect( yes, SIGNAL( clicked() ), this, SLOT( accept() ) );
    yes->setDefault( TRUE );
    hbox->addWidget( yes );
    QPushButton* cancel = new QPushButton(i18n("&Cancel"), frame );
    connect( cancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    hbox->addWidget( cancel );

    QTimer::singleShot( 0, this, SLOT( requestFocus() ) );
    checkbox->setFocus();
}

void KSMShutdownDlg::requestFocus()
{
    XSetInputFocus( qt_xdisplay(), winId(), RevertToParent, CurrentTime );
}

bool KSMShutdownDlg::confirmShutdown( bool& saveSession )
{
    kapp->enableStyles();
    KSMShutdownDlg* l = new KSMShutdownDlg( KSMShutdownFeedback::self() );
    l->checkbox->setChecked( saveSession );

    // Show dialog (will save the background in showEvent)
    QSize sh = l->sizeHint();
    KDesktopWidget *desktop = KApplication::desktop();
    QRect rect = desktop->screenGeometry(desktop->screenNumber(QCursor::pos()));
    l->move(rect.x() + (rect.width() - sh.width())/2,
    	    rect.y() + (rect.height() - sh.height())/2);
    l->show();
    saveSession = l->checkbox->isChecked();
    bool result = l->result();
    l->hide();

    // Restore background
    bitBlt( KSMShutdownFeedback::self(), l->x(), l->y(), &l->pixmap() );

    delete l;

    kapp->disableStyles();
    return result;
}

void KSMShutdownDlg::showEvent( QShowEvent * )
{
    // Save background
    //kdDebug() << "showEvent => grabWindow "
    //          << x() << "," << y() << " " << width() << "x" << height() << endl;
    pm = QPixmap::grabWindow( KSMShutdownFeedback::self()->winId(),
                              x(), y(), width(), height() );
}

