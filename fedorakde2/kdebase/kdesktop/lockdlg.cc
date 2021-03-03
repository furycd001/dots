//===========================================================================
//
// This file is part of the KDE project
//
// Copyright (c) 1999 Martin R. Jones <mjones@kde.org>
//

#include <ctype.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include <qlayout.h>
#include <qfontmetrics.h>
#include <kapp.h>
#include <klocale.h>
#include <kstddirs.h>
#include <kglobal.h>
#include <kglobalsettings.h>
#include <kconfig.h>
#include <kiconloader.h>
#include "lockdlg.h"
#include "lockdlg.moc"

#include <X11/Xutil.h>
#include <X11/keysym.h>

#define MAX_PASSWORD_LENGTH     20

//===========================================================================
//
// Simple dialog for entering a password.
// It does not handle password validation.
//
PasswordDlg::PasswordDlg(QWidget *parent)
    : QFrame(parent)
{
    setFocusPolicy(StrongFocus);
    setFrameStyle(QFrame::Panel | QFrame::Raised);
    setLineWidth(2);

    mStars = 0;
    KConfig *config = KGlobal::config();
    config->reparseConfiguration();
    KConfigGroupSaver cgs(config, "Passwords");
    QString val = config->readEntry("EchoMode", "x");
    if (val == "OneStar")
	mStars = 1;
    else if (val == "ThreeStars")
	mStars = 3;

    mBlink = false;
    mPassword = "";

    QGridLayout *layout = new QGridLayout(this, 2, 3, 20, 10);
    layout->setResizeMode(QLayout::Minimum);
    layout->addColSpacing(1, 20);

    QLabel *pixlabel= new QLabel(this);
    pixlabel->setPixmap(QPixmap(locate("data", "kdesktop/pics/ksslogo.png")));
    layout->addMultiCellWidget(pixlabel, 0, 1, 0, 0, QLayout::AlignTop);

    QFont font = KGlobalSettings::generalFont();
    font.setPointSize(18);

    QString query = passwordQueryMsg(true);
    mLabel = new QLabel(query, this);
    mLabel->setFont(font);
    mLabel->setAlignment(AlignCenter);

    layout->addWidget(mLabel, 0, 2);

    font.setPointSize(16);
    mEntry = new QLabel("*********************_", this);
    mEntry->setFont(font);
    mEntry->setMinimumHeight(mEntry->sizeHint().height()+5);
    mEntry->setMinimumWidth(mEntry->sizeHint().width()+10);
    mEntry->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    mEntry->setText("");

    layout->addWidget(mEntry, 1, 2);
    layout->activate();

    resize(layout->sizeHint());

    mFailedTimerId = 0;
    mBlinkTimerId = startTimer(300);
}

//---------------------------------------------------------------------------
//
// Reset the password to ""
//
void PasswordDlg::resetPassword()
{
    mPassword = "";
    drawStars();
}

//---------------------------------------------------------------------------
//
// Show "Failed" in the dialog for 1.5 seconds
//
void PasswordDlg::showFailed()
{
    mLabel->setText(i18n("Failed"));
    mFailedTimerId = startTimer(1500);
}

//---------------------------------------------------------------------------
//
// Keyboard events should be passed to this function directly.
// We accept key presses this way because the keyboard is grabbed, so we
// don't get any events.  There's nicer ways of handling this, but this is
// simplest.
//
void PasswordDlg::keyPressed(XKeyEvent *xKeyEvent)
{
    KeySym keysym = 0;
    char buffer[10] = "";
    (void)XLookupString(xKeyEvent, buffer, 10, &keysym, 0);

    switch (keysym)
    {
        case XK_BackSpace:
            if (mPassword.length())
            {
                mPassword.truncate(mPassword.length() - 1);
                drawStars();
            }
            break;

        default:
            if (mPassword.length() < MAX_PASSWORD_LENGTH && !iscntrl(buffer[0]))
            {
                 mPassword += buffer[0];
                 drawStars();
            }
    }
}

//---------------------------------------------------------------------------
//
// Draws the stars if mStars is true
//
void PasswordDlg::drawStars()
{
    QString s("");

    if (mStars)
        s.fill('*', mPassword.length() * mStars);

    if (mBlink)
	s += "_";

    mEntry->setText(s);
}

//---------------------------------------------------------------------------
//
// Fetch current user id, and return "Firstname Lastname (username)"
//
QString PasswordDlg::currentUser(void)
{
    struct passwd *current = getpwuid(getuid());
    QString fullname = QString::fromLocal8Bit(current->pw_gecos);
    if (fullname.find(',') != -1)
    {
        // Remove everything from and including first comma
        fullname.truncate(fullname.find(','));
    }

    QString username = QString::fromLocal8Bit(current->pw_name);

    return fullname + " (" + username + ")";
}

//---------------------------------------------------------------------------
//
// This returns the string to use to ask the user for their password.
//
QString PasswordDlg::passwordQueryMsg(bool name)
{
    QString retval("");
    if (name)
    {
        retval = currentUser();
    }
    return i18n("Enter Password") + "\n" + retval;
} 

//---------------------------------------------------------------------------
//
// Handle timer events.
//
void PasswordDlg::timerEvent(QTimerEvent *ev)
{
    if (ev->timerId() == mBlinkTimerId)
    {
        // Show/hide the password entry cursor.
        mBlink = !mBlink;
        drawStars();
    }
    else if (ev->timerId() == mFailedTimerId)
    {
        // Show the normal password prompt.
        mLabel->setText(passwordQueryMsg(true));
        drawStars();
        killTimer(mFailedTimerId);
        mFailedTimerId = 0;
    }
}

