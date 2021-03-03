    /*

    Greeter module for xdm
    $Id: kgreeter.cpp,v 1.53 2001/07/30 00:54:23 ossi Exp $

    Copyright (C) 1997, 1998, 2000 Steffen Hansen <hansen@kde.org>
    Copyright (C) 2000, 2001 Oswald Buddenhagen <ossi@kde.org>


    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    */

#include <config.h>

#include <qfile.h>
#include <qbitmap.h>
#include <qtextstream.h>
#include <qpopupmenu.h>
#include <qtimer.h>
#include <qcombobox.h>
#include <qstring.h>
#include <qaccel.h>

#include <klocale.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <kmessagebox.h>
#include <kcmdlineargs.h>
#include <kseparator.h>
#include <kapp.h>

#include "kgreeter.h"
#include "kdmconfig.h"
#include "kdmclock.h"
#include "kfdialog.h"
#include "kdm_greet.h"

extern "C" {
#ifdef HAVE_XKB
// note: some XKBlib.h versions contain a global variable definition
// called "explicit". This keyword is not allowed on some C++ compilers so ->
# define explicit __explicit_dummy
# include <X11/XKBlib.h>
#endif
#ifdef HAVE_XINERAMA
# include <X11/extensions/Xinerama.h>
#endif
};

#include <sys/param.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>

KGreeter *kgreeter = 0;

void
KLoginLineEdit::focusOutEvent( QFocusEvent *e)
{
    emit lost_focus();
    QLineEdit::focusOutEvent( e);
}

class MyApp : public KApplication {

public:
    MyApp(int& argc, char** argv) : KApplication(argc, argv, "kdmgreet") {};
    virtual bool x11EventFilter( XEvent * );
};

bool
MyApp::x11EventFilter( XEvent * ev)
{
    QWidget *target;
    if( ev->type == ConfigureNotify) {
	// Hack to tell dialogs to take focus
	target = QWidget::find( ev->xconfigure.window);
	if (target) {
	    target = target->topLevelWidget();
	    if( target->isVisible() && !target->isPopup())
		XSetInputFocus( qt_xdisplay(), target->winId(),
				RevertToParent, CurrentTime);
	}
    } else if (ev->type == FocusIn || ev->type == FocusOut) {
	// Hack to tell dialogs to take focus when the keyboard is grabbed
	if (ev->xfocus.mode == NotifyWhileGrabbed)
	    ev->xfocus.mode = NotifyNormal;
    } else if (ev->type == ButtonPress || ev->type == ButtonRelease) {
	// Hack to let the RMB work (nearly) as LMB
	if (ev->xbutton.button == 3)
	    ev->xbutton.button = 1;
    }
    return false;
}

void KGreeter::keyPressEvent( QKeyEvent *e )
{
    if ( e->state() == 0)
	switch ( e->key() ) {
	    case Key_Enter:
	    case Key_Return:
		ReturnPressed();
		return;
	    case Key_Escape:
		clearButton->animateClick();
		return;
	}
    else if ( e->state() & Keypad && e->key() == Key_Enter ) {
	ReturnPressed();
	return;
    } else if ( !(~e->state() & (AltButton | ControlButton)) &&
	        e->key() == Key_Delete && 
		kdmcfg->_allowShutdown != SHUT_NONE) {
	shutdown_button_clicked();
	return;
    }
    e->ignore();
}

#define CHECK_STRING( x) (x != 0 && x[0] != 0)

