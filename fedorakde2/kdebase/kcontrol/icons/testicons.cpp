/* Test programme for icons setup module. */

#include <kapp.h>
#include "icons.h"

int main(int argc, char **argv)
{
    KApplication app(argc, argv, "testicons");
    KIconConfig *w = new KIconConfig(0L, "testicons");
    app.setMainWidget(w);
    w->show();
    return app.exec();
}
