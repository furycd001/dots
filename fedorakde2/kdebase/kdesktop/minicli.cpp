/* minicli.cpp
*
* Copyright (C) 1997 Matthias Ettrich <ettrich@kde.org>
* Copyright (c) 1999 Preston Brown <pbrown@kde.org>
*
* Copyright (C) 1999,2000 Dawit Alemayehu <adawit@kde.org>
* Copyright (C) 2000 Malte Starostik <starosti@zedat.fu-berlin.de>
* Copyright (C) 2000 Geert Jansen <jansen@kde.org>
*/

#include <pwd.h>
#include <string.h>
#include <errno.h>

#include <qvbox.h>
#include <qaccel.h>
#include <qlabel.h>
#include <qtimer.h>
#include <qbitmap.h>
#include <qslider.h>
#include <qlayout.h>
#include <qcstring.h>
#include <qcheckbox.h>
#include <qwhatsthis.h>
#include <qpushbutton.h>

#include <klocale.h>
#include <kconfig.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kmimetype.h>
#include <kservice.h>
#include <kprocess.h>
#include <kcombobox.h>
#include <kcompletion.h>
#include <kcompletionbox.h>
#include <knuminput.h>
#include <klineedit.h>
#include <kapp.h>
#include <kdebug.h>
#include <kpassdlg.h>
#include <krun.h>
#include <kwin.h>
#include <kdesu/stub.h>
#include <kdesu/su.h>
#include <kstddirs.h>

#include <konq_faviconmgr.h>

#include "minicli.moc"

#define KDESU_ERR strerror(errno)

#define MSGBOX_WRAPPER( code ) \
        KWin::clearState( winId(), NET::StaysOnTop ); \
        code \
        KWin::setState( winId(), NET::StaysOnTop );

Minicli::Minicli( QWidget *parent, const char *name)
        :KDialog( parent, name )
{
    m_filterData = new KURIFilterData();
    m_IconName = QString::null;
    m_FocusWidget = 0;
    loadGUI();
    KWin::setState( winId(), NET::StaysOnTop );
}

Minicli::~Minicli()
{
    if( m_filterData )
        delete m_filterData;
}

void Minicli::loadGUI()
{
    QVBoxLayout* vbox = new QVBoxLayout( this, KDialog::marginHint(),
                                         KDialog::spacingHint() );
    vbox->setResizeMode( QLayout::Fixed );
    KWin::setIcons( winId(), DesktopIcon("run"), SmallIcon("run") );
    setCaption( i18n("Run Command") );

    QHBox *hBox = new QHBox( this );
    vbox->addWidget( hBox );
    hBox->setSpacing( KDialog::marginHint() );

    m_runIcon = new QLabel( hBox );
    m_runIcon->setPixmap(DesktopIcon("go"));
    m_runIcon->setFixedSize(m_runIcon->sizeHint());

    QLabel *label = new QLabel( i18n("Enter the name of the application you want "
                                     "to run or the URL you want to view."), hBox);
    label->setAlignment( Qt::WordBreak );
    hBox = new QHBox( this );
    vbox->addWidget( hBox );
    hBox->setSpacing( KDialog::marginHint() );

    label = new QLabel(i18n("Co&mmand:"), hBox);
    label->setFixedSize(label->sizeHint());

    m_runCombo = new KHistoryCombo( hBox );
    QWhatsThis::add(m_runCombo, i18n("Enter the command you wish to execute or the address "
                                     "of the resource you want to open. This can be a remote URL "
                                     "like \"www.kde.org\" or a local one like \"~/.kderc\""));
    connect( m_runCombo, SIGNAL( textChanged( const QString& ) ),
             SLOT( slotCmdChanged( const QString& ) ) );
    label->setBuddy(m_runCombo);
    m_runCombo->setFixedWidth( m_runCombo->fontMetrics().width('W') * 23 );
    m_parseTimer = new QTimer(this);
    connect(m_parseTimer, SIGNAL(timeout()), SLOT(slotParseTimer()));

    mbAdvanced = false;
    mpAdvanced = new MinicliAdvanced(this);
    mpAdvanced->hide();
    mpAdvanced->setEnabled( false );
    vbox->addWidget(mpAdvanced, AlignLeft);
    vbox->addSpacing( KDialog::spacingHint() );

    QWidget* btnBox = new QWidget( this );
    QBoxLayout* bbLay = new QHBoxLayout( btnBox );
    bbLay->setSpacing( KDialog::spacingHint() );

    m_btnOptions = new QPushButton( i18n("&Options >>"), btnBox );
    bbLay->addWidget( m_btnOptions );
    connect( m_btnOptions, SIGNAL(clicked()), SLOT(slotAdvanced()) );
    bbLay->addStretch( 1 );

    QPushButton* btn = new QPushButton( i18n("&Run"), btnBox );
    bbLay->addWidget( btn );
    btn->setDefault( true );
    connect( btn, SIGNAL(clicked()), this, SLOT(accept()) );

    m_btnCancel = new QPushButton( i18n("&Cancel"), btnBox );
    bbLay->addWidget( m_btnCancel );
    connect(m_btnCancel, SIGNAL(clicked()), this, SLOT(reject()));
    vbox->addWidget( btnBox );
    vbox->addSpacing( KDialog::marginHint() );

    move( (QApplication::desktop()->width()-width())/2,
          (QApplication::desktop()->height()-height())/4 );
    loadConfig();
    m_runCombo->clearEdit();
    m_runCombo->setFocus();

    // Very important, to get the size right before showing
    vbox->activate();
}

