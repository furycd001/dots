/****************************************************************************
**
**
** KRDB - puts current KDE color and font scheme into preprocessor statements
** cats specially written application default files and uses xrdb -merge to
** write to RESOURCE_MANAGER. Thus it gives a  simple way to make non-KDE
** applications fit in with the desktop
**
** Copyright (C) 1998 by Mark Donohoe
** Copyright (C) 1999 by Dirk A. Mueller (reworked for KDE 2.0)
** Copyright (C) 200 by Matthias Ettrich (add support for GTK applications )
** This application is freely distributable under the GNU Public License.
**
*****************************************************************************/

#include <stdlib.h>

#include <qdir.h>
#include <qdatastream.h>
#include <qstring.h>
#include <qtextstream.h>

#include <kapp.h>
#include <klocale.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <kstyle.h>
#include <kdebug.h>
#include <kglobal.h>
#include <kglobalsettings.h>
#include <kstddirs.h>
#include <kprocess.h>
#include <ktempfile.h>

enum FontStyle { Normal, Bold, Italic, Fixed };

// -----------------------------------------------------------------------------

static QString fontString( QFont rFont, FontStyle style, bool force8Bit = false )
{

  switch(style) {
  case Bold:
    rFont.setBold(true);
    break;
  case Italic:
    rFont.setItalic(true);
    break;
  case Fixed:
    rFont.setFixedPitch(true);
    break;
  case Normal:
    break;
  }
  
  if (force8Bit && (rFont.charSet() == QFont::Unicode))
    rFont.setCharSet(QFont::Latin1);

  return rFont.rawName();
}

// -----------------------------------------------------------------------------

static void addColorDef(QString& s, const char* n, const QColor& col)
{
  QString tmp;

  tmp.sprintf("#define %s #%02x%02x%02x\n",
              n, col.red(), col.green(), col.blue());

  s += tmp;
}


// -----------------------------------------------------------------------------

static void addFontDef(QString& s, const char* n, const QFont& f, FontStyle fs, bool force8Bit = false)
{
  QString tmp;

  tmp.sprintf("#define %s %s\n", n, fontString(f, fs, force8Bit).latin1());

  s += tmp;
}

// -----------------------------------------------------------------------------

static void copyFile(QFile& tmp, QString const& filename)
{
  QFile f( filename );
  if ( f.open(IO_ReadOnly|IO_Raw) ) {
      QCString buf( 8192 );
      while ( !f.atEnd() ) {
          int read = f.readBlock( buf.data(), buf.size() );
          if ( read > 0 )
              tmp.writeBlock( buf.data(), read );
      }
  }
}


// -----------------------------------------------------------------------------

static QString item( int i ) {
    return QString::number( i / 255.0, 'f', 3 );
}

static QString color( const QColor& col )
{
    return QString( "{ %1, %2, %3 }" ).arg( item( col.red() ) ).arg( item( col.green() ) ).arg( item( col.blue() ) );
}

static void createGtkrc( const QFont& font, const QColorGroup& cg )
{
    QCString filename = ::getenv("HOME");
    filename += "/.gtkrc-kde";
    QFile f( filename );

    if ( f.open( IO_WriteOnly) ) {
        QTextStream t( &f );
        t.setEncoding( QTextStream::Latin1 );

        t << "# created by KDE, " << QDateTime::currentDateTime().toString() << endl;
        t << "#" << endl;
        t << "# If you do not want KDE to override your GTK settings, select" << endl;
        t << "# Themes/Styles in the control center and disable the checkbox " << endl;
        t << "# \"Apply fonts and colors to non-KDE apps\"" << endl;
        t << "#" << endl;
        t << endl;
        t << "style \"default\"" << endl;
        t << "{" << endl;
//         t << "  " << "fontset = \"" << font.rawName()
//           << ", -*-" << font.family() << "-*-*-*-*-*-*-*-*-*-*-*-*\"" << endl;
//         t << endl;
        t << "  bg[NORMAL] = " << color( cg.background() ) << endl;
        t << "  bg[SELECTED] = " << color( cg.highlight() ) << endl;
        t << "  bg[INSENSITIVE] = " << color( cg.background() ) << endl;
        t << "  bg[ACTIVE] = " << color( cg.mid() ) << endl;
        t << "  bg[PRELIGHT] = " << color( cg.background() ) << endl;
        t << endl;
        t << "  base[NORMAL] = " << color( cg.base() ) << endl;
        t << "  base[SELECTED] = " << color( cg.highlight() ) << endl;
        t << "  base[INSENSITIVE] = " << color( cg.background() ) << endl;
        t << "  base[ACTIVE] = " << color( cg.base() ) << endl;
        t << "  base[PRELIGHT] = " << color( cg.base() ) << endl;
        t << endl;
        t << "  text[NORMAL] = " << color( cg.text() ) << endl;
        t << "  text[SELECTED] = " << color( cg.highlightedText() ) << endl;
        t << "  text[INSENSITIVE] = " << color( cg.mid() ) << endl;
        t << "  text[ACTIVE] = " << color( cg.text() ) << endl;
        t << "  text[PRELIGHT] = " << color( cg.text() ) << endl;
        t << endl;
        t << "  fg[NORMAL] = " << color( cg.foreground() ) << endl;
        t << "  fg[SELECTED] = " << color( cg.highlightedText() ) << endl;
        t << "  fg[INSENSITIVE] = " << color( cg.mid() ) << endl;
        t << "  fg[ACTIVE] = " << color( cg.foreground() ) << endl;
        t << "  fg[PRELIGHT] = " << color( cg.foreground() ) << endl;
        t << "}" << endl;
        t << endl;
        t << "class \"*\" style \"default\"" << endl;
        t << endl;
    }
}

