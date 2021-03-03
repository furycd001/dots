/*****************************************************************

Copyright (c) 2000, Matthias Hoelzer-Kluepfel

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************/

#ifndef __ktipwindow_h__
#define __ktipwindow_h__


#include <qstringlist.h>


#include <kdialog.h>
#include <kuniqueapp.h>


class KTextBrowser;
class QCheckBox;
class QPushButton;


class TipWindow : public QDialog
{
  Q_OBJECT

public:

  TipWindow();

  void done( int );

public slots:

  void nextTip();
  void prevTip();
  void startupClicked();


private:

  void loadTips();

  KTextBrowser *text;
  QCheckBox    *startup;
  QPushButton  *next, *prev, *ok;

  QStringList  tips;

  int current;

};


class TipApp : public KUniqueApplication
{
  Q_OBJECT

public:

  TipApp();
  ~TipApp();

  virtual int newInstance() { window->nextTip(); return 0; };


private:

  TipWindow *window;

};


#endif
