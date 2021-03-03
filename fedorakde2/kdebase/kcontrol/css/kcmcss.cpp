#include <iostream>


#include <qlayout.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qlineedit.h>
#include <qstylesheet.h>
#include <qtextbrowser.h>
#include <qcolor.h>
#include <qcombobox.h>


#include <kglobal.h>
#include <klocale.h>
#include <kstddirs.h>
#include <kconfig.h>
#include <kapp.h>
#include <kcolorbutton.h>
#include <kfontdialog.h>
#include <kurlrequester.h>

#include "cssconfig.h"
#include "template.h"
#include "preview.h"


#include "kcmcss.h"


CSSConfig::CSSConfig(QWidget *parent, const char *name)
  : KCModule(parent, name)
{
  dialog = new CSSConfigDialog(this);

  QStringList fonts;
  KFontChooser::getFontList(fonts, false);
  dialog->fontFamily->insertStringList(fonts);

  connect(dialog->useDefault, SIGNAL(clicked()),
	  this, SLOT(configChanged()));
  connect(dialog->useAccess, SIGNAL(clicked()),
	  this, SLOT(configChanged()));
  connect(dialog->useUser, SIGNAL(clicked()),
	  this, SLOT(configChanged()));
  connect(dialog->urlRequester, SIGNAL(textChanged(const QString&)),
	  this, SLOT(configChanged()));

  connect(dialog->basefontsize, SIGNAL(highlighted(int)),
          this, SLOT(configChanged()));
  connect(dialog->basefontsize, SIGNAL(textChanged(const QString&)),
	  this, SLOT(configChanged()));
  connect(dialog->dontScale, SIGNAL(clicked()),
	  this, SLOT(configChanged()));
  connect(dialog->blackOnWhite, SIGNAL(clicked()),
	  this, SLOT(configChanged()));
  connect(dialog->whiteOnBlack, SIGNAL(clicked()),
	  this, SLOT(configChanged()));
  connect(dialog->customColor, SIGNAL(clicked()),
	  this, SLOT(configChanged()));
  connect(dialog->foregroundColor, SIGNAL(changed(const QColor &)),
	  this, SLOT(configChanged()));
  connect(dialog->backgroundColor, SIGNAL(changed(const QColor &)),
	  this, SLOT(configChanged()));
  connect(dialog->fontFamily, SIGNAL(highlighted(int)),
          this, SLOT(configChanged()));
  connect(dialog->fontFamily, SIGNAL(textChanged(const QString&)),
	  this, SLOT(configChanged()));
  connect(dialog->sameFamily, SIGNAL(clicked()),
	  this, SLOT(configChanged()));
  connect(dialog->preview, SIGNAL(clicked()),
	  this, SLOT(preview()));
  connect(dialog->sameColor, SIGNAL(clicked()),
	  this, SLOT(configChanged()));
  connect(dialog->hideImages, SIGNAL(clicked()),
	  this, SLOT(configChanged()));
  connect(dialog->hideBackground, SIGNAL(clicked()),
	  this, SLOT(configChanged()));

  QVBoxLayout *vbox = new QVBoxLayout(this, 0, 0);
  vbox->addWidget(dialog);

  load();
}


void CSSConfig::load()
{
  KConfig *c = new KConfig("kcmcssrc", false, false);

  c->setGroup("Stylesheet");
  QString u = c->readEntry("Use", "default");
  dialog->useDefault->setChecked(u == "default");
  dialog->useUser->setChecked(u == "user");
  dialog->useAccess->setChecked(u == "access");
  dialog->urlRequester->setURL(c->readEntry("SheetName"));

  c->setGroup("Font");
  dialog->basefontsize->setEditText(QString::number(c->readNumEntry("BaseSize", 12)));
  dialog->dontScale->setChecked(c->readBoolEntry("DontScale", false));

  QString fname = c->readEntry("Family", "Arial");
  for (int i=0; i < dialog->fontFamily->count(); ++i)
    if (dialog->fontFamily->text(i) == fname)
      {
	dialog->fontFamily->setCurrentItem(i);
	break;
      }

  dialog->sameFamily->setChecked(c->readBoolEntry("SameFamily", false));

  c->setGroup("Colors");
  QString m = c->readEntry("Mode", "black-on-white");
  dialog->blackOnWhite->setChecked(m == "black-on-white");
  dialog->whiteOnBlack->setChecked(m == "white-on-black");
  dialog->customColor->setChecked(m == "custom");
  dialog->backgroundColor->setColor(c->readColorEntry("BackColor", &Qt::white));
  dialog->foregroundColor->setColor(c->readColorEntry("ForeColor", &Qt::black));
  dialog->sameColor->setChecked(c->readBoolEntry("SameColor", false));

  // Images
  c->setGroup("Images");
  dialog->hideImages->setChecked(c->readBoolEntry("Hide", false));
  dialog->hideBackground->setChecked(c->readBoolEntry("HideBackground", true));

  delete c;
}


