/* This file is part of the KDE libraries
   Copyright (C) 2000 Matej Koss <koss@miesto.sk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qpushbutton.h>
#include <qtimer.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qdatetime.h>

#include <kapp.h>
#include <kdebug.h>
#include <kdialog.h>
#include <kstringhandler.h>
#include <kglobal.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kwin.h>

#include "jobclasses.h"
#include "defaultprogress.h"

class DefaultProgress::DefaultProgressPrivate
{
public:
  bool noCaptionYet;
};

DefaultProgress::DefaultProgress( bool showNow )
  : ProgressBase( 0 ),
  m_iTotalSize(0), m_iTotalFiles(0), m_iTotalDirs(0),
  m_iProcessedSize(0), m_iProcessedDirs(0), m_iProcessedFiles(0)
{
  d = new DefaultProgressPrivate;

  // Set a useful icon for this window!
  KWin::setIcons( winId(),
          KGlobal::iconLoader()->loadIcon( "filesave", KIcon::NoGroup, 32 ),
          KGlobal::iconLoader()->loadIcon( "filesave", KIcon::NoGroup, 16 ) );
  
  QVBoxLayout *topLayout = new QVBoxLayout( this, KDialog::marginHint(),
                                            KDialog::spacingHint() );
  topLayout->addStrut( 360 );   // makes dlg at least that wide

  QGridLayout *grid = new QGridLayout( 2, 3 );
  topLayout->addLayout(grid);
  grid->addColSpacing(1, KDialog::spacingHint());
  // filenames or action name
  grid->addWidget(new QLabel(i18n("Source:"), this), 0, 0);

  sourceLabel = new KSqueezedTextLabel(this);
  grid->addWidget(sourceLabel, 0, 2);

  destInvite = new QLabel(i18n("Destination:"), this);
  grid->addWidget(destInvite, 1, 0);

  destLabel = new KSqueezedTextLabel(this);
  grid->addWidget(destLabel, 1, 2);

  m_pProgressBar = new KProgress(0, 100, 0, KProgress::Horizontal, this);
  topLayout->addWidget( m_pProgressBar );

  // processed info
  QHBoxLayout *hBox = new QHBoxLayout();
  topLayout->addLayout(hBox);

  sizeLabel = new QLabel(this);
  hBox->addWidget(sizeLabel);

  resumeLabel = new QLabel(this);
  hBox->addWidget(resumeLabel);

  progressLabel = new QLabel( this );
/*  progressLabel->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding,
                                             QSizePolicy::Preferred ) );*/
  progressLabel->setAlignment( QLabel::AlignRight );
  hBox->addWidget( progressLabel );
  
  hBox = new QHBoxLayout();
  topLayout->addLayout(hBox);
  
  speedLabel = new QLabel(this);
  hBox->addWidget(speedLabel, 1);

  QFrame *line = new QFrame( this );
  line->setFrameShape( QFrame::HLine );
  line->setFrameShadow( QFrame::Sunken );
  topLayout->addWidget( line );

  hBox = new QHBoxLayout();
  topLayout->addLayout(hBox);

  hBox->addStretch(1);

  QPushButton *pb = new QPushButton( i18n("Cancel"), this );
  connect( pb, SIGNAL( clicked() ), SLOT( slotStop() ) );
  hBox->addWidget( pb );

  resize( sizeHint() );
  setMaximumHeight(sizeHint().height());

  d->noCaptionYet = true;
  setCaption(i18n("Progress Dialog")); // show something better than kio_uiserver

  if ( showNow ) {
    show();
  }
}

DefaultProgress::~DefaultProgress()
{
  delete d;
}

void DefaultProgress::slotTotalSize( KIO::Job*, unsigned long bytes )
{
  m_iTotalSize = bytes;
}


void DefaultProgress::slotTotalFiles( KIO::Job*, unsigned long files )
{
  m_iTotalFiles = files;
  showTotals();
}


void DefaultProgress::slotTotalDirs( KIO::Job*, unsigned long dirs )
{
  m_iTotalDirs = dirs;
  showTotals();
}

void DefaultProgress::showTotals()
{
  // Show the totals in the progress label, if we still haven't
  // processed anything. This is useful when the stat'ing phase
  // of CopyJob takes a long time (e.g. over networks).
  if ( m_iProcessedFiles == 0 && m_iProcessedDirs == 0 )
  {
    QString tmps;
    if ( m_iTotalDirs > 1 ) 
      // that we have a singular to translate looks weired but is only logical
      tmps = i18n("%n directory", "%n directories", m_iTotalDirs) + "   ";
    tmps += i18n("%n file", "%n files", m_iTotalFiles);
    progressLabel->setText( tmps );
  }
}

void DefaultProgress::slotPercent( KIO::Job*, unsigned long percent )
{
  QString tmp(i18n( "%1% of %2 ").arg( percent ).arg( KIO::convertSize(m_iTotalSize)));
  m_pProgressBar->setValue( percent );
  switch(mode) {
  case Copy:
    tmp.append(i18n(" (Copying)"));
    break;
  case Move:
    tmp.append(i18n(" (Moving)"));
    break;
  case Delete:
    tmp.append(i18n(" (Deleting)"));
    break;
  case Create:
    tmp.append(i18n(" (Creating)"));
    break;
  }

  setCaption( tmp );
  d->noCaptionYet = false;
}


