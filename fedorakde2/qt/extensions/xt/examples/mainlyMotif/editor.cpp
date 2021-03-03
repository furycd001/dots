#include "qxt.h"
#include <qmultilineedit.h>
#include <qpainter.h>
#include <qmessagebox.h>
#include <X11/Shell.h>
#include <X11/StringDefs.h>
#include <Xm/Form.h>
#include <Xm/Text.h>


static const char* QTEDMSG =
    "This is a Qt widget.\nIt is a QMultiLineEdit.";

static const char* XTEDMSG =
    "This is an Xt widget.\nIt is an xmTextWidgetClass.";


class EncapsulatedQtWidget : public QXtWidget {
    QMultiLineEdit* mle;
public:
    EncapsulatedQtWidget(Widget parent) :
	QXtWidget("editor", parent, TRUE)
    {
	mle = new QMultiLineEdit(this);
	mle->setText(QTEDMSG);
    }

    void resizeEvent(QResizeEvent*)
    {
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

    Widget form = XtVaCreateManagedWidget("form",
		    xmFormWidgetClass, toplevel, 0);

    EncapsulatedQtWidget qtchild(form);

    const int marg=10;

    Arg args[20];
    Cardinal nargs=0;
    XtSetArg(args[nargs], XmNwidth, 200);                      nargs++;
    XtSetArg(args[nargs], XmNheight, 200);                     nargs++;
    XtSetArg(args[nargs], XmNleftOffset, marg);                nargs++;
    XtSetArg(args[nargs], XmNtopOffset, marg);                 nargs++;
    XtSetArg(args[nargs], XmNbottomOffset, marg);              nargs++;
    XtSetArg(args[nargs], XmNtopAttachment, XmATTACH_FORM);    nargs++;
    XtSetArg(args[nargs], XmNbottomAttachment, XmATTACH_FORM); nargs++;
    XtSetArg(args[nargs], XmNleftAttachment, XmATTACH_FORM);   nargs++;
    XtSetValues(qtchild.xtWidget(), args, nargs);

    nargs=0;
    XtSetArg(args[nargs], XmNeditMode, XmMULTI_LINE_EDIT);     nargs++;
    XtSetArg(args[nargs], XmNvalue, XTEDMSG);                  nargs++;    
    XtSetArg(args[nargs], XmNwidth, 200);                      nargs++;
    XtSetArg(args[nargs], XmNheight, 200);                     nargs++;
    XtSetArg(args[nargs], XmNtopOffset, marg);                 nargs++;
    XtSetArg(args[nargs], XmNbottomOffset, marg);              nargs++;
    XtSetArg(args[nargs], XmNrightOffset, marg);               nargs++;
    XtSetArg(args[nargs], XmNtopAttachment, XmATTACH_FORM);    nargs++;
    XtSetArg(args[nargs], XmNbottomAttachment, XmATTACH_FORM); nargs++;
    XtSetArg(args[nargs], XmNrightAttachment, XmATTACH_FORM);  nargs++;
    XtSetArg(args[nargs], XmNleftAttachment, XmATTACH_WIDGET); nargs++;
    XtSetArg(args[nargs], XmNleftWidget, qtchild.xtWidget());  nargs++;
    Widget xtchild = XtCreateManagedWidget("editor", xmTextWidgetClass,
	form, args, nargs);

    XtRealizeWidget(toplevel);
    XtAppMainLoop(app);
}
