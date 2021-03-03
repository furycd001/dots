/*
 *  lookandfeeltab.cpp
 *
 *  Copyright (c) 2000 Matthias Elter <elter@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 */

#include <qlayout.h>
#include <qgroupbox.h>
#include <qwhatsthis.h>
#include <qcheckbox.h>
#include <qframe.h>
#include <qvbox.h>
#include <qhbox.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qfileinfo.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qpixmap.h>
#include <qimage.h>

#include <kconfig.h>
#include <kdialog.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <klocale.h>
#include <kcombobox.h>
#include <kimageio.h>
#include <kiconeffect.h>
#include <kfiledialog.h>
#include <kmessagebox.h>

#include "lookandfeeltab_impl.h"
#include "lookandfeeltab_impl.moc"


extern int kickerconfig_screen_number;

LookAndFeelTab::LookAndFeelTab( QWidget *parent, const char* name )
  : LookAndFeelTabBase (parent, name)
{
  connect(tiles_cb, SIGNAL(clicked()), SLOT(tiles_clicked()));

  QWhatsThis::add( tiles_cb, i18n("If this option is enabled, the panel will display"
    " panel buttons using tile images instead of just drawing flat buttons. You can still"
    " enable or disable usage of tiles for the different kinds of panel buttons, using the"
    " options below.") );

  connect(zoom_cb, SIGNAL(clicked()), SIGNAL(changed() ) );

  QWhatsThis::add( zoom_cb, i18n("If this option is enabled, the button icons"
     " are zoomed when the mouse cursor  is moved over them."));

  connect(kmenu_cb, SIGNAL(clicked()), SLOT(kmenu_clicked()));
  kmenu_input->setInsertionPolicy(QComboBox::NoInsertion);
  connect(kmenu_input, SIGNAL(activated(const QString&)), SLOT(kmenu_changed(const QString&)));
  QWhatsThis::add( kmenu_cb, i18n("Enable or disable the usage of a tile image for the K menu.") );
  QWhatsThis::add( kmenu_input, i18n("Choose a tile image for the K menu."));
  QWhatsThis::add( kmenu_label, i18n("This is a preview of the tile that will be used for the K menu.") );

  connect(browser_cb, SIGNAL(clicked()), SLOT(browser_clicked()));
  browser_input->setInsertionPolicy(QComboBox::NoInsertion);
  connect(browser_input, SIGNAL(activated(const QString&)), SLOT(browser_changed(const QString&)));
  QWhatsThis::add( browser_cb, i18n("Enable or disable the usage of tile images for Quick Browser buttons.") );
  QWhatsThis::add( browser_input, i18n("Choose a tile image for Quick Browser buttons."));
  QWhatsThis::add( browser_label, i18n("This is a preview of the tile that will be used for Quick Browser buttons.") );

  connect(url_cb, SIGNAL(clicked()), SLOT(url_clicked()));
  url_input->setInsertionPolicy(QComboBox::NoInsertion);
  connect(url_input, SIGNAL(activated(const QString&)), SLOT(url_changed(const QString&)));
  QWhatsThis::add( url_cb, i18n("Enable or disable the usage of a tile image for buttons that launch applications.") );
  QWhatsThis::add( url_input, i18n("Choose a tile image for buttons that launch applications."));
  QWhatsThis::add( url_label, i18n("This is a preview of the tile that will be used for buttons that launch applications.") );

  connect(exe_cb, SIGNAL(clicked()), SLOT(exe_clicked()));
  exe_input->setInsertionPolicy(QComboBox::NoInsertion);
  connect(exe_input, SIGNAL(activated(const QString&)), SLOT(exe_changed(const QString&)));
  QWhatsThis::add( exe_cb, i18n("Enable or disable the usage of tile images for legacy application buttons.") );
  QWhatsThis::add( exe_input, i18n("Choose a tile image for legacy application buttons."));
  QWhatsThis::add( exe_label, i18n("This is a preview of the tile that will be used for legacy application buttons.") );

  connect(wl_cb, SIGNAL(clicked()), SLOT(wl_clicked()));
  wl_input->setInsertionPolicy(QComboBox::NoInsertion);
  connect(wl_input, SIGNAL(activated(const QString&)), SLOT(wl_changed(const QString&)));
  QWhatsThis::add( wl_cb, i18n("Enable or disable the usage of tile images for window list buttons.") );
  QWhatsThis::add( wl_input, i18n("Choose a tile image for window list buttons."));
  QWhatsThis::add( wl_label, i18n("This is a preview of the tile that will be used for window list buttons.") );

  connect(desktop_cb, SIGNAL(clicked()), SLOT(desktop_clicked()));
  desktop_input->setInsertionPolicy(QComboBox::NoInsertion);
  connect(desktop_input, SIGNAL(activated(const QString&)), SLOT(desktop_changed(const QString&)));
  QWhatsThis::add( desktop_cb, i18n("Enable or disable the usage of tile images for desktop access buttons.") );
  QWhatsThis::add( desktop_input, i18n("Choose a tile image for desktop access buttons."));
  QWhatsThis::add( desktop_label, i18n("This is a preview of the tile that will be used for desktop access buttons.") );

  connect(m_backgroundImage, SIGNAL(clicked()), SIGNAL(changed()));
  connect(m_backgroundButton, SIGNAL(clicked()), SLOT(browse_theme()));

    QWhatsThis::add(m_backgroundImage, i18n("If this option is selected, you "
                                            "can choose a background image that will be displayed on the "
                                            "panel. If it is not selected, the default colors will be used, "
                                            "see the 'Colors' control module."));

    QWhatsThis::add(m_backgroundLabel, i18n("This is a preview for the selected background image."));

    QString wtstr = i18n("Here you can choose a theme to be displayed by the panel. "
                         "Press the 'Browse' button to choose a theme using the file dialog.<p> "
                         "This option is only active if 'Use background theme' is selected.");
    QWhatsThis::add(m_backgroundButton, wtstr );
    QWhatsThis::add(m_backgroundInput, wtstr );

    m_backgroundInput->setReadOnly(true);

    connect(m_showToolTips, SIGNAL(clicked()), SIGNAL(changed()));

  fill_tile_input();
  load();
}