void Minicli::loadConfig()
{
    KConfig *config = KGlobal::config();
    config->setGroup("MiniCli");
    QStringList histList = config->readListEntry("History");
    int maxHistory = config->readNumEntry("HistoryLength", 50);
    m_runCombo->setMaxCount( maxHistory );
    m_runCombo->setHistoryItems( histList );
    QStringList compList = config->readListEntry("CompletionItems");
    if( compList.isEmpty() )
        m_runCombo->completionObject()->setItems( histList );
    else
        m_runCombo->completionObject()->setItems( compList );
    int mode = config->readNumEntry( "CompletionMode", KGlobalSettings::completionMode() );
    m_runCombo->setCompletionMode( (KGlobalSettings::Completion) mode );
}

void Minicli::saveConfig()
{
    KConfig *config = KGlobal::config();
    config->setGroup("MiniCli");
    config->writeEntry( "History", m_runCombo->historyItems() );
    config->writeEntry( "CompletionItems", m_runCombo->completionObject()->items() );
    config->writeEntry( "CompletionMode", (int) m_runCombo->completionMode() );
    config->sync();
}

void Minicli::accept()
{
    int ret = run_command();
    if( ret > 0 )
        return;

    m_runCombo->addToHistory( m_runCombo->currentText() );
    reset();
    QDialog::accept();
    saveConfig();
}

void Minicli::reject()
{
    reset();
    QDialog::reject();
}

void Minicli::reset()
{
    if( mbAdvanced )
        slotAdvanced();
    mpAdvanced->reset();
    m_runIcon->setPixmap( DesktopIcon("go") );
    m_runCombo->setCurrentItem( 0 );
    m_runCombo->clearEdit();
    m_runCombo->setFocus();
    m_runCombo->reset();
    m_FocusWidget = 0;
}

void Minicli::keyPressEvent( QKeyEvent* e )
{
  if ( e->key() == Qt::Key_Escape )
  {
    e->accept();
    m_btnCancel->animateClick();
    return;
  }
  QDialog::keyPressEvent( e );
}

