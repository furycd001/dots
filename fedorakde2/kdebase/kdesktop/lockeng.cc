//===========================================================================
//
// This file is part of the KDE project
//
// Copyright (c) 1999 Martin R. Jones <mjones@kde.org>
//


#include <config.h>

#include <stdlib.h>
#include <qbitmap.h>
#include <qtextstream.h>

#include <kstddirs.h>
#include <kglobal.h>
#include <kapp.h>
#include <kconfig.h>
#include <kdesktopfile.h>
#include <kservicegroup.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <klocale.h>

#include "lockeng.h"
#include "lockeng.moc"

#ifdef HAVE_SETPRIORITY
#include <sys/time.h>
#include <sys/resource.h>
#endif

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>

#ifndef None
#define None 0L
#endif

#define PASSDLG_HIDE_TIMEOUT        10000

int ignoreXError(Display *, XErrorEvent *);
static Window gVRoot = 0;
static Window gVRootData = 0;
static Atom   gXA_VROOT;
static Atom   gXA_SCREENSAVER_VERSION;

//===========================================================================
//
// Screen saver engine.  Handles screensaver window, starting screensaver
// hacks, and password entry.
//
SaverEngine::SaverEngine()
    : QWidget(0L, "saver window", WStyle_Customize | WStyle_NoBorder),
      DCOPObject("KScreensaverIface")
{
    kapp->installX11EventFilter(this);

    // Save X screensaver parameters
    XGetScreenSaver(qt_xdisplay(), &mXTimeout, &mXInterval,
                    &mXBlanking, &mXExposures);

    // We'll handle blanking
    XSetScreenSaver(qt_xdisplay(), 0, mXInterval, mXBlanking, mXExposures);

    // Get root window size
    XWindowAttributes rootAttr;
    XGetWindowAttributes(qt_xdisplay(), RootWindow(qt_xdisplay(),
                        qt_xscreen()), &rootAttr);
    mRootWidth = rootAttr.width;
    mRootHeight = rootAttr.height;

    // Add non-KDE path
    KGlobal::dirs()->addResourceType("scrsav",
                                    KGlobal::dirs()->kde_default("apps") +
                                    "System/ScreenSavers/");

    // Add KDE specific screensaver path
    QString relPath="System/ScreenSavers/";
    KServiceGroup::Ptr servGroup = KServiceGroup::baseGroup( "screensavers" );
    if (servGroup)
    {
      relPath=servGroup->relPath();
      kdDebug() << "relPath=" << relPath << endl;
    }
    KGlobal::dirs()->addResourceType("scrsav",
                                     KGlobal::dirs()->kde_default("apps") +
                                     relPath);

    mState = Waiting;
    mPassDlg = 0;
    mHidePassTimerId = 0;
    mCheckPassTimerId = 0;
    mCheckingPass = false;
    mXAutoLock = 0;
    mEnabled = false;
    mLockOnce = false;
    mTimeout = 300;

    // virtual root property
    gXA_VROOT = XInternAtom (qt_xdisplay(), "__SWM_VROOT", False);
    gXA_SCREENSAVER_VERSION = XInternAtom (qt_xdisplay(), "_SCREENSAVER_VERSION", False);

    XWindowAttributes attrs;
    XGetWindowAttributes(qt_xdisplay(), winId(), &attrs);
    mColorMap = attrs.colormap;

    connect(&mPassProc, SIGNAL(processExited(KProcess *)),
                        SLOT(passwordChecked(KProcess *)));
    connect(&mHackProc, SIGNAL(processExited(KProcess *)),
                        SLOT(hackExited(KProcess *)));

    configure();
}

//---------------------------------------------------------------------------
//
// Destructor - usual cleanups.
//
SaverEngine::~SaverEngine()
{
    hidePassDlg();

    if (mXAutoLock)
    {
        delete mXAutoLock;
    }

    // Restore X screensaver parameters
    XSetScreenSaver(qt_xdisplay(), mXTimeout, mXInterval, mXBlanking,
                    mXExposures);
}

//---------------------------------------------------------------------------
void SaverEngine::lock()
{
    if (mState == Waiting)
    {
        mLockOnce = true;
        startSaver();
    }
}

//---------------------------------------------------------------------------
void SaverEngine::save()
{
    if (mState == Waiting)
    {
        startSaver();
    }
}

//---------------------------------------------------------------------------
void SaverEngine::quit()
{
    if (mState == Saving)
    {
        stopSaver();
    }
}

//---------------------------------------------------------------------------
int SaverEngine::isEnabled()
{
  return mEnabled;
}

