/****************************************************************************
** $Id: qt/examples/ftpclient/ftpmainwindow.cpp   2.3.2   edited 2001-09-05 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "ftpmainwindow.h"
#include "ftpview.h"

#include <qvbox.h>
#include <qhbox.h>
#include <qsplitter.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qprogressbar.h>
#include <qdir.h>
#include <qinputdialog.h>
#include <qapplication.h>
#include <qstatusbar.h>

FtpMainWindow::FtpMainWindow()
    : QMainWindow(),
      localOperator( "/" )
{
    setup();

    // connect to the signals of the local QUrlOperator - this will be used to
    // work on the local file system (listing dirs, etc.) and to copy files
    // TO the local filesystem (downloading)
    connect( &localOperator, SIGNAL( newChildren( const QValueList<QUrlInfo> &, QNetworkOperation * ) ),
	     leftView, SLOT( slotInsertEntries( const QValueList<QUrlInfo> & ) ) );
    connect( &localOperator, SIGNAL( start( QNetworkOperation * ) ),
	     this, SLOT( slotLocalStart( QNetworkOperation *) ) );
    connect( &localOperator, SIGNAL( finished( QNetworkOperation * ) ),
	     this, SLOT( slotLocalFinished( QNetworkOperation *) ) );
    connect( leftView, SIGNAL( itemSelected( const QUrlInfo & ) ),
	     this, SLOT( slotLocalDirChanged( const QUrlInfo & ) ) );
    connect( &localOperator, SIGNAL( dataTransferProgress( int, int, QNetworkOperation * ) ),
             this, SLOT( slotLocalDataTransferProgress( int, int, QNetworkOperation * ) ) );

    // connect to the signals of the remote QUrlOperator - this will be used to
    // work on the remote file system (on the FTP Server) and to copy files
    // TO the ftp server (uploading)
    connect( &remoteOperator, SIGNAL( newChildren( const QValueList<QUrlInfo> &, QNetworkOperation * ) ),
	     rightView, SLOT( slotInsertEntries( const QValueList<QUrlInfo> & ) ) );
    connect( &remoteOperator, SIGNAL( start( QNetworkOperation * ) ),
	     this, SLOT( slotRemoteStart( QNetworkOperation *) ) );
    connect( &remoteOperator, SIGNAL( finished( QNetworkOperation * ) ),
	     this, SLOT( slotRemoteFinished( QNetworkOperation *) ) );
    connect( rightView, SIGNAL( itemSelected( const QUrlInfo & ) ),
	     this, SLOT( slotRemoteDirChanged( const QUrlInfo & ) ) );
    connect( &remoteOperator, SIGNAL( dataTransferProgress( int, int, QNetworkOperation * ) ),
             this, SLOT( slotRemoteDataTransferProgress( int, int, QNetworkOperation * ) ) );
    connect( &remoteOperator, SIGNAL( connectionStateChanged( int, const QString & ) ),
             this, SLOT( slotConnectionStateChanged( int, const QString & ) ) );

    // read the local filesystem at the beginning once
    localOperator.listChildren();

    // create status bar
    (void)statusBar();
}

void FtpMainWindow::setupLeftSide()
{
    // Setup the left side of the GUI, this is the listview
    // of the local filesystem

    QVBox *layout = new QVBox( splitter );
    layout->setSpacing( 5 );
    layout->setMargin( 5 );

    QHBox *h = new QHBox( layout );
    h->setSpacing( 5 );
    QLabel *l = new QLabel( tr( "Local Path:" ), h );
    l->setFixedWidth( l->sizeHint().width() );
    localCombo = new QComboBox( TRUE, h );
    localCombo->insertItem( "/" );

    connect( localCombo, SIGNAL( activated( const QString & ) ),
	     this, SLOT( slotLocalDirChanged( const QString & ) ) );

    leftView = new FtpView( layout );

    QHBox *bottom = new QHBox( layout );
    bottom->setSpacing( 5 );
    QPushButton *bMkdir = new QPushButton( tr( "New Directory" ), bottom );
    QPushButton *bRemove = new QPushButton( tr( "Remove" ), bottom );
    connect( bMkdir, SIGNAL( clicked() ),
	     this, SLOT( slotLocalMkdir() ) );
    connect( bRemove, SIGNAL( clicked() ),
	     this, SLOT( slotLocalRemove() ) );

    splitter->setResizeMode( layout, QSplitter::Stretch );
}

void FtpMainWindow::setupRightSide()
{
    // Setup the right side of the GUI, this is the listview
    // of the remote filesystem (FTP), needs also lineedits/combos
    // for username, password, etc.

    QVBox *layout = new QVBox( splitter );
    layout->setSpacing( 5 );
    layout->setMargin( 5 );

    QHBox *h = new QHBox( layout );
    h->setSpacing( 5 );
    QLabel *l = new QLabel( tr( "Remote Host:" ), h );
    l->setFixedWidth( l->sizeHint().width() );
    remoteHostCombo = new QComboBox( TRUE, h );

    l = new QLabel( tr( "Port:" ), h );
    l->setFixedWidth( l->sizeHint().width() );
    portSpin = new QSpinBox( 0, 32767, 1, h );
    portSpin->setValue( 21 );
    portSpin->setFixedWidth( portSpin->sizeHint().width() );
    remoteOperator.setPort( portSpin->value() );

    h = new QHBox( layout );
    h->setSpacing( 5 );
    l = new QLabel( tr( "Remote Path:" ), h );
    l->setFixedWidth( l->sizeHint().width() );
    remotePathCombo = new QComboBox( TRUE, h );

    h = new QHBox( layout );
    h->setSpacing( 5 );
    l = new QLabel( tr( "Username:" ), h );
    l->setFixedWidth( l->sizeHint().width() );
    userCombo = new QComboBox( TRUE, h );

    l = new QLabel( tr( "Password:" ), h );
    l->setFixedWidth( l->sizeHint().width() );
    passLined = new QLineEdit( h );
    passLined->setEchoMode( QLineEdit::Password );

    rightView = new FtpView( layout );

    QHBox *bottom = new QHBox( layout );
    bottom->setSpacing( 5 );
    QPushButton *bMkdir = new QPushButton( tr( "New Directory" ), bottom );
    QPushButton *bRemove = new QPushButton( tr( "Remove" ), bottom );
    connect( bMkdir, SIGNAL( clicked() ),
	     this, SLOT( slotRemoteMkdir() ) );
    connect( bRemove, SIGNAL( clicked() ),
	     this, SLOT( slotRemoteRemove() ) );

    splitter->setResizeMode( layout, QSplitter::Stretch );

    connect( remotePathCombo, SIGNAL( activated( const QString & ) ),
	     this, SLOT( slotRemoteDirChanged( const QString & ) ) );
}

void FtpMainWindow::setupCenterCommandBar()
{
    // Setup the command bar in the middle between the two views

    QVBox *w = new QVBox( splitter );
    splitter->setResizeMode( w, QSplitter::FollowSizeHint );
    w->setSpacing( 5 );
    w->setMargin( 5 );

    QPushButton *bConnect = new QPushButton( tr( "&Connect" ), w );
    (void)new QWidget( w );
    QPushButton *bUpload = new QPushButton( tr( "== &Upload ==>" ), w );
    QPushButton *bDownload = new QPushButton( tr( "<== &Download ==" ), w );
    (void)new QWidget( w );

    connect( bConnect, SIGNAL( clicked() ),
	     this, SLOT( slotConnect() ) );
    connect( bUpload, SIGNAL( clicked() ),
	     this, SLOT( slotUpload() ) );
    connect( bDownload, SIGNAL( clicked() ),
	     this, SLOT( slotDownload() ) );
}

void FtpMainWindow::setup()
{
    // Setup the GUI

    mainWidget = new QVBox( this );
    splitter = new QSplitter( mainWidget );
    setupLeftSide();
    setupCenterCommandBar();
    setupRightSide();

    progressLabel1 = new QLabel( tr( "No Operation in Progress" ), mainWidget );
    progressBar1 = new QProgressBar( mainWidget );
    progressLabel2 = new QLabel( tr( "No Operation in Progress" ), mainWidget );
    progressBar2 = new QProgressBar( mainWidget );

    progressLabel1->hide();
    progressBar1->hide();
    progressLabel2->hide();
    progressBar2->hide();

    setCentralWidget( mainWidget );
}

void FtpMainWindow::slotLocalDirChanged( const QString &path )
{
    // The user changed the path on the left side

    oldLocal = localOperator;
    localOperator.setPath( path );
    localOperator.listChildren();
}

void FtpMainWindow::slotLocalDirChanged( const QUrlInfo &info )
{
    // The user changed the path on the left side

    oldLocal = localOperator;
    localOperator.addPath( info.name() );
    localOperator.listChildren();
    localCombo->insertItem( localOperator.path(), 0 );
    localCombo->setCurrentItem( 0 );
}

void FtpMainWindow::slotRemoteDirChanged( const QString &path )
{
    // The user changed the path on the right side

    if ( !remoteOperator.isValid() )
	return;
    oldRemote = remoteOperator;
    remoteOperator.setPath( path );
    remoteOperator.listChildren();
}

void FtpMainWindow::slotRemoteDirChanged( const QUrlInfo &info )
{
    // The user changed the path on the right side

    oldRemote = remoteOperator;
    remoteOperator.addPath( info.name() );
    remoteOperator.listChildren();
    remotePathCombo->insertItem( remoteOperator.path(), 0 );
    remotePathCombo->setCurrentItem( 0 );
}

void FtpMainWindow::slotConnect()
{
    // The user pressed the connect button, so let's connect to the
    // FTP server
    // First we need to set stuff (host, path, etc.) which the user
    // entered on the right side to the remote QUrlOperator

    // protocol + hostname
    QString s = "ftp://" + remoteHostCombo->currentText();
    oldRemote = remoteOperator;
    remoteOperator = s;

    // path on the server
    if ( !remotePathCombo->currentText().isEmpty() )
	remoteOperator.setPath( remotePathCombo->currentText() );
    else
	remoteOperator.setPath( "/" );

    // if nothing or "ftp" or "anonymous" has been entered into the username combo,
    // let's connect anonymous, else private with password
    if ( !userCombo->currentText().isEmpty() &&
	 userCombo->currentText().lower() != "anonymous" &&
	 userCombo->currentText().lower() != "ftp" ) {
	remoteOperator.setUser( userCombo->currentText() );
	remoteOperator.setPassword( passLined->text() );
    }

    // set the port
    remoteOperator.setPort( portSpin->value() );

    // finally read the directory on the ftp server
    remoteOperator.listChildren();
}

void FtpMainWindow::slotUpload()
{
    // the user pressed the upload button

    // if files have been selected on the left side (local filesystem)
    QValueList<QUrlInfo> files = leftView->selectedItems();
    if ( files.isEmpty() )
	return;

    // create a list of the URLs which should be copied
    QStringList lst;
    QValueList<QUrlInfo>::Iterator it = files.begin();
    for ( ; it != files.end(); ++it )
	lst << QUrl( localOperator, ( *it ).name() );

    // copy the list of selected files to the directory in which the
    // remoteOperator currently is (upload)
    remoteOperator.copy( lst, remoteOperator, FALSE );
}

void FtpMainWindow::slotDownload()
{
    // if the user pressed the download button

    // if files have been selected on the right side (remote filesystem)
    QValueList<QUrlInfo> files = rightView->selectedItems();
    if ( files.isEmpty() )
	return;

    // create a list of the URLs which should be downloaded
    QStringList lst;
    QValueList<QUrlInfo>::Iterator it = files.begin();
    for ( ; it != files.end(); ++it )
	lst << QUrl( remoteOperator, ( *it ).name() );

    // copy the list of selected files to the directory in which the
    // localOperator currently is (download)
    localOperator.copy( lst, localOperator, FALSE );
}

void FtpMainWindow::slotLocalStart( QNetworkOperation *op )
{
    // this slot is always called if the local QUrlOperator starts
    // listing a directory or dowloading a file

    if ( !op )
	return;

    if ( op->operation() == QNetworkProtocol::OpListChildren ) {
	// start listing a dir? clear the left view!
	leftView->clear();
    } else if ( op->operation() == QNetworkProtocol::OpGet ) {
	// start downloading a file? reset the progress bar!
	progressBar1->setTotalSteps( 0 );
	progressBar1->reset();
    }
}

void FtpMainWindow::slotLocalFinished( QNetworkOperation *op )
{
    // this slot is always called if the local QUrlOperator finished
    // an operation

    if ( !op )
	return;

    if ( op && op->state() == QNetworkProtocol::StFailed ) {
	// an error happend, let the user know that
	QMessageBox::critical( this, tr( "ERROR" ), op->protocolDetail() );

	// do something depending in the error code
	int ecode = op->errorCode();
	if ( ecode == QNetworkProtocol::ErrListChlidren || ecode == QNetworkProtocol::ErrParse ||
	     ecode == QNetworkProtocol::ErrUnknownProtocol || ecode == QNetworkProtocol::ErrLoginIncorrect ||
	     ecode == QNetworkProtocol::ErrValid || ecode == QNetworkProtocol::ErrHostNotFound ||
	     ecode == QNetworkProtocol::ErrFileNotExisting ) {
	    localOperator = oldLocal;
	    localCombo->setEditText( localOperator.path() );
	    localOperator.listChildren();
	}
    } else if ( op->operation() == QNetworkProtocol::OpPut ) {
	// finished saving the downloaded file? reread the dir and hide the progress bar
	localOperator.listChildren();
	progressLabel1->hide();
	progressBar1->hide();
    } else if ( op->operation() == QNetworkProtocol::OpGet ) {
	// finished reading a file from the ftp server? reset the progress bar
	progressBar1->setTotalSteps( 0 );
	progressBar1->reset();
    }

}

void FtpMainWindow::slotRemoteStart( QNetworkOperation *op )
{
    // this slot is always called if the remote QUrlOperator starts
    // listing a directory or uploading a file

    if ( !op )
	return;

    if ( op->operation() == QNetworkProtocol::OpListChildren ) {
	// start listing a dir? clear the right view!
	rightView->clear();
    } else if ( op->operation() == QNetworkProtocol::OpGet ) {
	// start downloading a file? reset the progress bar!
	progressBar2->setTotalSteps( 0 );
	progressBar2->reset();
    }
}

void FtpMainWindow::slotRemoteFinished( QNetworkOperation *op )
{
    // this slot is always called if the remote QUrlOperator finished
    // an operation

    if ( !op )
	return;

    if ( op && op->state() == QNetworkProtocol::StFailed ) {
	// an error happend, let the user know that
	QMessageBox::critical( this, tr( "ERROR" ), op->protocolDetail() );

	// do something depending in the error code
	int ecode = op->errorCode();
	if ( ecode == QNetworkProtocol::ErrListChlidren || ecode == QNetworkProtocol::ErrParse ||
	     ecode == QNetworkProtocol::ErrUnknownProtocol || ecode == QNetworkProtocol::ErrLoginIncorrect ||
	     ecode == QNetworkProtocol::ErrValid || ecode == QNetworkProtocol::ErrHostNotFound ||
	     ecode == QNetworkProtocol::ErrFileNotExisting ) {
	    remoteOperator = oldRemote;
	    remoteHostCombo->setEditText( remoteOperator.host() );
	    remotePathCombo->setEditText( remoteOperator.path() );
	    passLined->setText( remoteOperator.password() );
	    userCombo->setEditText( remoteOperator.user() );
	    portSpin->setValue( remoteOperator.port() );
	    remoteOperator.listChildren();
	}
    } else if ( op->operation() == QNetworkProtocol::OpListChildren ) {
	// finished reading a dir? set the correct path to the pth combo of the right view
	remotePathCombo->setEditText( remoteOperator.path() );
    } else if ( op->operation() == QNetworkProtocol::OpPut ) {
	// finished saving the uploaded file? reread the dir and hide the progress bar
	remoteOperator.listChildren();
	progressLabel2->hide();
	progressBar2->hide();
    } else if ( op->operation() == QNetworkProtocol::OpGet ) {
	// finished reading a file from the local filesystem? reset the progress bar
	progressBar2->setTotalSteps( 0 );
	progressBar2->reset();
    }
}

void FtpMainWindow::slotLocalDataTransferProgress( int bytesDone, int bytesTotal,
						   QNetworkOperation *op )
{
    // Show the progress here of the local QUrlOperator reads or writes data

    if ( !op )
	return;

    if ( !progressBar1->isVisible() ) {
	if ( bytesDone < bytesTotal) {
	    progressLabel1->show();
	    progressBar1->show();
	    progressBar1->setTotalSteps( bytesTotal );
	    progressBar1->setProgress( 0 );
	    progressBar1->reset();
	} else
	    return;
    }

    if ( progressBar1->totalSteps() == bytesTotal )
	progressBar1->setTotalSteps( bytesTotal );

    if ( op->operation() == QNetworkProtocol::OpGet )
	progressLabel1->setText( tr( "Read: %1" ).arg( op->arg( 0 ) ) );
    else if ( op->operation() == QNetworkProtocol::OpPut )
	progressLabel1->setText( tr( "Write: %1" ).arg( op->arg( 0 ) ) );
    else
	return;

    progressBar1->setProgress( bytesDone );
}

void FtpMainWindow::slotRemoteDataTransferProgress( int bytesDone, int bytesTotal,
						    QNetworkOperation *op )
{
    // Show the progress here of the remote QUrlOperator reads or writes data

    if ( !op )
	return;

    if ( !progressBar2->isVisible() ) {
	if ( bytesDone < bytesTotal) {
	    progressLabel2->show();
	    progressBar2->show();
	    progressBar2->setTotalSteps( bytesTotal );
	    progressBar2->setProgress( 0 );
	    progressBar2->reset();
	} else
	    return;
    }

    if ( progressBar2->totalSteps() != bytesTotal )
	progressBar2->setTotalSteps( bytesTotal );

    if ( op->operation() == QNetworkProtocol::OpGet )
	progressLabel2->setText( tr( "Read: %1" ).arg( op->arg( 0 ) ) );
    else if ( op->operation() == QNetworkProtocol::OpPut )
	progressLabel2->setText( tr( "Write: %1" ).arg( op->arg( 0 ) ) );
    else
	return;

    progressBar2->setProgress( bytesDone );
}

void FtpMainWindow::slotLocalMkdir()
{
    // create a dir on the local filesystem

    bool ok = FALSE;
    QString name = QInputDialog::getText( tr( "Directory Name:" ), QString::null, QString::null, &ok, this );

    if ( !name.isEmpty() && ok )
	localOperator.mkdir( name );
}

void FtpMainWindow::slotLocalRemove()
{
    bool removeRequested = FALSE;
    QValueList<QUrlInfo> selected = leftView->selectedItems();
    for ( QValueList<QUrlInfo>::Iterator it = selected.begin();
	  it != selected.end(); ++it )
	if ( (*it).isFile() ) {
	    localOperator.remove( (*it).name() );
	    removeRequested = TRUE;
	}

    if ( removeRequested )
	localOperator.listChildren();
}

void FtpMainWindow::slotRemoteMkdir()
{
    // create a dir on the remote filesystem (FTP server)

    bool ok = FALSE;
    QString name = QInputDialog::getText( tr( "Directory Name:" ), QString::null, QString::null, &ok, this );

    if ( !name.isEmpty() && ok )
	remoteOperator.mkdir( name );
}

void FtpMainWindow::slotRemoteRemove()
{
    bool removeRequested = FALSE;
    QValueList<QUrlInfo> remoteSelected = rightView->selectedItems();
    for ( QValueList<QUrlInfo>::Iterator it = remoteSelected.begin();
	  it != remoteSelected.end(); ++it )
	if ( (*it).isFile() ) {
	    remoteOperator.remove( (*it).name() );
	    removeRequested = TRUE;
	}
    if ( removeRequested )
	remoteOperator.listChildren();
}

void FtpMainWindow::slotConnectionStateChanged( int, const QString &msg )
{
    statusBar()->message( msg );
}