int Minicli::run_command()
{
    kapp->propagateSessionManager();

    // Make sure we have an updated icon. We do
    // so only when the parse timer is active.
    if( m_parseTimer->isActive() )
    {
        m_parseTimer->stop();
        slotParseTimer();
    }

    QString cmd = (m_filterData->uri().isLocalFile() ? m_filterData->uri().path():m_filterData->uri().url());
    if ( m_runCombo->currentText().isEmpty() )
    {
        return 1;  // Ignore
    }

    else if (cmd == "logout")
    {
      kapp->requestShutDown();
      return 0;
    }
    else if( mpAdvanced->needsKDEsu() )
    {
        QCString user;
        struct passwd *pw;
        if (mpAdvanced->changeUid())
        {
            pw = getpwnam(mpAdvanced->username().local8Bit());
            if (pw == 0L)
            {
                MSGBOX_WRAPPER(  KMessageBox::sorry( this, i18n("<qt>The user <b>%1</b> "
                                 "does not exist on this system.</qt>").arg(mpAdvanced->username())); )
                return 1;
            }
            user = mpAdvanced->username().local8Bit();
        }
        else
        {
            pw = getpwuid(getuid());
            if (pw == 0L)
            {
                MSGBOX_WRAPPER( KMessageBox::error( this, i18n("You don't exist!\n")); )
                return 1;
            }
            user = pw->pw_name;
        }

        QApplication::flushX();
        int pid = fork();
        if (pid < 0)
        {
            kdError(1207) << "fork(): " << KDESU_ERR << "\n";
            return -1;
        }
        if (pid > 0)
        {
            return 0;
        }

        SuProcess proc;
        proc.setUser(user);

        if (mpAdvanced->changeScheduler())
        {
            proc.setPriority(mpAdvanced->priority());
            proc.setScheduler(mpAdvanced->scheduler());
        }

        QCString command = cmd.local8Bit();
        if( m_filterData->hasArgsAndOptions() )
            command += m_filterData->argsAndOptions().local8Bit();
        if (mpAdvanced->terminal())
            command.prepend("konsole2 -e /bin/sh -c ");
        proc.setCommand(command);
        if (proc.checkInstall(mpAdvanced->password()) < 0)
        {
            MSGBOX_WRAPPER( KMessageBox::sorry(this, i18n("Incorrect password! Please try again.")); )
            return 1;
        }

        // Block SIGCHLD because SuProcess::exec() uses waitpid()
        sigset_t sset;
        sigemptyset(&sset);
        sigaddset(&sset, SIGCHLD);
        sigprocmask(SIG_BLOCK, &sset, 0L);
        proc.setTerminal(true);
        proc.setErase(true);
        int ret = proc.exec(mpAdvanced->password());
        _exit(ret);
    }
    else
    {
        QString exec;
        if (mpAdvanced->terminal())
        {
            exec = QString::fromLatin1("konsole2");
            cmd = QString::fromLatin1("konsole2 -e ") + cmd;
            if( m_filterData->hasArgsAndOptions() )
                cmd += m_filterData->argsAndOptions();
        }
        else
        {
            switch( m_filterData->uriType() )
            {
                case KURIFilterData::LOCAL_FILE:
                case KURIFilterData::LOCAL_DIR:
                case KURIFilterData::NET_PROTOCOL:
                case KURIFilterData::HELP:
                {
                    // No need for kfmclient, KRun does it all (David)
                    (void) new KRun( m_filterData->uri() );
                    return 0;
                }
                case KURIFilterData::EXECUTABLE:
                case KURIFilterData::SHELL:
                {
                    exec = cmd;
                    if( m_filterData->hasArgsAndOptions() )
                        cmd += m_filterData->argsAndOptions();
                    //kdDebug(1207) << "Command to execute: " << cmd << endl;
                    //kdDebug(1207) << "Executable name: " << exec << endl;
                    break;
                }
                case KURIFilterData::UNKNOWN:
                case KURIFilterData::ERROR:
                default:
                    MSGBOX_WRAPPER(
                    KMessageBox::sorry( this, i18n("<center><b>%1</b></center>\n"
                                                   "Could not run the specified command!").arg( cmd ));
                    )
                    return 1;
            }
        }
        if ( KRun::runCommand( cmd, exec, m_IconName ) )
        {
            return 0;
        }
        else
        {
            MSGBOX_WRAPPER(
            KMessageBox::sorry( this, i18n("<center><b>%1</b></center>\n"
                                           "The specified command does not exist!").arg( cmd ) );
            )
            return 1; // Let the user try again...
        }
    }
    return 0;
}

void Minicli::slotCmdChanged( const QString& )
{
    m_parseTimer->start(250, true);
}

