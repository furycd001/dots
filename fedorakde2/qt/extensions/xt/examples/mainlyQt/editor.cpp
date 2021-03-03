#include "qxt.h"
#include <qmainwindow.h>
#include <qmultilineedit.h>
#include <qpainter.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qsplitter.h>
#include <X11/IntrinsicP.h>
#include <X11/Shell.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/AsciiText.h>

static const char* QTEDMSG =
    "This is a Qt widget.\nIt is a QMultiLineEdit.";

static const char* XTEDMSG =
    "This is an Xt widget.\nIt is an asciiTextWidgetClass.";


class EncapsulatedXtWidget : public QXtWidget {
    Widget editor;
public:
    EncapsulatedXtWidget(QWidget* parent) :
	QXtWidget( "shell", topLevelShellWidgetClass, parent )
    {
 	Arg args[20];
 	Cardinal nargs=0;
 	XtSetArg(args[nargs], XtNeditType, XawtextEdit); nargs++;
 	XtSetArg(args[nargs], XtNstring, XTEDMSG);       nargs++;
 	editor = XtCreateWidget( "editor", asciiTextWidgetClass, xtWidget(), args, nargs);
	XtRealizeWidget( editor );
 	XtMapWidget( editor );
    }
    void resizeEvent( QResizeEvent* e )
    {
	QXtWidget::resizeEvent( e );
 	XtResizeWidget( editor, width(), height(), 2 );
    }
};


class TwoEditors : public QMainWindow {
    QMultiLineEdit* qtchild;
    EncapsulatedXtWidget* xtchild;

public:
    TwoEditors() :
	QMainWindow( 0, "mainWindow")
    {
	QPopupMenu* file = new QPopupMenu( this );
	file->insertItem("E&xit", qApp, SLOT( quit() ) );
	menuBar()->insertItem( "&File", file );
	statusBar();
	QSplitter* splitter = new QSplitter( this );
	splitter->setOpaqueResize( TRUE );
	setCentralWidget( splitter );
	xtchild = new EncapsulatedXtWidget( splitter );
	qtchild = new QMultiLineEdit( splitter );
	qtchild->setText(QTEDMSG);
    }

};

main(int argc, char** argv)
{
    QXtApplication app(argc, argv, "TwoEditors");
    TwoEditors m;
    app.setMainWidget(&m);
    m.show();
    return app.exec();
}
