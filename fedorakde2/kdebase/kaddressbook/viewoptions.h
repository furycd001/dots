#ifndef VIEWOPTIONS_H 
#define VIEWOPTIONS_H 

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 

#include <qdialog.h>

class QLineEdit;
class QCheckBox;
class QRadioButton;
class KColorButton;
class KURLRequester;

class ViewOptions : public QDialog
{
  Q_OBJECT

public:
  ViewOptions( bool backPixmapOn = false,
	       QString backPixmap = "",
	       bool underline = true,
	       bool autUnderline = true,
	       QColor cUnderline = QColor( 0, 0, 0),
	       bool tooltips = true,
	       QWidget * parent=0, 
	       const char * name=0, 
	       bool modal = false );

  QCheckBox *ckBackPixmap;
  KURLRequester *pixmapPath;
  QCheckBox *ckUnderline;
  QRadioButton *ckAutUnderline;
  QRadioButton *ckManUnderline;
  KColorButton *kcbUnderline;
  QCheckBox *ckTooltips;

public slots:
  void pixmapOn();
  void underlineOn();
  void autUnderlineOff();
};

#endif // PABWIDGET_H 
