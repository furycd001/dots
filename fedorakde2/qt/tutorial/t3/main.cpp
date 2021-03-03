/****************************************************************
**
** Qt tutorial 3
**
****************************************************************/

#include <qapplication.h>
#include <qpushbutton.h>
#include <qfont.h>
#include <qvbox.h>

int main( int argc, char **argv )
{
    QApplication a( argc, argv );

    QVBox box;
    box.resize( 200, 120 );

    QPushButton quit( "Quit", &box );
    quit.setFont( QFont( "Times", 18, QFont::Bold ) );

    QObject::connect( &quit, SIGNAL(clicked()), &a, SLOT(quit()) );

    a.setMainWidget( &box );
    box.show();

    return a.exec();
}