void DefaultProgress::slotInfoMessage( KIO::Job*, const QString & msg )
{
  speedLabel->setText( msg );
}


void DefaultProgress::slotProcessedSize( KIO::Job*, unsigned long bytes ) {
  m_iProcessedSize = bytes;

  QString tmp;
  tmp = i18n( "%1 of %2 complete").arg( KIO::convertSize(bytes) ).arg( KIO::convertSize(m_iTotalSize));
  sizeLabel->setText( tmp );
}


void DefaultProgress::slotProcessedDirs( KIO::Job*, unsigned long dirs )
{
  m_iProcessedDirs = dirs;

  QString tmps;
  tmps = i18n("%1 / %n directory", "%1 / %n directories", m_iTotalDirs).arg( m_iProcessedDirs );
  tmps += "   ";
  tmps += i18n("%1 / %n file", "%1 / %n files", m_iTotalFiles).arg( m_iProcessedFiles );
  progressLabel->setText( tmps );
}


void DefaultProgress::slotProcessedFiles( KIO::Job*, unsigned long files )
{
  m_iProcessedFiles = files;

  QString tmps;
  if ( m_iTotalDirs > 1 ) {
    tmps = i18n("%1 / %n directory", "%1 / %n directories", m_iTotalDirs).arg( m_iProcessedDirs );
    tmps += "   ";
  }
  tmps += i18n("%1 / %n file", "%1 / %n files", m_iTotalFiles).arg( m_iProcessedFiles );
  progressLabel->setText( tmps );
}


void DefaultProgress::slotSpeed( KIO::Job*, unsigned long bytes_per_second )
{
  if ( bytes_per_second == 0 ) {
    speedLabel->setText( i18n( "Stalled") );
  } else {
    QTime remaining = KIO::calculateRemaining( m_iTotalSize, m_iProcessedSize, bytes_per_second );
    speedLabel->setText( i18n( "%1/s ( %2 remaining )").arg( KIO::convertSize( bytes_per_second )).arg( remaining.toString() ) );
  }
}


void DefaultProgress::slotCopying( KIO::Job*, const KURL& from, const KURL& to )
{
  if ( d->noCaptionYet ) {
    setCaption(i18n("Copy file(s) progress"));
    d->noCaptionYet = false;
  }
  mode = Copy;
  sourceLabel->setText(from.prettyURL());
  setDestVisible( true );
  destLabel->setText(to.prettyURL());
}


void DefaultProgress::slotMoving( KIO::Job*, const KURL& from, const KURL& to )
{
  if ( d->noCaptionYet ) {
    setCaption(i18n("Move file(s) progress"));
    d->noCaptionYet = false;
  }
  mode = Move;
  sourceLabel->setText(from.prettyURL());
  setDestVisible( true );
  destLabel->setText(to.prettyURL());
}


void DefaultProgress::slotCreatingDir( KIO::Job*, const KURL& dir )
{
  if ( d->noCaptionYet ) {
    setCaption(i18n("Creating directory"));
    d->noCaptionYet = false;
  }
  mode = Create;
  sourceLabel->setText(dir.prettyURL());
  setDestVisible( false );
}


void DefaultProgress::slotDeleting( KIO::Job*, const KURL& url )
{
  if ( d->noCaptionYet ) {
    setCaption(i18n("Delete file(s) progress"));
    d->noCaptionYet = false;
  }
  mode = Delete;
  sourceLabel->setText(url.prettyURL());
  setDestVisible( false );
}

void DefaultProgress::slotStating( KIO::Job*, const KURL& url )
{
  setCaption(i18n("Examining file progress"));
  sourceLabel->setText(url.prettyURL());
  setDestVisible( false );
}

void DefaultProgress::slotMounting( KIO::Job*, const QString & dev, const QString & point )
{
  setCaption(i18n("Mounting %1").arg(dev));
  sourceLabel->setText(point);
  setDestVisible( false );
}

void DefaultProgress::slotUnmounting( KIO::Job*, const QString & point )
{
  setCaption(i18n("Unmounting"));
  sourceLabel->setText(point);
  setDestVisible( false );
}

void DefaultProgress::slotCanResume( KIO::Job*, unsigned long resume )
{
  if ( resume ) {
    resumeLabel->setText( i18n("Resuming from %1").arg(resume) );
  } else {
    resumeLabel->setText( i18n("Not resumable") );
  }
}

void DefaultProgress::setDestVisible( bool visible )
{
  // We can't hide the destInvite/destLabel labels,
  // because it screws up the QGridLayout.
  if (visible)
  {
    destInvite->setText( i18n("Destination:") );
  }
  else
  {
    destInvite->setText( QString::null );
    destLabel->setText( QString::null );
  }
}

#include "defaultprogress.moc"
