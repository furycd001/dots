/****************************************************************
**
** Qt tutorial 10
**
****************************************************************/

#include <qapplication.h>
#include <qpushbutton.h>
#include <qlcdnumber.h>
#include <qfont.h>
#include <qlayout.h>

#include "lcdrange.h"
#include "cannon.h"


class MyWidget: public QWidget
{
public:
    MyWidget( QWidget *parent=0, const char *name=0 );
};


MyWidget::MyWidget( QWidget *parent, const char *name )
        : QWidget( parent, name )
{
    QPushButton *quit = new QPushButton( "&Quit", this, "quit" );
    quit->setFont( QFont( "Times", 18, QFont::Bold ) );

    connect( quit, SIGNAL(clicked()), qApp, SLOT(quit()) );

    LCDRange *angle = new LCDRange( this, "angle" );
    angle->setRange( 5, 70 );

    LCDRange *force  = new LCDRange( this, "force" );
    force->setRange( 10, 50 );

    CannonField *cannonField = new CannonField( this, "cannonField" );

    connect( angle, SIGNAL(valueChanged(int)),
	     cannonField, SLOT(setAngle(int)) );
    connect( cannonField, SIGNAL(angleChanged(int)),
	     angle, SLOT(setValue(int)) );

    connect( force, SIGNAL(valueChanged(int)),
	     cannonField, SLOT(setForce(int)) );
    connect( cannonField, SIGNAL(forceChanged(int)),
	     force, SLOT(setValue(int)) );

    QGridLayout *grid = new QGridLayout( this, 2, 2, 10 );
    grid->addWidget( quit, 0, 0 );
    grid->addWidget( cannonField, 1, 1 );
    grid->setColStretch( 1, 10 );

    QVBoxLayout *leftBox = new QVBoxLayout;
    grid->addLayout( leftBox, 1, 0 );
    leftBox->addWidget( angle );
    leftBox->addWidget( force );

    angle->setValue( 60 );
    force->setValue( 25 );
    angle->setFocus();
}

int main( int argc, char **argv )
{
    QApplication::setColorSpec( QApplication::CustomColor );
    QApplication a( argc, argv );

    MyWidget w;
    w.setGeometry( 100, 100, 500, 355 );
    a.setMainWidget( &w );
    w.show();
    return a.exec();
}
