    /*

    Greeter module for xdm
    $Id: kgreeter.h,v 1.15 2001/07/31 14:38:36 ossi Exp $

    Copyright (C) 1997, 1998 Steffen Hansen <hansen@kde.org>
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
 

#ifndef KGREETER_H
#define KGREETER_H

class QTimer;
class QIconView;
class QLabel;
class QPushButton;
class QPopupMenu;
class QComboBox;

#include <qglobal.h>
#include <qlineedit.h>
#include <qframe.h>
#include <qiconview.h>
#include <qmessagebox.h>

#include <kpassdlg.h>
#include <ksimpleconfig.h>

#include "kfdialog.h"
#include "kdmshutdown.h"

class KdmClock;


class KLoginLineEdit : public QLineEdit {
    Q_OBJECT

public:
    KLoginLineEdit( QWidget *parent = 0) : QLineEdit(parent)
    {
	setMaxLength(100);
    }

signals:
    void lost_focus();

protected:
    void focusOutEvent( QFocusEvent *e);
};


class KGreeter : public QFrame {
    Q_OBJECT

public:
    KGreeter(QWidget *parent = 0, const char *ti = 0);
    ~KGreeter();
    void ReturnPressed();

public slots:
    void go_button_clicked();
    void clear_button_clicked();
    void chooser_button_clicked();
    void console_button_clicked();
    void quit_button_clicked();
    void shutdown_button_clicked();
    void slot_user_name( QIconViewItem *);
    void slot_session_selected( );
    void SetTimer();
    void timerDone();
    void load_wm();

protected:
    void timerEvent( QTimerEvent *) {};
    void keyPressEvent( QKeyEvent *);

private:
    void set_wm(const char *);
    void insertUsers( QIconView *);
    void MsgBox(QMessageBox::Icon typ, QString msg);
    bool verifyUser( bool);

    enum WmStat { WmNone, WmPrev, WmSel };
    WmStat		wmstat;
    QString		enam;
    KSimpleConfig	*stsfile;
    QIconView		*user_view;
    KdmClock		*clock;
    QLabel		*pixLabel;
    QLabel		*loginLabel, *passwdLabel, *sessargLabel;
    KLoginLineEdit	*loginEdit;
    KPasswordEdit	*passwdEdit; 
    QComboBox		*sessargBox;
    QWidget		*sessargStat;
    QLabel		*sasPrev, *sasSel;
    QFrame		*separator;
    QTimer		*timer;
    QLabel		*failedLabel;
    QPushButton		*goButton, *clearButton;
    QPushButton		*menuButton;
    QPopupMenu		*optMenu;
    QPushButton		*quitButton;
    QPushButton		*shutdownButton;
};


#endif /* KGREETER_H */

/*
 * Local variables:
 * mode: c++
 * c-file-style: "k&r"
 * End:
 */
