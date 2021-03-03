/****************************************************************************
** $Id: qt/examples/ftpclient/ftpmainwindow.h   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef FTPMAINWINDOW_H
#define FTPMAINWINDOW_H

#include <qmainwindow.h>
#include <qurloperator.h>

class FtpView;
class QSplitter;
class QVBox;
class QSpinBox;
class QComboBox;
class QLineEdit;
class QNetworkOperation;
class QLabel;
class QProgressBar;

class FtpMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    FtpMainWindow();

    QSplitter *mainSplitter() const {
	return splitter;
    }

private:
    void setupLeftSide();
    void setupRightSide();
    void setupCenterCommandBar();
    void setup();

private slots:
    void slotLocalDirChanged( const QString &path );
    void slotLocalDirChanged( const QUrlInfo &info );
    void slotRemoteDirChanged( const QString &path );
    void slotRemoteDirChanged( const QUrlInfo &info );
    void slotConnect();
    void slotUpload();
    void slotDownload();
    void slotLocalStart( QNetworkOperation * );
    void slotLocalFinished( QNetworkOperation * );
    void slotRemoteStart( QNetworkOperation * );
    void slotRemoteFinished( QNetworkOperation * );
    void slotLocalDataTransferProgress( int, int, QNetworkOperation * );
    void slotRemoteDataTransferProgress( int, int, QNetworkOperation * );
    void slotLocalMkdir();
    void slotLocalRemove();
    void slotRemoteMkdir();
    void slotRemoteRemove();
    void slotConnectionStateChanged( int, const QString &msg );
    
private:
    QSplitter *splitter;
    QVBox *mainWidget;
    FtpView *leftView, *rightView;
    QComboBox *localCombo, *remoteHostCombo, *remotePathCombo, *userCombo;
    QLineEdit *passLined;
    QSpinBox *portSpin;
    QUrlOperator localOperator, remoteOperator, oldLocal, oldRemote;
    QLabel *progressLabel1, *progressLabel2;
    QProgressBar *progressBar1, *progressBar2;

};

#endif
