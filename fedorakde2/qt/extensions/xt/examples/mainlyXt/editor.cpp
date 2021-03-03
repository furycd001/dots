#include "qxt.h"
#include <qmultilineedit.h>
#include <qpainter.h>
#include <qmessagebox.h>
#include <X11/Shell.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/AsciiText.h>

static const char* QTEDMSG =
    "This is a Qt widget.\nIt is a QMultiLineEdit.";

static const char* XTEDMSG =
    "This is an Xt widget.\nIt is an asciiTextWidgetClass.";


class EncapsulatedQtWidget : public QXtWidget {
public:
    QMultiLineEdit* mle;
    EncapsulatedQtWidget(Widget parent) :
	QXtWidget("editor", parent, TRUE)
    {
	mle = new QMultiLineEdit(this);
	mle->setText(QTEDMSG);
    }

    void resizeEvent(QResizeEvent* e )
    {
	QXtWidget::resizeEvent( e );
	mle->resize(width(),height());
    }
};

main(int argc, char** argv)
{
    XtAppContext app;

    Widget toplevel = XtAppInitialize(
	&app, "Editors",
	0, 0, &argc, argv, 0, 0, 0);
    QXtApplication qapp(XtDisplay(toplevel));

    Widget form = XtVaCreateManagedWidget("form", formWidgetClass, toplevel, 0);

    EncapsulatedQtWidget qtchild(form);

    Arg args[20];
    Cardinal nargs=0;
    XtSetArg(args[nargs], XtNwidth, 200);                    nargs++;
    XtSetArg(args[nargs], XtNheight, 200);                   nargs++;
    XtSetValues(qtchild.xtWidget(), args, nargs);
    nargs=0;
    XtSetArg(args[nargs], XtNeditType, XawtextEdit);         nargs++;
    XtSetArg(args[nargs], XtNstring, XTEDMSG);               nargs++;
    XtSetArg(args[nargs], XtNwidth, 200);                    nargs++;
    XtSetArg(args[nargs], XtNheight, 200);                   nargs++;
    XtSetArg(args[nargs], XtNfromHoriz, qtchild.xtWidget()); nargs++;
    Widget xtchild = XtCreateManagedWidget("editor", asciiTextWidgetClass,
	form, args, nargs);

    XtRealizeWidget(toplevel);

//     XSetInputFocus( qt_xdisplay(), qtchild.mle->winId(), RevertToParent, CurrentTime );


    //XtAppMainLoop(app);

    // or the equivalent:

    XEvent xe;
    while (1)
    {
	XtAppNextEvent(app, &xe);
	XtDispatchEvent(&xe);
    }
}
