/*
 * Copyright (c) 1998 Stefan Taferner <taferner@kde.org>
 */
#ifndef ABOUT_H
#define ABOUT_H

#include <kcmodule.h>

class QLabel;
class QCheckBox;
class QComboBox;
class QPushButton;
class QBoxLayout;
class Theme;

class About : public KCModule
{
  Q_OBJECT
public:
  About(QWidget *parent=0, const char* name=0, bool init=FALSE);
  ~About();

protected slots:
  virtual void slotThemeChanged();

protected:
  QLabel *lblTheme, *lblAuthor, *lblVersion, *lblHomepage;
};

#endif /*ABOUT_H*/

