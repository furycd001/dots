    /*

    Dialog class to handle input focus -- see headerfile
    $Id: kfdialog.cpp,v 1.5 2001/03/19 14:28:15 ossi Exp $

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
 

#include <qapplication.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qhbox.h>

#include <kapp.h>
#include <kdialogbase.h>
#include <klocale.h>

#include "kfdialog.h"

int
FDialog::exec()
{
    setResult(0);

    show();
    if (isModal()) {
#if 0	/* Enable this for qt 3.0 */
	qApp->processEvents();

	if (XGrabKeyboard ( qt_xdisplay(), winId(), True, GrabModeAsync,
			    GrabModeAsync, CurrentTime ) == GrabSuccess) {
	    QDialog::exec();
	    XUngrabKeyboard (qt_xdisplay(), CurrentTime);
	} else
	    hide();
#endif

	// Give focus back to parent:
	if( parentWidget() != 0)
	    parentWidget()->setActiveWindow();
    }

    return result();
}

void
KFMsgBox::box(QWidget *parent, QMessageBox::Icon type, const QString &text)
{
    QFrame winFrame( 0);
    winFrame.setFrameStyle( QFrame::Panel | QFrame::Raised);
    winFrame.setLineWidth( 2);

    KDialogBase dialog(QString::fromLatin1(""),
		       KDialogBase::Yes,
		       KDialogBase::Yes, KDialogBase::Yes,
		       &winFrame, 0, true, true,
		       i18n("&OK"));

    QWidget *contents = new QWidget(&dialog);
    QHBoxLayout *lay = new QHBoxLayout(contents);
    lay->setSpacing(KDialog::spacingHint()*2);
    lay->setMargin(KDialog::marginHint()*2);

    lay->addStretch(1);
    QLabel *label1 = new QLabel( contents);
    label1->setPixmap(QMessageBox::standardIcon(type, kapp->style().guiStyle()));
    lay->add( label1 );
    lay->add( new QLabel(text, contents) );
    lay->addStretch(1);

    dialog.setMainWidget(contents);
    dialog.enableButtonSeparator(false);

    dialog.resize(dialog.sizeHint());
    int ft = winFrame.frameWidth() * 2;
    winFrame.resize( dialog.width() + ft, dialog.height() + ft);
    int fw = winFrame.width(), fh = winFrame.height();

    QWidget *desk = QApplication::desktop();

    QPoint p;
    if ( parent ) {
	parent = parent->topLevelWidget();
	QPoint pp = parent->mapToGlobal( QPoint(0,0) );
	p = QPoint( pp.x() + parent->width()/2,
		    pp.y() + parent->height()/ 2 );
    } else {
	p = QPoint( desk->width()/2, desk->height()/2 );
    }

    p = QPoint( p.x()-fw/2, p.y()-fh/2);

    if ( p.x() + fw > desk->width() )
	p.setX( desk->width() - fw);
    if ( p.x() < 0 )
	p.setX( 0 );

    if ( p.y() + fh > desk->height() )
	p.setY( desk->height() - fh);
    if ( p.y() < 0 )
	p.setY( 0 );

    winFrame.move( p);
    winFrame.show();

    dialog.exec();

    if( parent != 0)
	parent->setActiveWindow();
}

#include "kfdialog.moc"
