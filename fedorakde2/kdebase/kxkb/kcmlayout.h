#ifndef __KCM_LAYOUT_H__
#define __KCM_LAYOUT_H__


class QComboBox;
class QLabel;
class QListView;
class QCheckBox;


#include <kcmodule.h>
#include <kglobalaccel.h>


#include "rules.h"


class LayoutConfig : public KCModule
{
  Q_OBJECT

public:

  LayoutConfig(QWidget *parent = 0L, const char *name = 0L);
  virtual ~LayoutConfig();

  void load();
  void save();
  void defaults();

  QString quickHelp() const;


protected slots:

  void configChanged();

  void ruleChanged(const QString &rule);


private:

  QComboBox *ruleCombo, *modelCombo, *layoutCombo;

  KeyRules *rules;

  QListView *additional;
  QCheckBox *disableCheckbox;
  KCModule *misc;
};


#endif