// -----------------------------------------------------------------------------

void runRdb() {

  KGlobal::dirs()->addResourceType("appdefaults", KStandardDirs::kde_default("data") + "kdisplay2/app-defaults/");
  //KGlobal::dirs()->addResourceType("appdefaults", KStandardDirs::kde_default("data") + "kdisplay/app-defaults/"); // could add the modern ones, but there's never been any more additions
  QColorGroup cg = kapp->palette().normal();
  if ( !kapp->kstyle() || !kapp->kstyle()->inherits("KLegacyStyle") )
      createGtkrc( kapp->font(), cg );

  QString preproc;

  addColorDef(preproc, "FOREGROUND"         , cg.foreground());
  QColor backCol = cg.background();
  addColorDef(preproc, "BACKGROUND"         , backCol);
  addColorDef(preproc, "HIGHLIGHT"          , backCol.light(100+(2*KGlobalSettings::contrast()+4)*16/1));
  addColorDef(preproc, "LOWLIGHT"           , backCol.dark(100+(2*KGlobalSettings::contrast()+4)*10));
  addColorDef(preproc, "SELECT_BACKGROUND"  , cg.highlight());
  addColorDef(preproc, "SELECT_FOREGROUND"  , cg.highlightedText());
  addColorDef(preproc, "WINDOW_BACKGROUND"  , cg.base());
  addColorDef(preproc, "WINDOW_FOREGROUND"  , cg.foreground());
  addColorDef(preproc, "INACTIVE_BACKGROUND", KGlobalSettings::inactiveTitleColor());
  addColorDef(preproc, "INACTIVE_FOREGROUND", KGlobalSettings::inactiveTitleColor());
  addColorDef(preproc, "ACTIVE_BACKGROUND"  , KGlobalSettings::activeTitleColor());
  addColorDef(preproc, "ACTIVE_FOREGROUND"  , KGlobalSettings::activeTitleColor());

  addFontDef(preproc, "FONT"                , KGlobalSettings::generalFont(), Normal);
  addFontDef(preproc, "BOLD_FONT"           , KGlobalSettings::generalFont(), Bold);
  addFontDef(preproc, "ITALIC_FONT"         , KGlobalSettings::generalFont(), Italic);
  addFontDef(preproc, "FIXED_FONT"          , KGlobalSettings::fixedFont(), Fixed);
  // Fontlist
  preproc += "#define FONTLIST FONT,BOLD_FONT=BOLD,ITALIC_FONT=ITALIC\n";

  addFontDef(preproc, "FONT_8BIT"           , KGlobalSettings::generalFont(), Normal, true);
  addFontDef(preproc, "BOLD_FONT_8BIT"      , KGlobalSettings::generalFont(), Bold, true);
  addFontDef(preproc, "ITALIC_FONT_8BIT"    , KGlobalSettings::generalFont(), Italic, true);
  addFontDef(preproc, "FIXED_FONT_8BIT"     , KGlobalSettings::fixedFont(), Fixed, true);
  // Fontlist
  preproc += "#define FONTLIST_8BIT FONT_8BIT,BOLD_FONT_8BIT=BOLD,ITALIC_FONT_8BIT=ITALIC\n";

  //---------------------------------------------------------------

  QStringList list;

  QStringList adPaths = KGlobal::dirs()->findDirs("appdefaults", "");
  for (QStringList::ConstIterator it = adPaths.begin(); it != adPaths.end(); it++) {
    QDir dSys( *it );

    if ( dSys.exists() ) {
      dSys.setFilter( QDir::Files );
      dSys.setSorting( QDir::Name );
      dSys.setNameFilter("*.ad");
      list += dSys.entryList();
    }
  }

  QString propString;

  KTempFile tmpFile;

  if (tmpFile.status() != 0)
  {
    kdDebug() << "Couldn't open temp file" << endl;
    exit(0);
  }

  QFile &tmp = *(tmpFile.file());
  tmp.writeBlock( preproc.latin1(), preproc.length() );

  for (QStringList::ConstIterator it = list.begin(); it != list.end(); it++)
    copyFile(tmp, locate("appdefaults", *it ));

  // very primitive support for  ~/.Xdefaults by appending it
  copyFile(tmp, QDir::homeDirPath() + "/.Xdefaults");

  tmpFile.close();

  KProcess proc;

  proc.setExecutable("xrdb");
  proc << "-merge" << tmpFile.name();

  proc.start( KProcess::Block, KProcess::Stdin );

  tmpFile.unlink();
}

