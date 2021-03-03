#include <qapplication.h>
#include <qwidget.h>
#include "kpopupmenu.h"

class DemoWidget : public QWidget {
private:
    KPopupMenu *menu;
    
void mousePressEvent(QMouseEvent *)
{
    menu->popup(QCursor::pos());
}

void paintEvent(QPaintEvent *)
{
    drawText(32, 32, "Press a Mouse Button!");
}

public:
    DemoWidget() : QWidget()
    {
        menu = new KPopupMenu("Popup Menu:");
        menu->insertItem("Item1");
        menu->insertItem("Item2");
        menu->insertSeparator();
        menu->insertItem("Quit", qApp, SLOT(quit()));
    }       
};

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    DemoWidget w;
    app.setMainWidget(&w);
    app.setFont(QFont("helvetica", 12, QFont::Bold), TRUE);
    w.show();
    return app.exec();
}
    