void LookAndFeelTab::browse_theme()
{
    QString newtheme = KFileDialog::getOpenFileName(QString::null
                                                    , KImageIO::pattern(KImageIO::Reading)
                                                    , 0, i18n("Select an image file"));
    if (theme == newtheme) return;
    if (newtheme.isEmpty()) return;

    QImage tmpImg(newtheme);
    if( !tmpImg.isNull() ) {
        tmpImg = tmpImg.smoothScale(m_backgroundLabel->contentsRect().width(),
                                    m_backgroundLabel->contentsRect().height());
        theme_preview.convertFromImage(tmpImg);
        if( !theme_preview.isNull() ) {
            theme = newtheme;
            m_backgroundInput->setText(theme);
            m_backgroundLabel->setPixmap(theme_preview);
            emit changed();
            return;
        }
    }

    KMessageBox::error(this, i18n("Failed to load image file."), i18n("Failed to load image file."));
}

void LookAndFeelTab::tiles_clicked()
{
  bool enabled = tiles_cb->isChecked();

  kmenu_group->setEnabled(enabled);
  url_group->setEnabled(enabled);
  exe_group->setEnabled(enabled);
  browser_group->setEnabled(enabled);
  wl_group->setEnabled(enabled);
  desktop_group->setEnabled(enabled);
  emit changed();
}

void LookAndFeelTab::kmenu_clicked()
{
  bool enabled = kmenu_cb->isChecked();
  kmenu_label->setEnabled(enabled);
  kmenu_input->setEnabled(enabled);
  emit changed();
}

void LookAndFeelTab::kmenu_changed(const QString& t)
{
  setLabel( kmenu_label, t );
  emit changed();
}

void LookAndFeelTab::url_clicked()
{
  bool enabled = url_cb->isChecked();
  url_input->setEnabled(enabled);
  url_label->setEnabled(enabled);
  emit changed();
}
void LookAndFeelTab::url_changed(const QString& t)
{
  setLabel( url_label, t );
  emit changed();
}

void LookAndFeelTab::browser_clicked()
{
  bool enabled = browser_cb->isChecked();
  browser_input->setEnabled(enabled);
  browser_label->setEnabled(enabled);
  emit changed();
}
void LookAndFeelTab::browser_changed(const QString& t)
{
  setLabel( browser_label, t );
  emit changed();
}

void LookAndFeelTab::exe_clicked()
{
  bool enabled = exe_cb->isChecked();
  exe_input->setEnabled(enabled);
  exe_label->setEnabled(enabled);
  emit changed();
}
void LookAndFeelTab::exe_changed(const QString& t)
{
  setLabel( exe_label, t );
  emit changed();
}

void LookAndFeelTab::wl_clicked()
{
  bool enabled = wl_cb->isChecked();
  wl_input->setEnabled(enabled);
  wl_label->setEnabled(enabled);
  emit changed();
}
void LookAndFeelTab::wl_changed(const QString& t)
{
  setLabel( wl_label, t );
  emit changed();
}

void LookAndFeelTab::desktop_clicked()
{
  bool enabled = desktop_cb->isChecked();
  desktop_input->setEnabled(enabled);
  desktop_label->setEnabled(enabled);
  emit changed();
}
void LookAndFeelTab::desktop_changed(const QString& t)
{
  setLabel( desktop_label, t );
  emit changed();
}

