#include <qmainwindow.h>
#include <qstatusbar.h>
#include <qmessagebox.h>
#include <qmenubar.h>
#include <qapplication.h>
#include <qkeycode.h>
#include <qimage.h>

#include "canvas.h"

#include <stdlib.h>

extern QString butterfly_fn;
extern QString logo_fn;

int main(int argc, char** argv)
{
    QApplication app(argc,argv);

    /*
    qDebug("sizeof(QCanvasPolygonalItem)=%d",sizeof(QCanvasPolygonalItem));
    qDebug("sizeof(QCanvasText)=%d",sizeof(QCanvasText));
    qDebug("sizeof(QWidget)=%d",sizeof(QWidget));
    qDebug("sizeof(QLabel)=%d",sizeof(QLabel));
    */

    
    
    if ( argc > 1 )
	butterfly_fn = argv[1];
    else
	butterfly_fn = "butterfly.png";
		       
    
    if ( argc > 2 )
	logo_fn = argv[2];
    else
	logo_fn = "qtlogo.png";
    
    
    QCanvas canvas(800,600);
    canvas.setAdvancePeriod(30);
    Main m(canvas);
    m.resize(m.sizeHint());

    qApp->setMainWidget(&m);

    m.setCaption("Qt Example - Canvas");
    if ( QApplication::desktop()->width() > m.width() + 10
	 && QApplication::desktop()->height() > m.height() +30 )
	m.show();
    else
	m.showMaximized();

    m.show();
    //    m.help();
    qApp->setMainWidget(0);

    QObject::connect( qApp, SIGNAL(lastWindowClosed()), qApp, SLOT(quit()) );

    return app.exec();
}