void
KGreeter::insertUsers( QIconView *iconview)
{
    QPixmap default_pix( locate("user_pic", QString::fromLatin1("default.png")));
    if( default_pix.isNull())
	LogError("Can't open default pixmap \"default.png\"\n");
    if( kdmcfg->_showUsers == SHOW_ALL ) {
	struct passwd *ps;
	for( setpwent(); (ps = getpwent()) != 0; ) {
	    // usernames are stored in the same encoding as files
	    QString username = QFile::decodeName ( ps->pw_name );
	    if( CHECK_STRING(ps->pw_dir) &&
		CHECK_STRING(ps->pw_shell) &&
		(ps->pw_uid >= (unsigned)kdmcfg->_lowUserId || 
		 username == "root") &&
		ps->pw_uid <= (unsigned)kdmcfg->_highUserId &&
        	!kdmcfg->_noUsers.contains( username )
	    ) {
		// we might have a real user, insert him/her
		QPixmap p( locate("user_pic",
				  username + QString::fromLatin1(".png")));
		if( p.isNull())
		    p = default_pix;
		QIconViewItem *item = new QIconViewItem( iconview,
							     username, p);
		item->setDragEnabled(false);
	    }
	}
	endpwent();
    } else {
	QStringList::ConstIterator it = kdmcfg->_users.begin();
	for( ; it != kdmcfg->_users.end(); ++it) {
	    QPixmap p( locate("user_pic",
			      *it + QString::fromLatin1(".png")));
	    if( p.isNull())
		p = default_pix;
	    QIconViewItem *item = new QIconViewItem( iconview, *it, p);
	    item->setDragEnabled(false);
	}
    }
    if( kdmcfg->_sortUsers)
        iconview->sort();
}

#undef CHECK_STRING

static void
inserten (QPopupMenu *mnu, const QString& txt,
	  const QObject *receiver, const char *member)
{
    mnu->insertItem(txt, receiver, member, QAccel::shortcutKey(txt));
}

