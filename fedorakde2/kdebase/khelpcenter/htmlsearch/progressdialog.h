#ifndef __PROGRESS_DIALOG_H__
#define __PROGRESS_DIALOG_H__


#include <kdialogbase.h>


class QLabel;
class KProgress;


class ProgressDialog : public KDialogBase
{

  Q_OBJECT

public:

  ProgressDialog(QWidget *parent=0, const char *name=0);

  void setFilesScanned(int s);
  void setFilesToDig(int d);
  void setFilesDigged(int d);

  void setState(int n);

private:

  QLabel    *filesScanned, *check1, *check2, *check3;
  KProgress *bar;

};


#endif