void Minicli::slotAdvanced()
{
    mbAdvanced = !mbAdvanced;
    if (mbAdvanced)
    {
        mpAdvanced->show();
        m_btnOptions->setText(i18n("&Options <<"));
        mpAdvanced->setMaximumSize(1000, 1000);
        mpAdvanced->setEnabled( true );
        // Set the focus back to the widget
        // that had it to begin with, i.e. do
        // not put the focus on the "Options"
        // button
        m_FocusWidget = focusWidget();
        if( m_FocusWidget )
            m_FocusWidget->setFocus();
        mpAdvanced->adjustSize();
    }
    else
    {
        mpAdvanced->hide();
        m_btnOptions->setText(i18n("&Options >>"));
        if( m_FocusWidget && m_FocusWidget->parent() != mpAdvanced )
            m_FocusWidget->setFocus();
        mpAdvanced->setMaximumSize(0, 0);
        mpAdvanced->setEnabled( false );
        mpAdvanced->adjustSize();
    }
}

void Minicli::slotParseTimer()
{
    // Change the icon according to the command type.
    QString cmd = m_runCombo->currentText().stripWhiteSpace();

    // If user completely deleted the text!!!
    if ( cmd.isEmpty() )
    {
        m_runIcon->setPixmap(DesktopIcon("go"));
        return;
    }

    if( mpAdvanced->terminal() )
    {
        m_IconName = QString::fromLatin1( "konsole" );
    }
    else
    {
        m_filterData->setData( cmd );
        QStringList filters;
        filters << "kshorturifilter" << "kurisearchfilter";
        KURIFilter::self()->filterURI( *(m_filterData), filters );
        m_IconName = m_filterData->iconName();
        if( m_IconName.isEmpty() || m_IconName == "unknown" )
            m_IconName = QString::fromLatin1("go");
    }
    QPixmap icon = DesktopIcon( m_IconName );
    if ( m_IconName == "www" )
    {
        // Not using KIconEffect::overlay as that requires the same size
        // for the icon and the overlay, also the overlay definately doesn't
        // have a more that one-bit alpha channel here
        QPixmap overlay( locate ( "icon", KonqFavIconMgr::iconForURL( m_filterData->uri().url() ) + ".png" ) );
        if ( !overlay.isNull() )
        {
            int x = icon.width() - overlay.width(),
                y = icon.height() - overlay.height();
            if ( icon.mask() )
            {
                QBitmap mask = *icon.mask();
                bitBlt( &mask, x, y,
                        overlay.mask() ? const_cast<QBitmap *>(overlay.mask()) : &overlay,
                        0, 0, overlay.width(), overlay.height(),
                        overlay.mask() ? OrROP : SetROP );
                icon.setMask(mask);
            }
            bitBlt( &icon, x, y, &overlay );
        }
    }
    m_runIcon->setPixmap( icon );
}


