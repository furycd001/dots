/*
 * Copyright (c) 1998 Stefan Taferner <taferner@kde.org>
 */
#ifndef OPTIONS_H
#define OPTIONS_H

#include <kcmodule.h>

class QLabel;
class QCheckBox;
class QComboBox;
class QPushButton;
class QBoxLayout;
class QGridLayout;

#define OptionsInherited KCModule
class Options : public KCModule
{
  Q_OBJECT
public:
  Options(QWidget *parent=0, const char* name=0, bool init=FALSE);
  ~Options();

  virtual void load();
  virtual void save();

  /** Update status information on available groups of current theme. */
  virtual void updateStatus(void);

protected slots:
  virtual void slotThemeChanged();
  virtual void slotThemeApply();
  virtual void slotCbxClicked();
  virtual void slotDetails();
  virtual void slotInvert();
  virtual void slotClear();

protected:
  /** Creates a new options line */
  virtual QCheckBox* newLine(const char* groupName, const QString& text,
			     QLabel** statusPtr);

  virtual void readConfig();
  virtual void writeConfig();

  virtual void updateStatus(const char* groupName, QLabel* status);

protected:
  QCheckBox *mCbxColors;
  QCheckBox *mCbxWallpapers;
  QCheckBox *mCbxSounds;
  QCheckBox *mCbxIcons;
  QCheckBox *mCbxWM;
  QCheckBox *mCbxPanel;
  QCheckBox *mCbxOverwrite;
  QLabel *mStatColors;
  QLabel *mStatWallpapers;
  QLabel *mStatSounds;
  QLabel *mStatIcons;
  QLabel *mStatWM;
  QLabel *mStatPanel;
  QGridLayout *mGrid;
  bool mGui;
  int mGridRow;
};

#endif /*OPTIONS_H*/

