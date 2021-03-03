/*
 * installer.cpp
 *
 * Copyright (c) 1998 Stefan Taferner <taferner@kde.org>
 *
 * Requires the Qt widget libraries, available at no cost at
 * http://www.troll.no/
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
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#include <qdir.h>
#include <qbuttongroup.h>
#include <qframe.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qfileinfo.h>
#include <qmultilineedit.h>

#include <unistd.h>
#include <stdlib.h>

#include "installer.h"
#include "themecreator.h"
#include "global.h"
#include "newthemedlg.h"

#include <kbuttonbox.h>
#include <klistbox.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <kglobal.h>
#include <kglobalsettings.h>
#include <kdebug.h>
#include <kstddirs.h>
#include <kmessagebox.h>
#include <kurldrag.h>
#include <kio/netaccess.h>

static QString findThemePath(QString name)
{
    if (name.isEmpty())
       return QString::null;

    name = Theme::removeExtension(name);
    QStringList paths = KGlobal::dirs()->findAllResources("themes", name+".*", false, true);
    if (paths.isEmpty())
       return QString::null;
    return paths[0];
}

ThemeListBox::ThemeListBox(QWidget *parent)
  : KListBox(parent)
{
   setAcceptDrops(true);
   connect(this, SIGNAL(mouseButtonPressed(int, QListBoxItem *, const QPoint &)),
           this, SLOT(slotMouseButtonPressed(int, QListBoxItem *, const QPoint &)));
}

void ThemeListBox::dragEnterEvent(QDragEnterEvent* event)
{
   event->accept((event->source() != this) && QUriDrag::canDecode(event));
}

void ThemeListBox::dropEvent(QDropEvent* event)
{
   KURL::List urls;
   if (KURLDrag::decode(event, urls))
   {
      emit filesDropped(urls);
   }
}

void ThemeListBox::slotMouseButtonPressed(int button, QListBoxItem *item, const QPoint &p)
{
   if ((button & LeftButton) == 0) return;
   mOldPos = p;
   mDragFile = QString::null;
   int cur = index(item);
   if (cur >= 0)
   {
      QString themeName = text(cur);
      mDragFile = findThemePath(themeName);
   }
}

void ThemeListBox::mouseMoveEvent(QMouseEvent *e)
{
   if (((e->state() & LeftButton) != 0) && !mDragFile.isEmpty())
   {
      int delay = KGlobalSettings::dndEventDelay();
      QPoint newPos = e->globalPos();
      if(newPos.x() > mOldPos.x()+delay || newPos.x() < mOldPos.x()-delay ||
         newPos.y() > mOldPos.y()+delay || newPos.y() < mOldPos.y()-delay)
      {
         KURL url;
         url.setPath(mDragFile);
         KURL::List urls;
         urls.append(url);
         QUriDrag *d = KURLDrag::newDrag(urls, this);
         d->dragCopy();
      }
   }
   KListBox::mouseMoveEvent(e);
}

//-----------------------------------------------------------------------------
Installer::Installer (QWidget *aParent, const char *aName, bool aInit)
  : KCModule(aParent, aName)
{
  KButtonBox* bbox;

  mGui = !aInit;
  if (!mGui)
  {
    return;
  }

  connect(theme, SIGNAL(changed()), SLOT(slotThemeChanged()));

  mGrid = new QGridLayout(this, 2, 3, 6, 6);
  mThemesList = new ThemeListBox(this);
  connect(mThemesList, SIGNAL(highlighted(int)), SLOT(slotSetTheme(int)));
  connect(mThemesList, SIGNAL(filesDropped(const KURL::List&)), SLOT(slotFilesDropped(const KURL::List&)));
  mGrid->addMultiCellWidget(mThemesList, 0, 1, 0, 0);

  mPreview = new QLabel(this);
  mPreview->setFrameStyle(QFrame::Panel|QFrame::Sunken);
  mPreview->setMinimumSize(QSize(320,240));
  mGrid->addWidget(mPreview, 0, 1);

  bbox = new KButtonBox(this, KButtonBox::Vertical, 0, 6);
  mGrid->addMultiCellWidget(bbox, 0, 1, 2, 2);

  mBtnAdd = bbox->addButton(i18n("Add..."));
  connect(mBtnAdd, SIGNAL(clicked()), SLOT(slotAdd()));

  mBtnSaveAs = bbox->addButton(i18n("Save as..."));
  connect(mBtnSaveAs, SIGNAL(clicked()), SLOT(slotSaveAs()));

  mBtnCreate = bbox->addButton(i18n("Create..."));
  connect(mBtnCreate, SIGNAL(clicked()), SLOT(slotCreate()));

  mBtnRemove = bbox->addButton(i18n("Remove"));
  connect(mBtnRemove, SIGNAL(clicked()), SLOT(slotRemove()));


  bbox->layout();

  mText = new QMultiLineEdit(this);
  mText->setMinimumSize(mText->sizeHint());
  mText->setReadOnly(true);
  mGrid->addWidget(mText, 1, 1);

  mGrid->setColStretch(0, 1);
  mGrid->setColStretch(1, 3);
  mGrid->setRowStretch(0, 3);
  mGrid->setRowStretch(1, 1);

  readThemesList();
  slotSetTheme(-1);
}


//-----------------------------------------------------------------------------
Installer::~Installer()
{
}

int Installer::addTheme(const QString &path)
{
    QString tmp = path;
    int i = tmp.findRev('/');
    if (i >= 0)
       tmp = tmp.right(tmp.length() - tmp.findRev('/') - 1);
    QString p = Theme::removeExtension(tmp);
    tmp = i18n( p.utf8() );
    i = mThemesList->count();
    while((i > 0) && (mThemesList->text(i-1) > tmp))
	i--;
    if ((i > 0) && (mThemesList->text(i-1) == tmp))
       return i-1;
    mThemesList->insertItem(tmp, i);
    mThemesList->text2path.insert( tmp, p );
    return i;
}

// Copy theme package into themes directory
void Installer::addNewTheme(const KURL &srcURL)
{
  QString dir = KGlobal::dirs()->saveLocation("themes");
  KURL url;
  QString filename = srcURL.fileName();
  int i = filename.findRev('.');
  // Convert extension to lower case.
  if (i >= 0)
     filename = filename.left(i)+filename.mid(i).lower();
  url.setPath(dir+filename);
  bool rc = KIO::NetAccess::copy(srcURL, url);
  if (!rc)
  {
    kdWarning() << "Failed to copy theme " << srcURL.fileName()
        << " into themes directory " << dir << endl;
    return;
  }

  mThemesList->setCurrentItem(addTheme(url.path()));
}

//-----------------------------------------------------------------------------
void Installer::readThemesList(void)
{
  mThemesList->clear();

  // Read local themes
  QStringList entryList = KGlobal::dirs()->findAllResources("themes", QString::null, false, true);
  QStringList::ConstIterator name;
  for(name = entryList.begin(); name != entryList.end(); name++) {
    QString tmp = *name;
    if (tmp.right(8) == ".themerc") continue;
    addTheme(tmp);
  }

  // Stephan: the theme manager used to differ between global and local ones using spaces
  // as I don't know where this is used, I can't fix it ;(
}

//-----------------------------------------------------------------------------
void Installer::defaults()
{
  mThemesList->setCurrentItem(0);
}

void Installer::load()
{
  kdDebug() << "Installer::load() called" << endl;
}


//-----------------------------------------------------------------------------
void Installer::save()
{
  kdDebug() << "Installer::save() called" << endl;
}


//-----------------------------------------------------------------------------
void Installer::slotCreate()
{
  QString name;
  NewThemeDlg dlg(this);

  if (!dlg.exec()) return;
  dlg.hide();

  name = dlg.fileName();
  if (!theme->create(name)) return;
  theme->setName(dlg.themeName().local8Bit());
  theme->setAuthor(dlg.author());
  theme->setEmail(dlg.email());
  theme->setHomepage(dlg.homepage());
  theme->setVersion("0.1");
  theme->savePreview(dlg.preview());
  theme->extract();

  mThemesList->setCurrentItem(addTheme(name));
}


//-----------------------------------------------------------------------------
void Installer::slotRemove()
{
  int cur = mThemesList->currentItem();
  if (cur < 0) return;

  bool rc = false;
  QString themeName = mThemesList->text(cur);
  QString themeFile = findThemePath(themeName);
  if (!themeFile.isEmpty())
  {
     KURL url;
     url.setPath(themeFile);
     rc = KIO::NetAccess::del(url);
  }
  if (!rc)
  {
    KMessageBox::sorry(this, i18n("Failed to remove theme '%1'").arg(themeName));
    return;
  }
  mThemesList->removeItem(cur);
  if (cur >= (int)mThemesList->count()) cur--;
  mThemesList->setCurrentItem(cur);
}


//-----------------------------------------------------------------------------
void Installer::slotSetTheme(int id)
{
  bool enabled, isGlobal=false;
  QString name;

  if (id < 0)
  {
    mPreview->setText("");
    mText->setText("");
    enabled = false;
  }
  else
  {
    QString error = i18n("(Could not load theme)");
    QString path = mThemesList->text(id);
    if ( mThemesList->text2path.contains( path ) )
        path = mThemesList->text2path[path];
    name = findThemePath(path);
    enabled = false;
    if (!name.isEmpty())
    {
      enabled = theme->load(name, error);
    }
    if (!enabled)
    {
      mPreview->setText(i18n("(Could not load theme)"));
      mText->setText("");
      KMessageBox::sorry(this, error);
    }
  }

  mBtnSaveAs->setEnabled(enabled);
  mBtnRemove->setEnabled(!isGlobal);
}


//-----------------------------------------------------------------------------
void Installer::slotAdd()
{
  static QString path;
  if (path.isEmpty()) path = QDir::homeDirPath();

  KFileDialog dlg(path, Theme::allExtensions(), 0, 0, true);
  dlg.setCaption(i18n("Add Theme"));
  if (!dlg.exec()) return;

  path = dlg.baseURL().url();
  addNewTheme(dlg.selectedURL());
}

void Installer::slotFilesDropped(const KURL::List &urls)
{
  for(KURL::List::ConstIterator it = urls.begin();
      it != urls.end();
      ++it)
  {
     addNewTheme(*it);
  }
}

//-----------------------------------------------------------------------------
void Installer::slotSaveAs()
{
  QString fname, fpath, cmd, themeFile, ext;
  static QString path;
  QFileInfo finfo;
  int cur;

  if (path.isEmpty()) path = QDir::homeDirPath();

  cur = mThemesList->currentItem();
  if (cur < 0) return;

  themeFile = mThemesList->text(cur);
  if (themeFile.isEmpty()) return;

  fpath = findThemePath(themeFile);

  KURL url;
  url.setPath(fpath);
  themeFile = url.fileName();
  ext = "*"+Theme::defaultExtension();

  KFileDialog dlg(path, ext, 0, 0, true);
  dlg.setCaption(i18n("Save Theme As"));
  dlg.setSelection(themeFile);
  if (!dlg.exec()) return;

  if (dlg.baseURL().isLocalFile())
     path = dlg.baseURL().path();
  fpath = dlg.selectedFile();
  if (!Theme::checkExtension(fpath))
     fpath += Theme::defaultExtension();

  theme->save(fpath);
}


//-----------------------------------------------------------------------------
void Installer::slotThemeChanged()
{
  mText->setText(theme->description());

  mBtnSaveAs->setEnabled(true);

  if (theme->preview().isNull())
    mPreview->setText(i18n("(no preview pixmap)"));
  else mPreview->setPixmap(theme->preview());
  //mPreview->setFixedSize(theme->preview().size());
  emit changed(true);
}


//-----------------------------------------------------------------------------
int Installer::findItem(const QString aText) const
{
  int id = mThemesList->count()-1;

  while (id >= 0)
  {
    if (mThemesList->text(id) == aText) return id;
    id--;
  }

  return -1;
}


//-----------------------------------------------------------------------------
#include "installer.moc"