//---------------------------------------------------------------------------
bool SaverEngine::enable( bool e )
{
    if ( e == mEnabled )
	return true;

    // If we aren't in a suitable state, we will not reconfigure.
    if (mState != Waiting)
        return false;

    mEnabled = e;

    if (mEnabled)
    {
        readSaver(mSaver);

	if ( !mXAutoLock ) {
	    mXAutoLock = new XAutoLock();
	    connect(mXAutoLock, SIGNAL(timeout()), SLOT(idleTimeout()));
	}
        mXAutoLock->setTimeout(mTimeout);
        mXAutoLock->start();

        kdDebug(1204) << "Saver Engine started, timeout: " << mTimeout << endl;
    }
    else
    {
	if (mXAutoLock)
	{
	    delete mXAutoLock;
	    mXAutoLock = 0;
	}
        mSaverExec = QString::null;

        kdDebug(1204) << "Saver Engine disabled" << endl;
    }

    return true;
}

//---------------------------------------------------------------------------
int SaverEngine::isBlanked()
{
  return (mState != Waiting);
}

//---------------------------------------------------------------------------
//
// Read and apply configuration.
//
void SaverEngine::configure()
{
    // If we aren't in a suitable state, we will not reconfigure.
    if (mState != Waiting)
        return;

    // create a new config obj to ensure we read the latest options
    KConfig *config = KGlobal::config();
    config->reparseConfiguration();

    config->setGroup("ScreenSaver");

    bool e  = config->readBoolEntry("Enabled", false);
    mLock     = config->readBoolEntry("Lock", false);
    mPriority = config->readNumEntry("Priority", 19);
    if (mPriority < 0) mPriority = 0;
    if (mPriority > 19) mPriority = 19;
    mTimeout = config->readNumEntry("Timeout", 300);
    mSaver = config->readEntry("Saver");

    mEnabled = !e;   // force the enable()

    enable( e );
}

//---------------------------------------------------------------------------
//
// Read the command line needed to run the screensaver given a .desktop file.
//
void SaverEngine::readSaver(QString saver)
{
    if (!saver.isEmpty())
    {
        QString file = locate("scrsav", saver);

//        kdDebug(1204) << "Reading saver: " << saver << endl;

        KDesktopFile config(file, true);

        if (config.hasActionGroup("Root"))
        {
            config.setActionGroup("Root");
            mSaverExec = config.readEntry("Exec");
        }

//        kdDebug(1204) << "Saver-exec: " << mSaverExec << endl;
    }
}

//---------------------------------------------------------------------------
//
// Create a window to draw our screen saver on.
//
void SaverEngine::createSaverWindow()
{
    // We only create the window once, but we reset its attributes every
    // time.

    // Some xscreensaver hacks check for this property
    const char *version = "KDE 2.0";
    XChangeProperty (qt_xdisplay(), winId(),
                     gXA_SCREENSAVER_VERSION, XA_STRING, 8, PropModeReplace,
                     (unsigned char *) version, strlen(version));

    // Set virtual root property
    saveVRoot();
    if (gVRoot)
    {
      removeVRoot(gVRoot);
    }
    setVRoot(winId(), winId());

    XSetWindowAttributes attr;
    if (mColorMap != None)
    {
        attr.colormap = mColorMap;
    }
    else
    {
        attr.colormap = DefaultColormapOfScreen(
                                ScreenOfDisplay(qt_xdisplay(), qt_xscreen()));
    }
    attr.event_mask = KeyPressMask | ButtonPressMask | MotionNotify |
                        VisibilityChangeMask | ExposureMask;
    XChangeWindowAttributes(qt_xdisplay(), winId(),
                            CWEventMask | CWColormap, &attr);

    erase();

    // set NoBackground so that the saver can capture the current
    // screen state if necessary
    setBackgroundMode( QWidget::NoBackground );

    setCursor( blankCursor );
    setGeometry(0, 0, mRootWidth, mRootHeight);
    hide();

    kdDebug(1204) << "Saver window Id: " << winId() << endl;
}

//---------------------------------------------------------------------------
//
// Hide the screensaver window
//
void SaverEngine::hideSaverWindow()
{
  hide();
  removeVRoot(winId());
  XDeleteProperty(qt_xdisplay(), winId(), gXA_SCREENSAVER_VERSION);
  if (gVRoot)
  {
    setVRoot(gVRoot, gVRootData);
    gVRoot = 0;
    gVRootData = 0;
  }
  XSync(qt_xdisplay(), False);
}