MinicliAdvanced::MinicliAdvanced(QWidget *parent, const char *name)
                :QGroupBox(parent, name)
{
    setTitle(i18n("Advanced settings"));
    QBoxLayout *top = new QVBoxLayout( this, KDialog::marginHint(),
                                       KDialog::spacingHint() );
    top->addSpacing( fontMetrics().lineSpacing() );
    mpCBTerm = new QCheckBox(i18n("Run in &terminal"), this);
    QWhatsThis::add(mpCBTerm, i18n("Check this option if the application you "
                                   "want to run is a text mode application. The "
                                   "application will then be run in a terminal "
                                   "emulator window."));
    connect(mpCBTerm, SIGNAL(toggled(bool)), SLOT(slotTerminal(bool)));
    top->addWidget(mpCBTerm, AlignLeft);
    mpCBUser = new QCheckBox(i18n("Run as a different &user"), this);
    QWhatsThis::add(mpCBUser, i18n("Check this option if you want to run the "
                                   "application with a different user id.  "
                                   "Every process has a user id associated with "
                                   "it. This id code determines file access and "
                                   "other permissions. The password of the user "
                                   "is required to do this."));
    connect(mpCBUser, SIGNAL(toggled(bool)), SLOT(slotChangeUid(bool)));
    top->addWidget(mpCBUser, AlignLeft);
    QBoxLayout *hbox = new QHBoxLayout(0L, KDialog::marginHint(), KDialog::spacingHint());
    top->addLayout(hbox);
    hbox->addSpacing( KDialog::spacingHint() );
    QLabel *lbl = new QLabel(i18n("user&name:"), this);
    hbox->addWidget(lbl);
    mpEdit = new KLineEdit(this);
    QWhatsThis::add(mpEdit, i18n("Enter the user here who you want to run the "
                                 "application as."));
    lbl->setBuddy(mpEdit);
    connect(mpEdit, SIGNAL(textChanged(const QString &)),
            SLOT(slotUsername(const QString &)));
    hbox->addWidget(mpEdit);
    hbox->addStretch();
    mpCBPrio = new QCheckBox(i18n("Run with a different &priority"), this);
    QWhatsThis::add(mpCBPrio, i18n("Check this option if you want to run the "
                                   "application with a different priority. A "
                                   "higher priority tells the operating system "
                                   "to give more processing time to your "
                                   "application."));
    top->addWidget(mpCBPrio, AlignLeft);
    connect(mpCBPrio, SIGNAL(toggled(bool)), SLOT(slotChangeScheduler(bool)));

    hbox = new QHBoxLayout(0L, KDialog::marginHint(), KDialog::spacingHint());
    top->addLayout(hbox);
    hbox->addSpacing( KDialog::spacingHint() );
    lbl = new QLabel(i18n("pr&iority:"), this);
    hbox->addWidget(lbl);
    mpSlider = new QSlider(0, 100, 10, 50, QSlider::Horizontal, this);
    QWhatsThis::add(mpSlider, i18n("The priority can be set here. From left "
                                   "to right, it goes from low to high. The "
                                   "center position is the default value. For "
                                   "priorities higher than the default, you "
                                   "will need root's password."));
    lbl->setBuddy(mpSlider);
    mpSlider->setLineStep(5);
    connect(mpSlider, SIGNAL(valueChanged(int)), SLOT(slotPriority(int)));
    hbox->addWidget(mpSlider);
    lbl = new QLabel(i18n("high"), this);
    hbox->addWidget(lbl);
    hbox->addStretch();

    hbox = new QHBoxLayout(0L, KDialog::marginHint(), KDialog::spacingHint());
    top->addLayout(hbox);
    hbox->addSpacing(KDialog::spacingHint());
    lbl = new QLabel(i18n("&scheduler:"), this);
    hbox->addWidget(lbl);
    mpCombo = new KComboBox(this);
    mpCombo->completionBox()->setTabHandling( true );
    QWhatsThis::add(mpCombo, i18n("Here you can select which scheduler to use "
                                  "for the application. The scheduler is that "
                                  "part of the operating system which decides "
                                  "what process will run and which will have "
                                  "to wait. Two schedulers are available:"
                                  "<ul><li><em>Normal:</em> This is the "
                                  "standard, timesharing scheduler. It will "
                                  "fairly divide the available processing time "
                                  "over all processes. </li><li><em>Realtime:</em> "
                                  "This scheduler will run your application "
                                  "uninterrupted until it gives up the processor. "
                                  "This can be dangerous. An application that does "
                                  "not give up the processor might hang the system. "
                                  "You need root's password to use this scheduler."));
    lbl->setBuddy(mpCombo);
    connect(mpCombo, SIGNAL(activated(int)), SLOT(slotScheduler(int)));
    hbox->addWidget(mpCombo);
    hbox->addStretch();

    mpAuthLabel = new QLabel(this);
    top->addWidget(mpAuthLabel);
    hbox = new QHBoxLayout(0L, KDialog::marginHint(), KDialog::spacingHint());
    hbox->addSpacing( KDialog::spacingHint() );
    top->addLayout(hbox);
    lbl = new QLabel(i18n("pass&word:"), this);
    hbox->addWidget(lbl);
    mpPassword = new KPasswordEdit(this);
    QWhatsThis::add(mpPassword, i18n("Enter the requested password here."));
    lbl->setBuddy(mpPassword);
    hbox->addWidget(mpPassword);
    hbox->addStretch();

    // Provide username completion up to 1000 users.
    KCompletion *completion = new KCompletion;
    completion->setOrder(KCompletion::Sorted);
    struct passwd *pw;
    int i, maxEntries = 1000;
    setpwent();
    for (i=0; ((pw = getpwent()) != 0L) && (i < maxEntries); i++)
        completion->addItem(QString::fromLocal8Bit(pw->pw_name));
    endpwent();
    if (i < maxEntries)
    {
        mpEdit->setCompletionObject(completion, true);
        mpEdit->setCompletionMode(KGlobalSettings::completionMode());
        mpEdit->setAutoDeleteCompletionObject( true );
    }
    else
        delete completion;

    mpCombo->insertItem(i18n("Normal"), StubProcess::SchedNormal);
    mpCombo->insertItem(i18n("Realtime"), StubProcess::SchedRealtime);
    mpEdit->setEnabled(false);
    mpCombo->setEnabled(false);
    mpSlider->setEnabled(false);
    reset();
}

