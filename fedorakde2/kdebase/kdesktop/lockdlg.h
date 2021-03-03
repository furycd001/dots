//===========================================================================
//
// This file is part of the KDE project
//
// Copyright (c) 1999 Martin R. Jones <mjones@kde.org>
//

#ifndef __LOCKDLG_H__
#define __LOCKDLG_H__

#include <qframe.h>
#include <qlabel.h>                                                             
#include <X11/Xlib.h>

//===========================================================================
//
// Simple dialog for entering a password.
// It does not handle password validation.
//
class PasswordDlg : public QFrame
{
    Q_OBJECT
public:
    PasswordDlg(QWidget *parent);

    //-----------------------------------------------------------------------
    //
    // Reset the password to ""
    //
    void resetPassword();

    //-----------------------------------------------------------------------
    //
    // Show "Failed" in the dialog for 1.5 seconds
    //
    void showFailed();

    //-----------------------------------------------------------------------
    //
    // Keyboard events should be passed to this function directly.
    // We accept key presses this way because the keyboard is grabbed, so we
    // don't get any events.  There's nicer ways of handling this, but this
    // is simplest.
    //
    void keyPressed(XKeyEvent *);

    //-----------------------------------------------------------------------
    //
    // return the password the user entered.
    //
    QString password() const { return mPassword; }

protected:
    void drawStars();
    QString currentUser();
    QString passwordQueryMsg(bool name);
    virtual void timerEvent(QTimerEvent *);

private:
    int         mFailedTimerId;
    int         mBlinkTimerId;
    QLabel      *mLabel;
    QLabel      *mEntry;
    QString     mPassword;
    int         mStars;
    bool        mBlink;
};

#endif