//---------------------------------------------------------------------------
//
// Save the current virtual root window
//
void SaverEngine::saveVRoot()
{
  Window rootReturn, parentReturn, *children;
  unsigned int numChildren;
  Window root = kapp->desktop()->winId();

  gVRoot = 0;
  gVRootData = 0;

  int (*oldHandler)(Display *, XErrorEvent *);
  oldHandler = XSetErrorHandler(ignoreXError);

  if (XQueryTree(qt_xdisplay(), root, &rootReturn, &parentReturn,
      &children, &numChildren))
  {
    for (unsigned int i = 0; i < numChildren; i++)
    {
      Atom actual_type;
      int actual_format;
      unsigned long nitems, bytesafter;
      Window *newRoot = (Window *)0;

      if ((XGetWindowProperty(qt_xdisplay(), children[i], gXA_VROOT, 0, 1,
          False, XA_WINDOW, &actual_type, &actual_format, &nitems, &bytesafter,
          (unsigned char **) &newRoot) == Success) && newRoot)
      {
        gVRoot = children[i];
        gVRootData = *newRoot;
        break;
      }
    }
    if (children)
    {
      XFree((char *)children);
    }
  }

  XSetErrorHandler(oldHandler);
}

//---------------------------------------------------------------------------
//
// Set the virtual root property
//
void SaverEngine::setVRoot(Window win, Window rw)
{
  XChangeProperty(qt_xdisplay(), win, gXA_VROOT, XA_WINDOW, 32,
                  PropModeReplace, (unsigned char *)&rw, 1);
}

//---------------------------------------------------------------------------
//
// Remove the virtual root property
//
void SaverEngine::removeVRoot(Window win)
{
  XDeleteProperty (qt_xdisplay(), win, gXA_VROOT);
}

//---------------------------------------------------------------------------
//
// Grab the keyboard. Returns true on success
//
bool SaverEngine::grabKeyboard()
{
    int rv = XGrabKeyboard( qt_xdisplay(), QApplication::desktop()->winId(),
        True, GrabModeAsync, GrabModeAsync, CurrentTime );

    return (rv == GrabSuccess);
}

//---------------------------------------------------------------------------
//
// Grab the mouse.  Returns true on success
//
bool SaverEngine::grabMouse()
{
    int rv = XGrabPointer( qt_xdisplay(), QApplication::desktop()->winId(),
            True, ButtonPressMask
            | ButtonReleaseMask | EnterWindowMask | LeaveWindowMask
            | PointerMotionMask | PointerMotionHintMask | Button1MotionMask
            | Button2MotionMask | Button3MotionMask | Button4MotionMask
            | Button5MotionMask | ButtonMotionMask | KeymapStateMask,
            GrabModeAsync, GrabModeAsync, None, blankCursor.handle(),
            CurrentTime );

    return (rv == GrabSuccess);
}

//---------------------------------------------------------------------------
//
// Grab keyboard and mouse.  Returns true on success.
//
bool SaverEngine::grabInput()
{
    XSync(qt_xdisplay(), False);

    if (!grabKeyboard())
    {
        sleep(1);
        if (!grabKeyboard())
        {
            return false;
        }
    }

    if (!grabMouse())
    {
        sleep(1);
        if (!grabMouse())
        {
            XUngrabKeyboard(qt_xdisplay(), CurrentTime);
            return false;
        }
    }

    return true;
}

//---------------------------------------------------------------------------
//
// Release mouse an keyboard grab.
//
void SaverEngine::ungrabInput()
{
    XUngrabKeyboard(qt_xdisplay(), CurrentTime);
    XUngrabPointer(qt_xdisplay(), CurrentTime);
}

//---------------------------------------------------------------------------
//
// Start the screen saver.
//
void SaverEngine::startSaver()
{
    if (mState != Waiting)
    {
        kdWarning(1204) << "SaverEngine::startSaver() saver already active" << endl;
        return;
    }

    kdDebug(1204) << "SaverEngine: starting saver" << endl;

    if (!grabInput())
    {
        kdWarning(1204) << "SaverEngine::startSaver() grabInput() failed!!!!" << endl;
        return;
    }
    mState = Saving;
    if (mXAutoLock)
    {
        mXAutoLock->stop();
    }
    createSaverWindow();
    move(0, 0);
    show();
    setCursor( blankCursor );
    raise();
    XSync(qt_xdisplay(), False);

    if (startHack() == false)
    {
        // failed to start a hack.  Just show a blank screen
        setBackgroundColor(black);
    }
}

