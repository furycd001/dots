#include "kstep.h"
#include <klocale.h>

extern "C" {
    KStyle* allocate();
    int minor_version();
    int major_version();
    const char *description();
}

KStyle* allocate()
{
    return(new KStepStyle);
}

int minor_version()
{
    return(0);
}

int major_version()
{
    return(1);
}

const char *description()
{
    return(i18n("KStep unthemed plugin").utf8());
}