void LookAndFeelTab::setLabel( QLabel *label, const QString &t )
{
  QString tile = t + "_large_up.png";
  tile = KGlobal::dirs()->findResource("tiles", tile);

  if(!tile.isNull())
    {
      QPixmap pix(tile);
      if (!pix.isNull())
        label->setPixmap(pix);
      else
        label->clear();
    }
  else
    label->clear();
}

void LookAndFeelTab::load()
{
  QCString configname;
  if (kickerconfig_screen_number == 0)
      configname = "kickerrc";
  else
      configname.sprintf("kicker-screen-%drc", kickerconfig_screen_number);
  KConfig *c = new KConfig(configname, false, false);

    c->setGroup("General");

    bool use_theme = c->readBoolEntry("UseBackgroundTheme", false);
    theme = c->readEntry("BackgroundTheme", QString::null);

    m_backgroundImage->setChecked(use_theme);
    m_backgroundInput->setEnabled(use_theme);
    m_backgroundLabel->setEnabled(use_theme);
    m_backgroundButton->setEnabled(use_theme);

    if (theme != QString::null) {
        QString themepath;
        if (theme[0] == '/')
            themepath = theme;
        else
            themepath = locate("data", "kicker/"+theme);
        QImage tmpImg(themepath);
        if(!tmpImg.isNull()) {
            tmpImg = tmpImg.smoothScale(m_backgroundLabel->contentsRect().width(),
                                        m_backgroundLabel->contentsRect().height());
            theme_preview.convertFromImage(tmpImg);
            if(!theme_preview.isNull()) {
                m_backgroundInput->setText(theme);
                m_backgroundLabel->setPixmap(theme_preview);
            }
            else
                m_backgroundInput->setText(i18n("Error loading theme image file."));
        }
        else
            m_backgroundInput->setText(i18n("Error loading theme image file."));
    }

    m_showToolTips->setChecked( c->readBoolEntry( "ShowToolTips", true ) );

  c->setGroup("buttons");

  bool tiles = c->readBoolEntry("EnableTileBackground", false);
  tiles_cb->setChecked(tiles);

  kmenu_group->setEnabled(tiles);
  url_group->setEnabled(tiles);
  exe_group->setEnabled(tiles);
  browser_group->setEnabled(tiles);
  wl_group->setEnabled(tiles);
  desktop_group->setEnabled(tiles);

  bool zoom = c->readBoolEntry("EnableIconZoom", true);
  zoom_cb->setChecked(zoom);

  c->setGroup("button_tiles");

  bool kmenu_tiles = c->readBoolEntry("EnableKMenuTiles", true);
  kmenu_cb->setChecked(kmenu_tiles);
  kmenu_input->setEnabled(kmenu_tiles);
  kmenu_label->setEnabled(kmenu_tiles);

  bool url_tiles = c->readBoolEntry("EnableURLTiles", true);
  url_cb->setChecked(url_tiles);
  url_input->setEnabled(url_tiles);
  url_label->setEnabled(url_tiles);

  bool browser_tiles = c->readBoolEntry("EnableBrowserTiles", true);
  browser_cb->setChecked(browser_tiles);
  browser_input->setEnabled(browser_tiles);
  browser_label->setEnabled(browser_tiles);

  bool exe_tiles = c->readBoolEntry("EnableExeTiles", true);
  exe_cb->setChecked(exe_tiles);
  exe_input->setEnabled(exe_tiles);
  exe_label->setEnabled(exe_tiles);

  bool wl_tiles = c->readBoolEntry("EnableWindowListTiles", true);
  wl_cb->setChecked(wl_tiles);
  wl_input->setEnabled(wl_tiles);
  wl_label->setEnabled(wl_tiles);

  bool desktop_tiles = c->readBoolEntry("EnableDesktopButtonTiles", true);
  desktop_cb->setChecked(desktop_tiles);
  desktop_input->setEnabled(desktop_tiles);
  desktop_label->setEnabled(desktop_tiles);

  // set kmenu tile
  QString tile = c->readEntry("KMenuTile", "solid_blue");
  int index = 0;

  for (int i = 0; i < kmenu_input->count(); i++) {
    if (tile == kmenu_input->text(i)) {
      index = i;
      break;
    }
  }
  kmenu_input->setCurrentItem(index);
  kmenu_changed(kmenu_input->text(index));

  // set url tile
  tile = c->readEntry("URLTile", "solid_gray");
  index = 0;

  for (int i = 0; i < url_input->count(); i++) {
    if (tile == url_input->text(i)) {
      index = i;
      break;
    }
  }
  url_input->setCurrentItem(index);
  url_changed(url_input->text(index));

  // set browser tile
  tile = c->readEntry("BrowserTile", "solid_green");
  index = 0;

  for (int i = 0; i < browser_input->count(); i++) {
    if (tile == browser_input->text(i)) {
      index = i;
      break;
    }
  }
  browser_input->setCurrentItem(index);
  browser_changed(browser_input->text(index));

  // set exe tile
  tile = c->readEntry("ExeTile", "solid_red");
  index = 0;

  for (int i = 0; i < exe_input->count(); i++) {
    if (tile == exe_input->text(i)) {
      index = i;
      break;
    }
  }
  exe_input->setCurrentItem(index);
  exe_changed(exe_input->text(index));

  // set window list tile
  tile = c->readEntry("WindowListTile", "solid_green");
  index = 0;

  for (int i = 0; i < wl_input->count(); i++) {
    if (tile == wl_input->text(i)) {
      index = i;
      break;
    }
  }
  wl_input->setCurrentItem(index);
  wl_changed(wl_input->text(index));

  // set desktop tile
  tile = c->readEntry("DesktopButtonTile", "solid_orange");
  index = 0;

  for (int i = 0; i < desktop_input->count(); i++) {
    if (tile == desktop_input->text(i)) {
      index = i;
      break;
    }
  }
  desktop_input->setCurrentItem(index);
  desktop_changed(desktop_input->text(index));

  delete c;
}