//---------------------------------------------------------------------------
//
// Stop the screen saver.
//
void SaverEngine::stopSaver()
{
    if (mState == Waiting)
    {
        kdWarning(1204) << "SaverEngine::stopSaver() saver not active" << endl;
        return;
    }
    kdDebug(1204) << "SaverEngine: stopping saver" << endl;
    stopHack();
    hideSaverWindow();
    hidePassDlg();
    if (mXAutoLock)
    {
        mXAutoLock->start();
    }
    mState = Waiting;
    ungrabInput();
    mLockOnce = false;
}

//---------------------------------------------------------------------------
//
bool SaverEngine::startHack()
{
    if (mSaverExec.isEmpty())
    {
        return false;
    }

    if (mHackProc.isRunning())
    {
        stopHack();
    }

    mHackProc.clearArguments();

    QTextStream ts(&mSaverExec, IO_ReadOnly);
    QString word;
    ts >> word;
    QString path = KStandardDirs::findExe(word);

    if (!path.isEmpty())
    {
        mHackProc << path;

        kdDebug(1204) << "Starting hack: " << path << endl;

        while (!ts.atEnd())
        {
            ts >> word;
            if (word == "%w")
            {
                word = word.setNum(winId());
            }
            mHackProc << word;
        }

        if (mHackProc.start() == true)
        {
#ifdef HAVE_SETPRIORITY
            setpriority(PRIO_PROCESS, mHackProc.getPid(), mPriority);
#endif
            return true;
        }
    }

    return false;
}

//---------------------------------------------------------------------------
//
void SaverEngine::stopHack()
{
    if (mHackProc.isRunning())
    {
        mHackProc.kill();
    }
}

//---------------------------------------------------------------------------
//
void SaverEngine::hackExited( KProcess * )
{
    if ( mState != Waiting ) {
	// Hack exited while we're supposed to be saving the screen.
	// Make sure the saver window is black.
        setBackgroundColor(black);
    }
}

//---------------------------------------------------------------------------
//
// Show the password dialog
//
void SaverEngine::showPassDlg()
{
    if (mPassDlg)
    {
        hidePassDlg();
    }
    mPassDlg = new PasswordDlg(this);
    mPassDlg->move((mRootWidth - mPassDlg->width())/2,
                    (mRootHeight - mPassDlg->height())/2);
    mPassDlg->show();
    setPassDlgTimeout(PASSDLG_HIDE_TIMEOUT);
}

//---------------------------------------------------------------------------
//
// Hide the password dialog
//
void SaverEngine::hidePassDlg()
{
    if (mPassDlg)
    {
        delete mPassDlg;
        mPassDlg = 0;
        killPassDlgTimeout();
    }
}

//---------------------------------------------------------------------------
//
// Hide the password dialog in "t" seconds.
//
void SaverEngine::setPassDlgTimeout(int t)
{
    if (mHidePassTimerId)
    {
        killTimer(mHidePassTimerId);
    }
    mHidePassTimerId = startTimer(t);
}

//---------------------------------------------------------------------------
//
// Kill the password dialog hide timer.
//
void SaverEngine::killPassDlgTimeout()
{
    if (mHidePassTimerId)
    {
        killTimer(mHidePassTimerId);
        mHidePassTimerId = 0;
    }
}

//---------------------------------------------------------------------------
//
// XAutoLock has detected the required idle time.
//
void SaverEngine::idleTimeout()
{
    startSaver();
}

//---------------------------------------------------------------------------
//
// X11 Event.
//
bool SaverEngine::x11Event(XEvent *event)
{
    if (!mEnabled && mState == Waiting)
    {
        return false;
    }

    bool ret = false;
    switch (event->type)
    {
        case KeyPress:
            ret = handleKeyPress((XKeyEvent *)event);
	    break;

        case ButtonPress:
        case MotionNotify:
            if (mState == Saving)
            {
                if (mLock || mLockOnce)
                {
                    showPassDlg();
                    mState = Password;
                }
                else
                {
                    stopSaver();
                }
            }
            break;

        case CreateNotify:
            if (event->xcreatewindow.window == winId() ||
                (mPassDlg && event->xcreatewindow.window == mPassDlg->winId()))
            {
                break;
            }
            if (mXAutoLock)
            {
                mXAutoLock->windowCreated(event->xcreatewindow.window);
            }
            break;

        case VisibilityNotify:
            if (event->xvisibility.state != VisibilityUnobscured &&
                event->xvisibility.window == winId() &&
                (mState == Saving || mState == Password))
            {
                raise();
                QApplication::flushX();
            }
            break;

        case ConfigureNotify:
            // Workaround for bug in Qt 2.1, as advised by Matthias Ettrich (David)
            if (event->xconfigure.window != event->xconfigure.event)
                return true;

            if (mState == Saving || mState == Password)
            {
                raise();
                QApplication::flushX();
            }
            break;
    }

    return ret;
}

