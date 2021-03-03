

#include <qobject.h>

#include "kio_man.h"


#include <kapp.h>
#include <klocale.h>


class kio_man_test : public  MANProtocol
{
  Q_OBJECT

public:
  kio_man_test(const QCString &pool_socket, const QCString &app_socket);

protected:
  virtual void data(int);

};





int main(int argc, char **argv)
{
  KApplication a( argc, argv , "p2");

  MANProtocol testproto("/tmp/kiotest.in", "/tmp/kiotest.out");
  testproto.showIndex("3");

  return 0;
}