void CSSConfig::save()
{
  // write to config file
  KConfig *c = new KConfig("kcmcssrc", false, false);

  c->setGroup("Stylesheet");
  if (dialog->useDefault->isChecked())
    c->writeEntry("Use", "default");
  if (dialog->useUser->isChecked())
    c->writeEntry("Use", "user");
  if (dialog->useAccess->isChecked())
    c->writeEntry("Use", "access");
  c->writeEntry("SheetName", dialog->urlRequester->url());

  c->setGroup("Font");
  c->writeEntry("BaseSize", dialog->basefontsize->currentText());
  c->writeEntry("DontScale", dialog->dontScale->isChecked());
  c->writeEntry("SameFamily", dialog->sameFamily->isChecked());
  c->writeEntry("Family", dialog->fontFamily->currentText());

  c->setGroup("Colors");
  if (dialog->blackOnWhite->isChecked())
    c->writeEntry("Mode", "black-on-white");
  if (dialog->whiteOnBlack->isChecked())
    c->writeEntry("Mode", "white-on-black");
  if (dialog->customColor->isChecked())
    c->writeEntry("Mode", "custom");
  c->writeEntry("BackColor", dialog->backgroundColor->color());
  c->writeEntry("ForeColor", dialog->foregroundColor->color());
  c->writeEntry("SameColor", dialog->sameColor->isChecked());

  c->setGroup("Images");
  c->writeEntry("Hide", dialog->hideImages->isChecked());
  c->writeEntry("HideBackground", dialog->hideBackground->isChecked());

  c->sync();
  delete c;

  // generate CSS template
  QString templ = locate("data", "kcmcss/template.css");
  QString dest;
  if (!templ.isEmpty())
    {
      CSSTemplate css(templ);

      dest = kapp->dirs()->saveLocation("data", "kcmcss");
      dest += "/override.css";

      css.expand(dest, cssDict());
    }

  // make konqueror use the right stylesheet
  c = new KConfig("konquerorrc", false, false);

  c->setGroup("HTML Settings");
  c->writeEntry("UserStyleSheetEnabled", !dialog->useDefault->isChecked());

  if (dialog->useUser->isChecked())
    c->writeEntry("UserStyleSheet", dialog->urlRequester->url());
  if (dialog->useAccess->isChecked())
    c->writeEntry("UserStyleSheet", dest);

  c->sync();
  delete c;
}


void CSSConfig::defaults()
{
  dialog->useDefault->setChecked(true);
  dialog->useUser->setChecked(false);
  dialog->useAccess->setChecked(false);
  dialog->urlRequester->setURL("");

  dialog->basefontsize->setEditText(QString::number(12));
  dialog->dontScale->setChecked(false);

  QString fname =  "Arial";
  for (int i=0; i < dialog->fontFamily->count(); ++i)
    if (dialog->fontFamily->text(i) == fname)
      {
	dialog->fontFamily->setCurrentItem(i);
	break;
      }

  dialog->sameFamily->setChecked(false);
  dialog->blackOnWhite->setChecked(true);
  dialog->whiteOnBlack->setChecked(false);
  dialog->customColor->setChecked(false);
  dialog->backgroundColor->setColor(Qt::white);
  dialog->foregroundColor->setColor(Qt::black);
  dialog->sameColor->setChecked(false);

  dialog->hideImages->setChecked(false);
  dialog->hideBackground->setChecked( true);
}


QString CSSConfig::quickHelp() const
{
  return i18n("This module allows you to apply your own color"
              " and font settings to konqueror by using"
              " stylesheets (CSS). You can either specify"
              " options or apply your own self-written"
              " stylesheet by pointing to its location.<br>"
              " Note that these settings will always have"
              " precedence before all other settings made"
              " by the site author. This can be useful to"
              " visually impaired people or for web pages"
              " that are unreadable due to bad design.");
}


void CSSConfig::configChanged()
{
  emit changed(true);
}


QString px(int i, double scale)
{
  QString px;
  px.setNum(static_cast<int>(i * scale));
  px += "px";
  return px;
}