KGreeter::KGreeter(QWidget *parent, const char *t)
  : QFrame( parent, t, WStyle_Customize | WStyle_NoBorder | WStyle_Tool)
{
    setFrameStyle (QFrame::WinPanel | QFrame::Raised);
    QBoxLayout* vbox = new QBoxLayout(  this,
					QBoxLayout::TopToBottom,
					10, 10);
    QBoxLayout* hbox1 = new QBoxLayout( QBoxLayout::LeftToRight, 10);
    QBoxLayout* hbox2 = new QBoxLayout( QBoxLayout::LeftToRight, 10);

    QGridLayout* grid = new QGridLayout( 5, 4, 5);

    if (!kdmcfg->_greetString.isEmpty()) {
	QLabel* welcomeLabel = new QLabel( kdmcfg->_greetString, this);
	welcomeLabel->setAlignment(AlignCenter);
	welcomeLabel->setFont( kdmcfg->_greetFont);
	vbox->addWidget( welcomeLabel);
    }
    if( kdmcfg->_showUsers != SHOW_NONE) {
	user_view = new QIconView( this);
	user_view->setSelectionMode( QIconView::Single );
	user_view->setArrangement( QIconView::LeftToRight);
	user_view->setAutoArrange(true);
	user_view->setItemsMovable(false);
	user_view->setResizeMode(QIconView::Adjust);
	insertUsers( user_view);
	vbox->addWidget( user_view);
    } else {
	user_view = NULL;
    }

    pixLabel = 0;
    clock    = 0;

    switch( kdmcfg->_logoArea ) {
	case LOGO_CLOCK:
	    clock = new KdmClock( this, "clock" );
	    break;
	case LOGO_LOGO:
	    {
		QPixmap pixmap;
		if ( pixmap.load( kdmcfg->_logo ) ) {
		    pixLabel = new QLabel( this);
		    pixLabel->setFrameStyle( QFrame::Panel | QFrame::Sunken);
		    pixLabel->setAutoResize( true);
		    pixLabel->setIndent(0);
		    pixLabel->setPixmap( pixmap);
		}
	    }
	    break;
    }

    loginEdit = new KLoginLineEdit( this);
    loginLabel = new QLabel( loginEdit, i18n("&Login:"), this);

    passwdEdit = new KPasswordEdit( this, "edit", kdmcfg->_echoMode);
    passwdLabel = new QLabel( passwdEdit, i18n("&Password:"), this);

    sessargBox = new QComboBox( false, this);
    sessargLabel = new QLabel( sessargBox, i18n("Session &Type:"), this);
    sessargBox->insertStringList( kdmcfg->_sessionTypes );
    sessargStat = new QWidget( this);
    sasPrev = new QLabel( i18n("session type", "(previous)"), sessargStat);
    sasSel = new QLabel( i18n("session type", "(selected)"), sessargStat);
    sessargStat->setFixedSize(
	QMAX(sasPrev->sizeHint().width(), sasSel->sizeHint().width()),
	sessargBox->height());

    vbox->addLayout( hbox1);
    vbox->addLayout( hbox2);
    hbox1->addLayout( grid, 3);
    if (clock)
	hbox1->addWidget( (QWidget*)clock, 0, AlignTop);
    else if (pixLabel)
	hbox1->addWidget( (QWidget*)pixLabel, 0, AlignTop);

    KSeparator* sep = new KSeparator( KSeparator::HLine, this);

    failedLabel = new QLabel( this);
    failedLabel->setFont( kdmcfg->_failFont);

    grid->addWidget( loginLabel, 0, 0);
    grid->addMultiCellWidget( loginEdit, 0,0, 1,3);
    grid->addWidget( passwdLabel, 1, 0);
    grid->addMultiCellWidget( passwdEdit, 1,1, 1,3);
    grid->addWidget( sessargLabel, 2, 0);
    grid->addWidget( sessargBox, 2, 1);
    grid->addWidget( sessargStat, 2, 2);
    grid->addMultiCellWidget( failedLabel, 3,3, 0,3, AlignCenter);
    grid->addMultiCellWidget( sep, 4,4, 0,3);
    grid->setColStretch( 3, 1);

    goButton = new QPushButton( i18n("G&o!"), this);
    goButton->setFixedWidth(goButton->sizeHint().width());
    goButton->setDefault( true);
    connect( goButton, SIGNAL( clicked()), SLOT(go_button_clicked()));
    hbox2->addWidget( goButton);

    clearButton = new QPushButton( i18n("&Clear"), this);
    connect( clearButton, SIGNAL(clicked()), SLOT(clear_button_clicked()));
    hbox2->addWidget( clearButton);

    hbox2->addStretch( 1);

    optMenu = new QPopupMenu(this);
    optMenu->setCheckable(false);

    if (dhasConsole)
	inserten (optMenu, i18n("Co&nsole Login"),
		  this, SLOT(console_button_clicked()));

/*    inserten (optMenu, i18n("&Remote Login"),
	      this, SLOT(chooser_button_clicked()));
*/
    inserten (optMenu, disLocal ?
		       i18n("R&estart X Server") :
		       i18n("Clos&e Connection"),
	      this, SLOT(quit_button_clicked()));

    menuButton = new QPushButton( i18n("&Menu"), this);
    menuButton->setPopup(optMenu);
    hbox2->addWidget( menuButton);

    hbox2->addStretch( 1);

    int sbw = 0;
    if (kdmcfg->_allowShutdown != SHUT_NONE)
    {
	shutdownButton = new QPushButton(i18n("&Shutdown..."), this);
	connect( shutdownButton, SIGNAL(clicked()),
		 SLOT(shutdown_button_clicked()));
	hbox2->addWidget( shutdownButton);
	sbw = shutdownButton->width();
    }

    timer = new QTimer( this );
    // clear fields
    connect( timer, SIGNAL(timeout()), SLOT(timerDone()) );
    // update session type
    connect( loginEdit, SIGNAL(lost_focus()), SLOT( load_wm()));
    // start login timeout after entered login
    connect( loginEdit, SIGNAL(lost_focus()), SLOT( SetTimer()));
    // update sessargStat
    connect( sessargBox, SIGNAL(activated(int)),
	     SLOT(slot_session_selected()));
    if( user_view) {
	connect( user_view, SIGNAL(returnPressed(QIconViewItem*)),
		 SLOT(slot_user_name( QIconViewItem*)));
	connect( user_view, SIGNAL(clicked(QIconViewItem*)),
		 SLOT(slot_user_name( QIconViewItem*)));
    }

    clear_button_clicked();

    stsfile = new KSimpleConfig (QString::fromLatin1 (KDE_CONFDIR "/kdm/kdmsts"));
    stsfile->setGroup ("PrevUser");
    enam = QString::fromLocal8Bit(dname);
    if (kdmcfg->_preselUser != PRESEL_PREV)
	stsfile->deleteEntry (enam, false);
    if (kdmcfg->_preselUser != PRESEL_NONE) {
	if (kdmcfg->_preselUser == PRESEL_PREV) {
	    loginEdit->setText (stsfile->readEntry (enam));
	} else
	    loginEdit->setText (kdmcfg->_defaultUser);
	if (kdmcfg->_focusPasswd && !loginEdit->text().isEmpty())
	    passwdEdit->setFocus();
	else
	    loginEdit->selectAll();
	load_wm();
    }
}

