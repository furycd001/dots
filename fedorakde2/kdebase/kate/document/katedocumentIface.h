#ifndef _KATEDOCUMENT_IFACE_H_
#define _KATEDOCUMENT_IFACE_H_

#include <dcopobject.h>

class KateDocumentDCOPIface : virtual public DCOPObject
{
  K_DCOP

  k_dcop:
    virtual void open (const QString &name=0)=0;
};
#endif