MinicliAdvanced::~MinicliAdvanced()
{
}

void MinicliAdvanced::updateAuthLabel()
{
    QString authUser;
    if (mbChangeScheduler && (mPriority > 50) || (mScheduler != StubProcess::SchedNormal))
    {
        authUser = QString::fromLatin1("root");
        mpPassword->setEnabled(true);
    } else if (mbChangeUid && !mUsername.isEmpty())
    {
        authUser = mpEdit->text();
        mpPassword->setEnabled(true);
    } else
    {
        authUser = i18n("none");
        mpPassword->setEnabled(false);
    }
    mpAuthLabel->setText(i18n("Password required: %1").arg(authUser));
}

void MinicliAdvanced::slotTerminal(bool ena)
{
    mbTerminal = ena;
}

void MinicliAdvanced::slotChangeUid(bool ena)
{
    mbChangeUid = ena;
    mpEdit->setEnabled(ena);
    if(ena)
    {
        mpEdit->selectAll();
        mpEdit->setFocus();
    }
    updateAuthLabel();
}

void MinicliAdvanced::slotUsername(const QString &name)
{
    //kdDebug(1207) << "text: " << mpEdit->text() << endl;
    mUsername = name;
    updateAuthLabel();
}

void MinicliAdvanced::slotChangeScheduler(bool ena)
{
    mbChangeScheduler = ena;
    mpCombo->setEnabled(ena);
    mpSlider->setEnabled(ena);
    updateAuthLabel();
}

bool MinicliAdvanced::needsKDEsu()
{
    return ((mbChangeScheduler && ((mPriority != 50) || (mScheduler != StubProcess::SchedNormal)))
            || (mbChangeUid && !mUsername.isEmpty()));
}

void MinicliAdvanced::slotScheduler(int scheduler)
{
    mScheduler = scheduler;
    if (mScheduler == StubProcess::SchedRealtime)
    {
        // we have to set the flags of the toplevelwidget, not ours
        KWin::clearState( topLevelWidget()->winId(), NET::StaysOnTop );
        if (KMessageBox::warningContinueCancel(this,
                    i18n("Running a realtime application can be very dangerous.\n"
                         "If the application misbehaves, the system might hang\n"
                         "unrecoverably.\n\nAre you sure you want to continue?"),
                    i18n("Danger, Will Robinson!"), i18n("Continue"))
            != KMessageBox::Continue )
        {
          mScheduler = StubProcess::SchedNormal;
          mpCombo->setCurrentItem(mScheduler);
        }
        KWin::setState( topLevelWidget()->winId(), NET::StaysOnTop );
    }
    updateAuthLabel();
}

void MinicliAdvanced::slotPriority(int priority)
{
    // Provide a way to easily return to the default priority
    if ((priority > 40) && (priority < 60))
    {
        priority = 50;
        mpSlider->setValue(50);
    }
    mPriority = priority;
    updateAuthLabel();
}

const char *MinicliAdvanced::password()
{
    return mpPassword->password();
}

void MinicliAdvanced::reset()
{
    mbTerminal = false;
    mpCBTerm->setChecked(false);
    mbChangeUid = false;
    mpCBUser->setChecked(false);
    mUsername = "root";
    mpEdit->setText(mUsername);
    mbChangeScheduler = false;
    mpCBPrio->setChecked(false);
    mPriority = 50;
    mpSlider->setValue(mPriority);
    mScheduler = StubProcess::SchedNormal;
    mpCombo->setCurrentItem(mScheduler);
    mpPassword->erase();
    updateAuthLabel();
}
