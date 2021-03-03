/*
 *  dtime.cpp
 *
 *  Copyright (C) 1998 Luca Montecchiani <m.luca@usa.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include <qmessagebox.h>
#include <qcombobox.h>
#include <qgroupbox.h>
#include <qtimer.h>
#include <qpushbutton.h>
#include <qpainter.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qwhatsthis.h>

#include <kdebug.h>
#include <kconfig.h>
#include <kglobal.h>
#include <klocale.h>
#include <kcmodule.h>
#include <kprocess.h>
#include <kmessagebox.h>

#include "dtime.h"
#include "dtime.moc"

Dtime::Dtime(QWidget * parent, const char *name)
  : QWidget(parent, name)
{
  // *************************************************************
  // Start Dialog
  // *************************************************************

  // Date box
  QGroupBox* dateBox = new QGroupBox( this, "dateBox" );

  QGridLayout *l1 = new QGridLayout( dateBox, 2, 3, 10 );

  month = new QComboBox( FALSE, dateBox, "ComboBox_1" );
  connect( month, SIGNAL(activated(int)), SLOT(set_month(int)) );
  month->setSizeLimit( 12 );
  l1->addWidget( month, 0, 0 );
  QWhatsThis::add( month, i18n("Here you can change the system date's month.") );

  year = new QSpinBox( 1970, 3000, 1, dateBox );
  year->setButtonSymbols( QSpinBox::PlusMinus );
  connect(year, SIGNAL(valueChanged(int)), this, SLOT(set_year(int)) );
  l1->addWidget( year, 0, 2 );
  QWhatsThis::add( year, i18n("Here you can change the system date's year.") );

  cal = new KDateTable( this );
  l1->addMultiCellWidget( cal, 1, 1, 0, 2 );
  QWhatsThis::add( cal, i18n("Here you can change the system date's day of the month.") );

  // Time frame
  QGroupBox* timeBox = new QGroupBox( this, "timeBox" );

  QVBoxLayout *v2 = new QVBoxLayout( timeBox, 10 );

  kclock = new Kclock( timeBox, "kclock" );
  kclock->setMinimumHeight(150);
  v2->addWidget( kclock );
//  QWhatsThis::add( kclock, i18n("") );

  QGridLayout *v3 = new QGridLayout( 2, 9 );

//  v3->setColStretch( 0, 1 );

  hour = new QLineEdit( timeBox, "LineEdit_1" );
  connect( hour, SIGNAL(textChanged(const QString&)), SLOT(set_time()) );
  hour->setMaxLength( 2 );
  hour->setFrame( TRUE );
  hour->setValidator(new KStrictIntValidator(0, 23, hour));
  v3->addMultiCellWidget(hour, 0, 1, 1, 1 );

  QLabel *dots1 = new QLabel(":", timeBox);
  dots1->setMinimumWidth( 7 );
  dots1->setAlignment( QLabel::AlignCenter );
  v3->addMultiCellWidget(dots1, 0, 1, 2, 2 );

  minute = new QLineEdit( timeBox, "LineEdit_2" );
  connect( minute, SIGNAL(textChanged(const QString&)), SLOT(set_time()) );
  minute->setMaxLength( 2 );
  minute->setFrame( TRUE );
  minute->setValidator(new KStrictIntValidator(0, 59, minute));
  v3->addMultiCellWidget(minute, 0, 1, 3, 3 );

  QLabel *dots2 = new QLabel(":", timeBox);
  dots2->setMinimumWidth( 7 );
  dots2->setAlignment( QLabel::AlignCenter );
  v3->addMultiCellWidget(dots2, 0, 1, 4, 4 );

  second = new QLineEdit( timeBox, "LineEdit_3" );
  connect( second, SIGNAL(textChanged(const QString&)), SLOT(set_time()) );
  second->setMaxLength( 2 );
  second->setFrame( TRUE );
  second->setValidator(new KStrictIntValidator(0, 59, second));
  v3->addMultiCellWidget(second, 0, 1, 5, 5 );

  int w = 2*hour->fontMetrics().width("00");
  hour->setMaximumWidth(w);
  minute->setMaximumWidth(w);
  second->setMaximumWidth(w);

  v3->addColSpacing( 6, 5 );

  QPushButton* plusPB = new QPushButton( "+", timeBox, "plusPB" );
  connect( plusPB, SIGNAL(pressed()), this, SLOT(inc_time()) );
  plusPB->setAutoRepeat( TRUE );
  v3->addWidget(plusPB, 0, 7 );

  QPushButton* minusPB = new QPushButton( "-", timeBox, "minusPB" );
  connect( minusPB, SIGNAL(pressed()), this, SLOT(dec_time()) );
  minusPB->setAutoRepeat( TRUE );
  v3->addWidget(minusPB, 1, 7 );

  QString wtstr = i18n("Here you can change the system time. Click into the"
    " hours, minutes or seconds field to change the relevant value, either"
    " using the up and down buttons to the right or by entering a new value.");
  QWhatsThis::add( minusPB, wtstr );
  QWhatsThis::add( plusPB, wtstr );
  QWhatsThis::add( hour, wtstr );
  QWhatsThis::add( minute, wtstr );
  QWhatsThis::add( second, wtstr );

  plusPB->setFixedSize( 20, hour->height()/2 );
  minusPB->setFixedSize( 20, hour->height()/2 );

//  v3->setColStretch( 8, 1 );

  v2->addItem( v3 );

  QHBoxLayout *top = new QHBoxLayout( this, 5 );
  top->addWidget(dateBox, 1);
  top->addWidget(timeBox, 1);

  // *************************************************************
  // End Dialog
  // *************************************************************

  month->insertItem( i18n("January") );
  month->insertItem( i18n("February") );
  month->insertItem( i18n("March") );
  month->insertItem( i18n("April") );
  month->insertItem( i18n("May") );
  month->insertItem( i18n("June") );
  month->insertItem( i18n("July") );
  month->insertItem( i18n("August") );
  month->insertItem( i18n("September") );
  month->insertItem( i18n("October") );
  month->insertItem( i18n("November") );
  month->insertItem( i18n("December") );

  connect( cal, SIGNAL(dateChanged(QDate)), SLOT(changeDate(QDate)));

  connect( &internalTimer, SIGNAL(timeout()), SLOT(timeout()) );

  load();

  if (getuid() != 0)
    {
      cal->setEnabled(false);
      month->setEnabled(false);
      year->setEnabled(false);
      hour->setReadOnly(true);
      minute->setReadOnly(true);
      second->setReadOnly(true);
      kclock->setEnabled(false);
      plusPB->setEnabled(false);
      minusPB->setEnabled(false);
    }

}

void Dtime::set_year(int y)
{
  if ( !date.setYMD(y, date.month(), date.day()) )
    date.setYMD(1970,date.month(),date.day());
  cal->setDate(date);
  emit timeChanged(TRUE);
}

void Dtime::set_time()
{
  if( ontimeout )
    return;

  internalTimer.stop();

  time.setHMS( hour->text().toInt(), minute->text().toInt(), second->text().toInt() );
  kclock->setTime( time );

  emit timeChanged( TRUE );
}

void Dtime::changeDate(QDate d)
{
  date = d;
  emit timeChanged( TRUE );
}

void Dtime::set_month(int m)
{
  if ( !date.setYMD(date.year(),m+1,date.day()) )
    date.setYMD(date.year(),m+1,1);
  cal->setDate(date);
  emit timeChanged(TRUE);
}

void Dtime::load()
{
  // Reset to the current date and time
  time = QTime::currentTime();
  date = QDate::currentDate();
  month->setCurrentItem(date.month()-1);
  year->setValue(date.year());
  cal->setDate(date);

  // start internal timer
  internalTimer.start( 1000 );

  timeout();
}

void Dtime::save()
{
  KProcess c_proc;

  BufS.sprintf("%02d%02d%02d%02d%04d.%02d",
               date.month(), date.day(),
               hour->text().toInt(), minute->text().toInt(),
               date.year(), second->text().toInt());

  kdDebug() << "Set date " << BufS << endl;

  c_proc.setExecutable( "date" );
  c_proc << BufS;
  c_proc.start( KProcess::Block );

  if ( c_proc.exitStatus() != 0 ) 
	{
    KMessageBox::error( this, i18n("Can not set date."));
    return;
  }

  // try to set hardware clock. We do not care if it fails
  KProcess hwc_proc;
  hwc_proc.setExecutable( "hwclock" );
  hwc_proc << "--systohc";
  hwc_proc.start(KProcess::Block);

  // restart time
  internalTimer.start( 1000 );
}

void Dtime::inc_time()
{
  if ( hour->hasFocus() )
    joke(hour,1,0,23,TRUE);
  if ( minute->hasFocus() )
    joke(minute,1,0,59,TRUE);
  if ( second->hasFocus() )
    joke(second,1,0,59,TRUE);
}

void Dtime::dec_time()
{
  if ( hour->hasFocus() )
    joke(hour,-1,0,23,TRUE);
  if ( minute->hasFocus() )
    joke(minute,-1,0,59,TRUE);
  if ( second->hasFocus() )
    joke(second,-1,0,59,TRUE);
}

void Dtime::joke(QLineEdit *edit,int incr,int Min,int Max,bool refr)
{
  if ( refr )
    refresh = FALSE;
  BufI=edit->text().toInt();
  BufI=BufI + incr;
  if ( BufI > Max ) BufI = Min;
  if ( BufI < Min ) BufI = Max;
  if ( Max > 99 )
    BufS.sprintf("%04d",BufI);
  else
    BufS.sprintf("%02d",BufI);
  edit->setText(BufS);
}

void Dtime::timeout()
{
  // get current time
  time = QTime::currentTime();

  ontimeout = TRUE;
  BufS.sprintf("%02d",time.second());
  second->setText(BufS);
  BufS.sprintf("%02d",time.minute());
  minute->setText(BufS);
  BufS.sprintf("%02d",time.hour());
  hour->setText(BufS);
  ontimeout = FALSE;

  kclock->setTime( time );
}

QString Dtime::quickHelp() const
{
  return i18n("<h1>Date & Time</h1> This control module can be used to set the system date and"
    " time. As these settings do not only affect you as a user, but rather the whole system, you"
    " can only change these settings when you start the Control Center as root. If you don't have"
    " the root password, but feel the system time should be corrected, please contact your system"
    " administrator.");
}

void Kclock::setTime(const QTime &time)
{
  this->time = time;
  repaint();
}

void Kclock::paintEvent( QPaintEvent * )
{
  if ( !isVisible() )
    return;

  QPainter paint;
  paint.begin( this );

  QPointArray pts;
  QPoint cp = rect().center();
  int d = QMIN(width(),height());
  paint.setPen( gray );
  paint.setBrush( gray );
  paint.setViewport(4,4,width(),height());

  for ( int c=0 ; c < 2 ; c++ )
    {
      QWMatrix matrix;
      matrix.translate( cp.x(), cp.y() );
      matrix.scale( d/1000.0F, d/1000.0F );

      // lancetta delle ore
      float h_angle = 30*(time.hour()%12-3) + time.minute()/2;
      matrix.rotate( h_angle );
      paint.setWorldMatrix( matrix );
      pts.setPoints( 4, -20,0,  0,-20, 300,0, 0,20 );
      paint.drawPolygon( pts );
      matrix.rotate( -h_angle );

      // lancetta dei minuti
      float m_angle = (time.minute()-15)*6;
      matrix.rotate( m_angle );
      paint.setWorldMatrix( matrix );
      pts.setPoints( 4, -10,0, 0,-10, 400,0, 0,10 );
      paint.drawPolygon( pts );
      matrix.rotate( -m_angle );

      // lancetta dei secondi
      float s_angle = (time.second()-15)*6;
      matrix.rotate( s_angle );
      paint.setWorldMatrix( matrix );
      pts.setPoints(4,0,0,0,0,400,0,0,0);
      paint.drawPolygon( pts );
      matrix.rotate( -s_angle );

      // quadrante
      for ( int i=0 ; i < 60 ; i++ )
        {
          paint.setWorldMatrix( matrix );
          if ( (i % 5) == 0 )
            paint.drawLine( 450,0, 500,0 ); // draw hour lines
          else  paint.drawPoint( 480,0 );   // draw second lines
          matrix.rotate( 6 );
        }

      paint.setPen( black );
      paint.setBrush( black );
      paint.setViewport(0,0,width(),height());
    }
  paint.end();
}

QValidator::State KStrictIntValidator::validate( QString & input, int & d ) const
{
  if( input.isEmpty() )
    return Valid;

  State st = QIntValidator::validate( input, d );

  if( st == Intermediate )
    return Invalid;

  return st;
}