KGreeter::~KGreeter ()
{
    delete stsfile;
}

void
KGreeter::slot_user_name( QIconViewItem *item)
{
    if( item != 0) {
	loginEdit->setText( item->text());
	passwdEdit->erase();
	passwdEdit->setFocus();
	load_wm();
	SetTimer();
    }
}

void
KGreeter::slot_session_selected()
{
    wmstat = WmSel;
    sasSel->show();
    sasPrev->hide();
}

void
KGreeter::SetTimer()
{
    if (failedLabel->text().isNull())
	timer->start( 40000, TRUE );
}

void
KGreeter::timerDone()
{
    if (!failedLabel->text().isNull()) {
	failedLabel->setText(QString::null);
	goButton->setEnabled( true);
	loginEdit->setEnabled( true);
	passwdEdit->setEnabled( true);
	sessargBox->setEnabled( true);
    }
    clear_button_clicked();
}

void
KGreeter::clear_button_clicked()
{
    loginEdit->clear();
    passwdEdit->erase();
    loginEdit->setFocus();
    sasPrev->hide();
    sasSel->hide();
    wmstat = WmNone;
    set_wm( "default");
}

void
KGreeter::quit_button_clicked()
{
    SessionExit (EX_RESERVER_DPY);
}

void
KGreeter::chooser_button_clicked()
{
//    qApp->exit( ex_choose );
}

void
KGreeter::console_button_clicked()
{
    SessionExit (EX_TEXTLOGIN);
}

void
KGreeter::shutdown_button_clicked()
{
    KDMShutdown k( this);
    k.exec();
}

void
KGreeter::set_wm(const char *cwm)
{
    QString wm = QString::fromLocal8Bit (cwm);
    for (int i = sessargBox->count(); i--;)
	if (sessargBox->text(i) == wm) {
	    sessargBox->setCurrentItem(i);
	    return;
	}
}

void
KGreeter::load_wm()
{
    int sum, len, i, num, dummy;
    QCString name;
    char **ptr;

    if (wmstat == WmSel)
	return;

    name = loginEdit->text().local8Bit();
    if (!(len = name.length())) {
	wmstat = WmNone;
	sasPrev->hide();
    } else {
	wmstat = WmPrev;
	sasPrev->show();
	GSendInt (G_GetSessArg);
	GSendStr (name.data());
	ptr = GRecvStrArr (&dummy);
	if (!ptr) {		/* no such user */
	    /* XXX - voodoo */
	    for (sum = 0, i = 0; i < len; i++)
		sum += (int)name[i] << ((i ^ sum) & 7);
	    sum ^= (sum >> 7);
	    /* forge a session with this hash - default more probable */
	    num = sessargBox->count();
	    i = sum % (num * 4 / 3);
	    if (i < num) {
		sessargBox->setCurrentItem(i);
		return;
	    }
	} else if (!ptr[0]) {	/* cannot read */
	    free (ptr);
	} else {
	    set_wm (ptr[0]);
	    for (i = 0; ptr[i]; i++)
		free (ptr[i]);
	    free (ptr);
	    return;
	}
    }
    set_wm ("default");
}


