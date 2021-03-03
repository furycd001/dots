// Let's crash.
#include <kapp.h>
#include <kdebug.h>
#include <stdio.h>

void level4()
{
  delete (void*)0xdead;
}

void level3()
{
  level4();
}

void level2()
{
  level3();
}

void level1()
{
  level2();
}

int main(int argc, char *argv[])
{
  KApplication app(argc,argv,"crashtest",false,false); 
  level1();
  return app.exec();
}