//---------------------------------------------------------------------------
//
// Handle key press event.
//
bool SaverEngine::handleKeyPress(XKeyEvent *xke)
{
    bool ret = false;

    switch (mState)
    {
        case Waiting:
            if (!xke->send_event && mXAutoLock)
            {
                mXAutoLock->keyPressed();
            }
	    break;

        case Password:
            if (!mCheckingPass)
            {
                KeySym keysym = XLookupKeysym(xke, 0);
                switch (keysym)
                {
                    case XK_Escape:
                        hidePassDlg();
                        mState = Saving;
                        break;

                    case XK_Return:
                    case XK_KP_Enter:
                        startCheckPassword();
                        break;

                    default:
                        setPassDlgTimeout(PASSDLG_HIDE_TIMEOUT);
                        mPassDlg->keyPressed(xke);
                }
            }
	    ret = true;
	    break;

        case Saving:
            if (mLock || mLockOnce)
            {
                showPassDlg();
                mState = Password;
            }
            else
            {
                stopSaver();
            }
	    return true;
    }

    return ret;
}

//---------------------------------------------------------------------------
//
// Starts the kcheckpass process to check the user's password.
//
// Serge Droz <serge.droz@pso.ch> 10.2000
// Define ACCEPT_ENV if you want to pass an environment variable to
// kcheckpass. Define ACCEPT_ARGS if you want to pass command line
// arguments to kcheckpass
#define ACCEPT_ENV
//#define ACCEPT_ARGS
void SaverEngine::startCheckPassword()
{
    const char *passwd = mPassDlg->password().ascii();
    if (passwd)
    {
        QString kcp_binName = locate("exe", "kcheckpass");

        mPassProc.clearArguments();
        mPassProc << kcp_binName;

#ifdef HAVE_PAM
# ifdef ACCEPT_ENV
        setenv("KDE_PAM_ACTION", KSCREENSAVER_PAM_SERVICE, 1);
# elif defined(ACCEPT_ARGS)
        mPassProc << "-c" << KSCREENSAVER_PAM_SERVICE;
# endif
#endif
	bool ret = mPassProc.start(KProcess::NotifyOnExit, KProcess::Stdin);
#ifdef HAVE_PAM
# ifdef ACCEPT_ENV
        unsetenv("KDE_PAM_ACTION");
# endif
#endif
	if (ret == false)
        {
            kdDebug(1204) << "kcheckpass failed to start" << endl;
            return;
        }

        // write Password to stdin
        mPassProc.writeStdin(passwd, strlen(passwd));
        mPassProc.closeStdin();

        killPassDlgTimeout();

        mCheckingPass = true;
    }
}

//---------------------------------------------------------------------------
//
// The kcheckpass process has exited.
//
void SaverEngine::passwordChecked(KProcess *proc)
{
    if (proc == &mPassProc)
    {
	    /* the exit codes of kcheckpass:
	       0: everything fine
		   1: authentification failed
		   2: passwd access failed [permissions/misconfig]
	    */
        if (mPassProc.normalExit() && (mPassProc.exitStatus() != 1))
        {
            stopSaver();
	    if ( mPassProc.exitStatus() == 2 )
	    {
		KMessageBox::error(0,
		  i18n( "<h1>Screen Locking Failed!</h1>"
		  "Your screen was not locked because the <i>kcheckpass</i> "
		  "program was not able to check your password.  This is "
		  "usually the result of kcheckpass not being installed "
		  "correctly.  If you installed KDE yourself, reinstall "
		  "kcheckpass as root.  If you are using a pre-compiled "
		  "package, contact the packager." ),
		  i18n( "Screen Locking Failed" ) );
	    }
        }
        else
        {
            mPassDlg->showFailed();
            mPassDlg->resetPassword();
            setPassDlgTimeout(PASSDLG_HIDE_TIMEOUT);
        }

        mCheckingPass = false;
    }
}

//---------------------------------------------------------------------------
//
// Handle our timer events.
//
void SaverEngine::timerEvent(QTimerEvent *ev)
{
    if (ev->timerId() == mHidePassTimerId && !mCheckingPass)
    {
        hidePassDlg();
        mState = Saving;
    }
}

//---------------------------------------------------------------------------
int ignoreXError(Display *, XErrorEvent *)
{
    return 0;
}

#undef None