#define errorbox QMessageBox::Critical
#define sorrybox QMessageBox::Warning
#define infobox QMessageBox::Information

void
KGreeter::MsgBox(QMessageBox::Icon typ, QString msg)
{
    KFMsgBox::box(kgreeter, typ, msg);
}

bool
KGreeter::verifyUser(bool haveto)
{
    int ret, expire;
    char *msg;

    GSendInt (G_Verify);
    GSendStr (loginEdit->text().local8Bit());
    GSendStr (passwdEdit->password());
    ret = GRecvInt ();
    if (ret == V_OK) {
	GSendInt (G_Restrict);
	GSendStr (loginEdit->text().local8Bit());
	ret = GRecvInt ();
    }
    switch (ret) {
	case V_ERROR:
	    MsgBox (errorbox, i18n("A critical error occurred.\n"
			"Please look at KDM's logfile for more information\n"
			"or contact your system administrator."));
	    break;
	case V_NOHOME:
	    MsgBox (sorrybox, i18n("Home directory not available."));
	    break;
	case V_NOROOT:
	    MsgBox (sorrybox, i18n("Root logins are not allowed."));
	    break;
	case V_BADSHELL:
	    MsgBox (sorrybox, 
		    i18n("Your login shell is not listed in /etc/shells."));
	    break;
	case V_AEXPIRED:
	    MsgBox (sorrybox, i18n("Your account has expired."));
	    break;
	case V_PEXPIRED:
	    MsgBox (sorrybox, i18n("Your password has expired."));
	    break;
	case V_BADTIME:
	    MsgBox (sorrybox, i18n("You are not allowed to login\n"
					"at the moment."));
	    break;
	case V_NOLOGIN:
	    msg = GRecvStr();
	    {
		QFile f;
		f.setName(QString::fromLocal8Bit(msg));
		f.open(IO_ReadOnly);
	        QTextStream t( &f );
		QString mesg;
		while ( !t.eof() )
		    mesg += t.readLine() + '\n';
		f.close();

		if (mesg.isEmpty())
		    MsgBox (sorrybox, 
			    i18n("Logins are not allowed at the moment.\n"
					"Try again later."));
		else
		    MsgBox (sorrybox, mesg);
	    }
	    free (msg);
	    break;
	case V_MSGERR:
	    msg = GRecvStr ();
	    MsgBox (sorrybox, QString::fromLocal8Bit (msg));
	    free (msg);
	    break;
	case V_AUTH:
	    if (!haveto)
		return false;
	    failedLabel->setText(i18n("Login failed"));
	    goButton->setEnabled( false);
	    loginEdit->setEnabled( false);
	    passwdEdit->setEnabled( false);
	    sessargBox->setEnabled( false);
	    timer->start( 1500 + kapp->random()/(RAND_MAX/1000), true );
	    return true;
	default:
	    switch (ret) {
	    default:
		LogPanic ("Unknown V_xxx code %d from core\n", ret);
	    case V_MSGINFO:
		msg = GRecvStr ();
		MsgBox (infobox, QString::fromLocal8Bit (msg));
		free (msg);
		break;
	    case V_AWEXPIRE:
		expire = GRecvInt ();
		if (expire == 1)
		    MsgBox (infobox, i18n("Your account expires tomorrow."));
		else
		    MsgBox (infobox, 
			i18n("Your account expires in %1 days.").arg(expire));
		break;
	    case V_PWEXPIRE:
		expire = GRecvInt ();
		if (expire == 1)
		    MsgBox (infobox, i18n("Your password expires tomorrow."));
		else
		    MsgBox (infobox, 
			i18n("Your password expires in %1 days.").arg(expire));
		break;
	    case V_OK:
		break;
	    }
	    //qApp->desktop()->setCursor( waitCursor);
	    qApp->setOverrideCursor( waitCursor);
	    hide();
	    GSendInt (G_Login);
	    GSendStr (loginEdit->text().local8Bit());
	    GSendStr (passwdEdit->password());
	    GSendInt (2);	//3
	    GSendStr (sessargBox->currentText().utf8());
	    // GSendStr (langBox->currentText().utf8());
	    GSendInt (0);
	    if (kdmcfg->_preselUser == PRESEL_PREV)
		stsfile->writeEntry (enam, loginEdit->text());
	    qApp->quit();
	    return true;
    }
    clear_button_clicked();
    return true;
}

