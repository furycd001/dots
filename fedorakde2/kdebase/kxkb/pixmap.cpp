#include <qimage.h>
#include <qfont.h>
#include <qpainter.h>


#include <kstddirs.h>
#include <klocale.h>


#include "pixmap.h"


QPixmap findPixmap(QString code)
{
  int pos=code.length();
  QString flag = locate("locale", QString("l10n/%1/flag.png").arg(code.lower()));
  if (flag.isEmpty())
    {
      pos = code.find("_");
      if (pos > 0 && code.find("intl") < 1) // so "us_intl" != "us" layout
	{
	  flag = locate("locale", QString("l10n/%1/flag.png").arg(code.mid(pos+1).lower()));
	  if (flag.isEmpty())
	    flag = locate("locale", QString("l10n/%1/flag.png").arg(code.left(pos).lower()));
	}
      //this is a tiny patch for the el (Hellenic <=> Greek) keyboard
      //which is not named after the country, but after the language 
      //unlike all others. 
      //remove if it causes trouble, but pls contact first 
      // d.kamenopoulos@mail.ntua.gr
      else if (code.lower() == QString("el"))
	flag = locate("locale", QString("l10n/gr/flag.png"));
      //end of patch
      else
	flag = locate("locale", QString("l10n/C/flag.png"));
    }

  if (flag.isEmpty())
    return QPixmap();

  QPixmap pm = QPixmap(flag);

  QImage image = pm.convertToImage();
  QRgb rgb;
  int x,y;
  for (y=0; y<image.height(); y++)
    for(x=0; x<image.width(); x++)
      {
	rgb = image.pixel(x,y);
	image.setPixel(x,y,qRgb(qRed(rgb)/2,qGreen(rgb)/2,qBlue(rgb)/2));
      }
  pm.convertFromImage(image);

  QPainter p(&pm);
  p.setFont(QFont("helvetica", 10, QFont::Bold));
  p.setPen(Qt::white);
  p.drawText(0, 0, pm.width(), pm.height()-2, Qt::AlignCenter, code.left(pos).right(2));

  return pm;
}


// Note: this seems studid, but allows for translations
void dummy()
{
  (void) i18n("Norwegian");
  (void) i18n("German");
  (void) i18n("Swedish");
  (void) i18n("Russian");
  (void) i18n("Swiss French");
  (void) i18n("Finnish");
  (void) i18n("Belgian");
  (void) i18n("Portuguese");
  (void) i18n("Thai");
  (void) i18n("Bulgarian");
  (void) i18n("Japanese");
  (void) i18n("Danish");
  (void) i18n("Lithuanian");
  (void) i18n("Italian");
  (void) i18n("United Kingdom");
  (void) i18n("French");
  (void) i18n("U.S. English w/ deadkeys");
  (void) i18n("PC-98xx Series");
  (void) i18n("Polish");
  (void) i18n("Hungarian");
  (void) i18n("Spanish");
  (void) i18n("Swiss German");
  (void) i18n("Canadian");
  (void) i18n("U.S. English");
  (void) i18n("Czech");
  (void) i18n("Czech (qwerty)");
  (void) i18n("Slovak");
  (void) i18n("Slovak (qwerty)");
  (void) i18n("Brazilian");
  (void) i18n("U.S. English w/ISO9995-3");
  (void) i18n("Estonian");
  (void) i18n("Romanian");

  //lukas: these seem to be new in XF 4.0.2
  (void) i18n("Armenian");
  (void) i18n("Azerbaidjani");
  (void) i18n("Icelandic");
  (void) i18n("Israeli");
  (void) i18n("Lithuanian azerty standard");
  (void) i18n("Lithuanian querty \"numeric\"");	     //for bw compatibility
  (void) i18n("Lithuanian querty \"programmer's\"");
  
  (void) i18n("Macedonian");
  (void) i18n("Serbian");
  (void) i18n("Slovenian");
  (void) i18n("Vietnamese");
  
  //these seem to be new in XFree86 4.1.0
  (void) i18n("Belarusian");
  (void) i18n("Latvian");
  (void) i18n("Ukrainian");
  (void) i18n("Turkish");
  (void) i18n("Croatian");
  (void) i18n("Lithuanian qwerty \"numeric\"");
  (void) i18n("Lithuanian qwerty \"programmer's\"");

  (void) i18n("Microsoft Natural");
  (void) i18n("Dell 101-key PC");
  (void) i18n("PC-98xx Series");
  (void) i18n("Winbook Model XP5");
  (void) i18n("Brazilian ABNT2");
  (void) i18n("Northgate OmniKey 101");
  (void) i18n("Generic 101-key PC");
  (void) i18n("Generic 102-key (Intl) PC");
  (void) i18n("Japanese 106-key");
  (void) i18n("Everex STEPnote");
  (void) i18n("Generic 104-key PC");
  (void) i18n("Keytronic FlexPro");
  (void) i18n("Generic 105-key (Intl) PC ");
}
