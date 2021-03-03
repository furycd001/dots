#ifndef SELECTFIELDS_H 
#define SELECTFIELDS_H 

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 

#include <qdialog.h>
#include <qstringlist.h>
#include <qwidget.h>

class QListBox;
class QLineEdit;
class QComboBox;
class QPushButton;
class SelectFields : public QDialog
{
  Q_OBJECT

public:
  SelectFields( QStringList oldFields,
	        QWidget *parent = 0, 
		const char *name = 0, 
		bool modal = false );
  virtual QStringList chosenFields();

public slots:
  virtual void select();
  virtual void unselect();
  virtual void addCustom();
  virtual void showFields( int );
  virtual void textChanged(const QString &);
private:
  QStringList currentField;
  QListBox *lbUnSelected;
  QListBox *lbSelected;
  QLineEdit *leCustomField;
  QComboBox *cbUnselected;
  QPushButton *pbAddCustom;
  QPushButton *pbAdd;
  QPushButton *pbRemove;
};

#endif // PABWIDGET_H 
