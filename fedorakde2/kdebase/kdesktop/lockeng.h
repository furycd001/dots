//===========================================================================
//
// This file is part of the KDE project
//
// Copyright (c) 1999 Martin R. Jones <mjones@kde.org>
//

#ifndef __LOCKENG_H__
#define __LOCKENG_H__

#include <qwidget.h>
#include <kprocess.h>
#include "KScreensaverIface.h"
#include "lockdlg.h"
#include "xautolock.h"

//===========================================================================
/**
 * Screen saver engine.  Handles screensaver window, starting screensaver
 * hacks, and password entry.
 */
class SaverEngine 
    : public QWidget,
      virtual public KScreensaverIface
{
    Q_OBJECT
public:
    SaverEngine();
    ~SaverEngine();

    /**
     * Lock the screen
     */
    virtual void lock();

    /**
     * Save the screen
     */
    virtual void save();

    /**
     * Quit the screensaver if running
     */
    virtual void quit();

    /**
     * return true if the screensaver is enabled
     */
    virtual int  isEnabled();

    /**
     * enable/disable the screensaver
     */
    virtual bool enable( bool e );

    /**
     * return true if the screen is currently blanked
     */
    virtual int  isBlanked();

    /**
     * Read and apply configuration.
     */
    virtual void configure();

    enum State { Waiting, Saving, Password };

protected:
    virtual bool x11Event(XEvent *);
    virtual void timerEvent(QTimerEvent *);

protected slots:
    void idleTimeout();
    void passwordChecked(KProcess *);
    void hackExited(KProcess *);

protected:
    void readSaver(QString saver);
    void createSaverWindow();
    void hideSaverWindow();
    void saveVRoot();
    void setVRoot(Window win, Window rw);
    void removeVRoot(Window win);
    bool grabKeyboard();
    bool grabMouse();
    bool grabInput();
    void ungrabInput();
    void startSaver();
    void stopSaver();
    bool startHack();
    void stopHack();
    void showPassDlg();
    void hidePassDlg();
    void setPassDlgTimeout(int t);
    void killPassDlgTimeout();
    void startCheckPassword();
    bool handleKeyPress(XKeyEvent *xke);

protected:
    bool        mEnabled;
    bool        mLock;
    int         mPriority;
    bool        mLockOnce;
    State       mState;
    PasswordDlg *mPassDlg;
    Colormap    mColorMap;
    XAutoLock   *mXAutoLock;
    int         mHidePassTimerId;
    int         mCheckPassTimerId;
    KProcess    mPassProc;
    KProcess    mHackProc;
    bool        mCheckingPass;
    int         mRootWidth;
    int         mRootHeight;
    QString     mSaverExec;
    QString	mSaver;
    int		mTimeout;

    // the original X screensaver parameters
    int         mXTimeout;
    int         mXInterval;
    int         mXBlanking;
    int         mXExposures;
};

#endif

