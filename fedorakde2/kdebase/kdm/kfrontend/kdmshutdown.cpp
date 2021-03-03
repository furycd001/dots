    /*

    Shutdown dialog
    $Id: kdmshutdown.cpp,v 1.18 2001/07/14 16:29:53 ossi Exp $

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
 
#include <qfile.h>
#include <qcombobox.h>

#include <kapp.h>
#include <klocale.h>
#include <kseparator.h>

#include "kdmshutdown.h"
#include "kdmconfig.h" // for shutdown-modes
#include "liloinfo.h"
#include "kdm_greet.h"


static inline void
set_min( QWidget* w)
{
    w->adjustSize();
    w->setMinimumSize( w->size());
}

static inline void
set_fixed( QWidget* w)
{
    w->adjustSize();
    w->setFixedSize( w->size());
}

KDMShutdown::KDMShutdown( QWidget* _parent)
    : FDialog( _parent, "Shutdown", true)
{
    int h = 10, w = 0;
    QComboBox *targets = 0;
    QFrame* winFrame = new QFrame( this);
    winFrame->setFrameStyle( QFrame::WinPanel | QFrame::Raised);
    QBoxLayout* box = new QBoxLayout( winFrame, QBoxLayout::TopToBottom, 
				      10, 10);
    QString shutdownmsg =  i18n( "Shutdown or reboot?");
    if( kdmcfg->_allowShutdown == SHUT_ROOT)
	shutdownmsg += '\n' + i18n( "(Enter Root Password)");

    label = new QLabel( shutdownmsg, winFrame);
    set_fixed( label);
    h += label->height() + 10;
    w = label->width();

    box->addWidget( label, 0, AlignCenter);


    KSeparator* sep = new KSeparator( KSeparator::HLine, winFrame);
    h += sep->height(); 
    box->addWidget( sep);

    btGroup = new QButtonGroup( /* this */);
     
    QRadioButton *rb;
    rb = new QRadioButton( winFrame /*btGroup*/);
    rb->setText( i18n("&Shutdown"));
    set_min( rb);
    rb->setFocusPolicy( StrongFocus);
    // Default action
    rb->setChecked( true);
    rb->setFocus();

    h += rb->height() + 10;
    w = QMAX( rb->width(), w);

    box->addWidget( rb);
    btGroup->insert( rb);

    QHBoxLayout *hbox = new QHBoxLayout(box);

    restart_rb = new QRadioButton( winFrame /*btGroup*/);
    restart_rb->setText( i18n("&Reboot"));
    set_min( restart_rb);
    restart_rb->setFocusPolicy( StrongFocus);
    h += restart_rb->height() + 10;
    w = QMAX( restart_rb->width(), w);

    hbox->addWidget(restart_rb);

#ifdef __linux__
    if ( kdmcfg->_useLilo ) {
	targets = new QComboBox(winFrame);
	hbox->addWidget(targets);

	// fill combo box with contents of lilo config
	LiloInfo info(kdmcfg->_liloCmd, kdmcfg->_liloMap);

	QStringList list;
	if (info.getBootOptions(&list) == 0) {
	    targets->insertStringList(list);
            liloTarget = info.getDefaultBootOptionIndex();
	    targets->setCurrentItem(liloTarget);
	    connect(targets,SIGNAL(activated(int)),this,SLOT(target_changed(int)));
	}
    }
#endif
    w = QMAX( restart_rb->width()
	     + (targets ? targets->sizeHint().width()+10 : 0), w);

    btGroup->insert( restart_rb);

    // Passwd line edit
    if( kdmcfg->_allowShutdown == SHUT_ROOT) {
	pswdEdit = new KPasswordEdit( winFrame, "edit", kdmcfg->_echoMode);
	pswdEdit->setFocusPolicy( StrongFocus);
	pswdEdit->setFocus();
	h+= pswdEdit->height() + 10;
	box->addWidget( pswdEdit);
	timer = new QTimer( this );
	connect( timer, SIGNAL(timeout()), SLOT(timerDone()) );
    } else {
	pswdEdit = 0;
	timer = 0;
    }

    QBoxLayout* box3 = new QBoxLayout( QBoxLayout::LeftToRight, 10);
    box->addLayout( box3);

    okButton = new QPushButton( i18n("&OK"), winFrame);
    set_min( okButton);
    okButton->setDefault( true);
    okButton->setFocusPolicy( StrongFocus);
    cancelButton = new QPushButton( i18n("&Cancel"), winFrame);
    set_min( cancelButton);
    cancelButton->setFocusPolicy( StrongFocus);
    h += cancelButton->height() + 10;
    w = QMAX( (okButton->width() + 10 + cancelButton->width()), w);

    box3->addWidget( okButton);
    box3->addWidget( cancelButton);

    connect( okButton, SIGNAL(clicked()), SLOT(bye_bye()));
    connect( cancelButton, SIGNAL(clicked()), SLOT(reject()));

    resize( 20 + w, h);
    winFrame->setGeometry( 0, 0, width(), height());
}

void
KDMShutdown::timerDone()
{
    pswdEdit->setEnabled( true);
    okButton->setEnabled( true);
    pswdEdit->setFocus();
}

void
KDMShutdown::target_changed(int id)
{
#ifdef __linux__
    restart_rb->setChecked(true);
    liloTarget = id;
#endif
}

void
KDMShutdown::bye_bye()
{
    if( pswdEdit) {
	GSendInt (G_Verify);
	GSendStr ("root");
	GSendStr (pswdEdit->password());
	if (GRecvInt () < V_OK ) {
	    pswdEdit->erase();
	    pswdEdit->setEnabled( false);
	    okButton->setEnabled( false);
	    timer->start( 1500 + kapp->random()/(RAND_MAX/1000), true );
	    return;
	}
    }
#ifdef __linux__
    if (kdmcfg->_useLilo && restart_rb->isChecked()) {
	LiloInfo info(kdmcfg->_liloCmd, kdmcfg->_liloMap);
	info.setNextBootOption(liloTarget);
    }
#endif
    SessionExit (restart_rb->isChecked() ? EX_REBOOT : EX_HALT);
}

#include "kdmshutdown.moc"

/*  
 * Local variables:  
 * mode: c++  
 * c-file-style: "k&r"  
 * End:  
*/