void
KGreeter::go_button_clicked()
{
    verifyUser(true);
}

void
KGreeter::ReturnPressed()
{
    if (!goButton->isEnabled())
	return;
    if (loginEdit->hasFocus()) {
	load_wm();
	if (!verifyUser(false))
	    passwdEdit->setFocus();
    } else if (passwdEdit->hasFocus() ||
	       sessargBox->hasFocus()) {
	verifyUser(true);
    }
}


extern bool kde_have_kipc;

extern "C" void
kg_main(int argc, char **argv)
{
    kde_have_kipc = false;
    MyApp myapp(argc, argv);

    kdmcfg = new KDMConfig();

    KGlobal::dirs()->addResourceType("user_pic",
				     KStandardDirs::kde_default("data") +
				     QString::fromLatin1("kdm/pics/users/"));

    qApp->setOverrideCursor( Qt::waitCursor );

    myapp.setFont( kdmcfg->_normalFont);

#ifdef HAVE_XKBSETPERCLIENTCONTROLS
    //
    //  Activate the correct mapping for modifiers in XKB extension as
    //  grabbed keyboard has its own mapping by default
    //
    int opcode, evbase, errbase, majret, minret;
    unsigned int value = XkbPCF_GrabsUseXKBStateMask;
    if (XkbQueryExtension (qt_xdisplay(), &opcode, &evbase,
                           &errbase, &majret, &minret))
        XkbSetPerClientControls (qt_xdisplay(), value, &value);
#endif
    SecureDisplay (qt_xdisplay());
    if (!dgrabServer)
	GSendInt (G_SetupDpy);
    kgreeter = new KGreeter;
    kgreeter->updateGeometry();
    kapp->processEvents(0);
    kgreeter->resize(kgreeter->sizeHint());
    int dw, dh, gw, gh, x, y;
#ifdef HAVE_XINERAMA
    int numHeads;
    XineramaScreenInfo *xineramaInfo;
    if (XineramaIsActive(qt_xdisplay()) &&
        ((xineramaInfo = XineramaQueryScreens(qt_xdisplay(), &numHeads)))) {
	dw = xineramaInfo->width;
	dh = xineramaInfo->height;
	XFree(xineramaInfo);
    } else
#endif
    {
	dw = QApplication::desktop()->width();
	dh = QApplication::desktop()->height();
    }
    gw = kgreeter->width();
    gh = kgreeter->height();
    if (kdmcfg->_greeterPosX >= 0) {
	x = kdmcfg->_greeterPosX;
	y = kdmcfg->_greeterPosY;
    } else {
	x = dw/2;
	y = dh/2;
    }
    x -= gw/2;
    y -= gh/2;
    if (x + gw > dw)
	x = dw - gw;
    if (y + gh > dh)
	y = dh - gh;
    kgreeter->move( x < 0 ? 0 : x, y < 0 ? 0 : y );
    kgreeter->show();
    QApplication::restoreOverrideCursor();
    Debug ("entering event loop\n");
    kapp->exec();
    delete kgreeter;
    qApp->restoreOverrideCursor();
    //qApp->desktop()->setActiveWindow();
    delete kdmcfg;
    UnsecureDisplay (qt_xdisplay());
}

#include "kgreeter.moc"

/*
 * Local variables:
 * mode: c++
 * c-file-style: "k&r"
 * End:
 */