QMap<QString,QString> CSSConfig::cssDict()
{
  QMap<QString,QString> dict;

  // Fontsizes ------------------------------------------------------

  int bfs = dialog->basefontsize->currentText().toInt();
  dict.insert("fontsize-base", px(bfs, 1.0));

  if (dialog->dontScale->isChecked())
    {
      dict.insert("fontsize-small-1", px(bfs, 1.0));
      dict.insert("fontsize-large-1", px(bfs, 1.0));
      dict.insert("fontsize-large-2", px(bfs, 1.0));
      dict.insert("fontsize-large-3", px(bfs, 1.0));
      dict.insert("fontsize-large-4", px(bfs, 1.0));
      dict.insert("fontsize-large-5", px(bfs, 1.0));
    }
  else
    {
      // TODO: use something harmonic here
      dict.insert("fontsize-small-1", px(bfs, 0.8));
      dict.insert("fontsize-large-1", px(bfs, 1.2));
      dict.insert("fontsize-large-2", px(bfs, 1.4));
      dict.insert("fontsize-large-3", px(bfs, 1.5));
      dict.insert("fontsize-large-4", px(bfs, 1.6));
      dict.insert("fontsize-large-5", px(bfs, 1.8));
    }

  // Colors --------------------------------------------------------

  if (dialog->blackOnWhite->isChecked())
    {
      dict.insert("background-color", "White");
      dict.insert("foreground-color", "Black");
    }
  else if (dialog->whiteOnBlack->isChecked())
    {
      dict.insert("background-color", "Black");
      dict.insert("foreground-color", "White");
    }
  else
    {
      dict.insert("background-color", dialog->backgroundColor->color().name());
      dict.insert("foreground-color", dialog->foregroundColor->color().name());
    }

  if (dialog->sameColor->isChecked())
    dict.insert("force-color", "! important");
  else
    dict.insert("force-color", "");

  // Fonts -------------------------------------------------------------
  dict.insert("font-family", dialog->fontFamily->currentText());
  if (dialog->sameFamily->isChecked())
    dict.insert("force-font", "! important");
  else
    dict.insert("force-font", "");

  // Images

  if (dialog->hideImages->isChecked())
    dict.insert("display-images", "background-image : none ! important");
  else
    dict.insert("display-images", "");
  if (dialog->hideBackground->isChecked())
    dict.insert("display-background", "background-image : none ! important");
  else
    dict.insert("display-background", "");

  return dict;
}


void CSSConfig::preview()
{

  QStyleSheetItem *h1 = new QStyleSheetItem(QStyleSheet::defaultSheet(), "h1");
  QStyleSheetItem *h2 = new QStyleSheetItem(QStyleSheet::defaultSheet(), "h2");
  QStyleSheetItem *h3 = new QStyleSheetItem(QStyleSheet::defaultSheet(), "h3");
  QStyleSheetItem *text = new QStyleSheetItem(QStyleSheet::defaultSheet(), "p");

  // Fontsize

  int bfs = dialog->basefontsize->currentText().toInt();
  text->setFontSize(bfs);
  if (dialog->dontScale->isChecked())
    {
      h1->setFontSize(bfs);
      h2->setFontSize(bfs);
      h3->setFontSize(bfs);
    }
  else
    {
      h1->setFontSize(static_cast<int>(bfs * 1.8));
      h2->setFontSize(static_cast<int>(bfs * 1.6));
      h3->setFontSize(static_cast<int>(bfs * 1.4));
    }

  // Colors

  QColor back, fore;

  if (dialog->blackOnWhite->isChecked())
    {
      back = Qt::white;
      fore = Qt::black;
    }
  else if (dialog->whiteOnBlack->isChecked())
    {
      back = Qt::black;
      fore = Qt::white;
    }
  else
    {
      back = dialog->backgroundColor->color();
      fore = dialog->foregroundColor->color();
    }

  h1->setColor(fore);
  h2->setColor(fore);
  h3->setColor(fore);
  text->setColor(fore);

  // Fonts

  h1->setFontFamily(dialog->fontFamily->currentText());
  h2->setFontFamily(dialog->fontFamily->currentText());
  h3->setFontFamily(dialog->fontFamily->currentText());
  text->setFontFamily(dialog->fontFamily->currentText());

  // Show the preview
  PreviewDialog *dlg = new PreviewDialog(this, 0, true);
  QColorGroup clgrp = dlg->preview->colorGroup();
  clgrp.setColor(QColorGroup::Base, back);
  dlg->preview->setPaperColorGroup(clgrp);
  dlg->preview->viewport()->setFont(QFont("helvetica", bfs));

  dlg->exec();

  delete dlg;
}


extern "C" {

  KCModule *create_kcmcss(QWidget *parent, const char *name)
  {
    KGlobal::locale()->insertCatalogue("kcmcss");
    return new CSSConfig(parent, name);
  }

}

#include "kcmcss.moc"