void LookAndFeelTab::save()
{
  QCString configname;
  if (kickerconfig_screen_number == 0)
      configname = "kickerrc";
  else
      configname.sprintf("kicker-screen-%drc", kickerconfig_screen_number);
  KConfig *c = new KConfig(configname, false, false);

  c->setGroup("General");
  c->writeEntry("UseBackgroundTheme", m_backgroundImage->isChecked());
  c->writeEntry("BackgroundTheme", theme);
  c->writeEntry( "ShowToolTips", m_showToolTips->isChecked() );

  c->setGroup("buttons");

  c->writeEntry("EnableTileBackground", tiles_cb->isChecked());
  c->writeEntry("EnableIconZoom", zoom_cb->isChecked());

  c->setGroup("button_tiles");
  c->writeEntry("EnableKMenuTiles", kmenu_cb->isChecked());
  c->writeEntry("EnableURLTiles", url_cb->isChecked());
  c->writeEntry("EnableBrowserTiles", browser_cb->isChecked());
  c->writeEntry("EnableExeTiles", exe_cb->isChecked());
  c->writeEntry("EnableWindowListTiles", wl_cb->isChecked());
  c->writeEntry("EnableDesktopButtonTiles", desktop_cb->isChecked());

  c->writeEntry("KMenuTile", kmenu_input->currentText());
  c->writeEntry("URLTile", url_input->currentText());
  c->writeEntry("BrowserTile", browser_input->currentText());
  c->writeEntry("ExeTile", exe_input->currentText());
  c->writeEntry("WindowListTile", wl_input->currentText());
  c->writeEntry("DesktopButtonTile", desktop_input->currentText());

  c->sync();

  delete c;
}

void LookAndFeelTab::defaults()
{
  tiles_cb->setChecked(false);

  kmenu_group->setEnabled(false);
  url_group->setEnabled(false);
  exe_group->setEnabled(false);
  browser_group->setEnabled(false);
  wl_group->setEnabled(false);
  desktop_group->setEnabled(false);

  kmenu_cb->setChecked(true);
  url_cb->setChecked(true);
  browser_cb->setChecked(true);
  exe_cb->setChecked(true);
  wl_cb->setChecked(true);
  desktop_cb->setChecked(true);
  zoom_cb->setChecked(true);

  theme = QString::null;

  m_backgroundImage->setChecked(false);
  m_backgroundInput->setText(theme);
  m_backgroundLabel->clear();

  m_backgroundInput->setEnabled(false);
  m_backgroundLabel->setEnabled(false);
  m_backgroundButton->setEnabled(false);
  m_showToolTips->setChecked(true);

}

void LookAndFeelTab::fill_tile_input()
{
  tiles = queryAvailableTiles();

  kmenu_input->clear();
  url_input->clear();
  browser_input->clear();
  exe_input->clear();
  wl_input->clear();
  desktop_input->clear();

  kmenu_input->insertStringList(tiles);
  url_input->insertStringList(tiles);
  browser_input->insertStringList(tiles);
  exe_input->insertStringList(tiles);
  wl_input->insertStringList(tiles);
  desktop_input->insertStringList(tiles);
}

QStringList LookAndFeelTab::queryAvailableTiles()
{
  QStringList list = KGlobal::dirs()->findAllResources("tiles","*_large_up.png");
  QStringList list2;

  for (QStringList::Iterator it = list.begin(); it != list.end(); ++it)
    {
      QString tile = (*it);
      QFileInfo fi(tile);
      tile = fi.fileName();
      tile.truncate(tile.find("_large_up.png"));
      list2.append(tile);
    }
  list2.sort();
  return list2;
}

QString LookAndFeelTab::quickHelp() const
{
  return i18n("");
}
