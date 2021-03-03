#include "viewoptions.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>

#include <kcolorbtn.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <kseparator.h>
#include <kurlrequester.h>
#include <klineedit.h>

ViewOptions::ViewOptions( bool backPixmapOn,
			  QString backPixmap,
			  bool underline,
			  bool autUnderline,
			  QColor cUnderline,
			  bool tooltips,
			  QWidget *parent,
			  const char *name,
			  bool modal )
  : QDialog( parent, name, modal )
{
  setCaption( i18n( "Viewing Options" ));

  QBoxLayout *vb = new QBoxLayout( this, QBoxLayout::TopToBottom, 10 );

  QBoxLayout *hb1 = new QBoxLayout( vb, QBoxLayout::LeftToRight, 10 );
  ckBackPixmap = new QCheckBox( i18n( "Show &Pixmap" ), this );
  ckBackPixmap->setChecked( backPixmapOn );
  hb1->addWidget( ckBackPixmap );
  QLabel *lBackPixmap = new QLabel( i18n( "&Location" ), this );
  hb1->addWidget( lBackPixmap );



  pixmapPath=new KURLRequester( this );
  hb1->addWidget( pixmapPath );

  pixmapPath->lineEdit()->setText(backPixmap);

  connect( pixmapPath, SIGNAL( textChanged( const QString& ) ),
           this, SLOT( pixmapOn() ));

  KSeparator* bar1 = new KSeparator( KSeparator::HLine, this);
  bar1->setFixedHeight( 10 );
  vb->addWidget( bar1 );

  QBoxLayout *hb2 = new QBoxLayout( vb, QBoxLayout::LeftToRight, 10 );
  ckUnderline = new QCheckBox( i18n( "Show &Underline" ), this );
  ckUnderline->setChecked( underline );
  hb2->addWidget( ckUnderline );
  QButtonGroup *bg = new QButtonGroup( this, 0 );
  bg->setFrameStyle( QFrame::NoFrame );
  QBoxLayout *hb2a = new QBoxLayout( bg, QBoxLayout::LeftToRight, 0 );
  ckAutUnderline = new QRadioButton( i18n( "&Automatic" ), bg );
  hb2a->addWidget( ckAutUnderline );
  hb2a->addSpacing( 20 );
  ckManUnderline = new QRadioButton( i18n( "&Manual" ), bg );
  hb2a->addWidget( ckManUnderline );
  ckAutUnderline->setChecked( autUnderline );
  ckManUnderline->setChecked( !autUnderline );
  hb2->addWidget( bg );
  kcbUnderline = new KColorButton( cUnderline, this );
  hb2->addWidget( kcbUnderline );
  hb2->addStretch();
  QObject::connect( kcbUnderline, SIGNAL( changed( const QColor& )),
		    this, SLOT( underlineOn() ));
  QObject::connect( kcbUnderline, SIGNAL( changed( const QColor& )),
		    this, SLOT( autUnderlineOff() ));
  QObject::connect( ckAutUnderline, SIGNAL( stateChanged( int )),
		    this, SLOT( underlineOn() ));

  KSeparator* bar2 = new KSeparator( KSeparator::HLine, this);
  bar2->setFixedHeight( 10 );
  vb->addWidget( bar2 );

  QBoxLayout *hb3 = new QBoxLayout( vb, QBoxLayout::LeftToRight, 10 );
  ckTooltips = new QCheckBox( i18n( "Show &Tooltips" ), this );
  ckTooltips->setChecked( tooltips );
  hb3->addWidget( ckTooltips );
  hb3->addStretch();

  KSeparator* bar3 = new KSeparator( KSeparator::HLine, this);
  bar3->setFixedHeight( 10 );
  vb->addWidget( bar3 );

  QBoxLayout *hb4 = new QBoxLayout( vb, QBoxLayout::LeftToRight, 10 );
  hb4->addStretch();
  QPushButton *pbOk = new QPushButton( i18n("&OK"), this );
  QObject::connect( pbOk, SIGNAL( clicked() ), this, SLOT( accept() ));
  pbOk->setFocus();
  hb4->addWidget( pbOk );
  QPushButton *pbCancel = new QPushButton( i18n("Cancel"), this );
  QObject::connect( pbCancel, SIGNAL( clicked() ), this, SLOT( reject() ));
  hb4->addWidget( pbCancel );

  vb->activate();
}

void ViewOptions::pixmapOn()
{
  ckBackPixmap->setChecked( TRUE );
}

void ViewOptions::underlineOn()
{
  ckUnderline->setChecked( TRUE );
}

void ViewOptions::autUnderlineOff()
{
  ckManUnderline->setChecked( TRUE );
}

#include "viewoptions.moc"
