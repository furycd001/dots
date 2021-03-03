/***************************************************************************
                          katedocument.cpp  -  description
                             -------------------
    begin                : Mon Jan 15 2001
    copyright            : (C) 2001 by Christoph "Crossfire" Cullmann
    email                : crossfire@babylon2k.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/*
   Copyright (C) 1998, 1999 Jochen Wilhelmy
                            digisnap@cs.tu-berlin.de

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#include "katedocument.h"
#include "katedocument.moc"

#include "../factory/katefactory.h"

#include <qfileinfo.h>
#include <qdatetime.h>

#include <kmessagebox.h>
#include <klocale.h>
#include <kconfig.h>
#include <qstring.h>

#include <sys/time.h>
#include <unistd.h>

#include <stdio.h>

#include <qtimer.h>
#include <qobject.h>
#include <qapplication.h>
#include <qclipboard.h>
#include <qfont.h>
#include <qpainter.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qtextcodec.h>
#include <kglobal.h>

#include <klocale.h>
#include <kcharsets.h>
#include <kdebug.h>
#include <kinstance.h>

#include <kglobalsettings.h>
#include <kaction.h>
#include <kstdaction.h>

#include "../view/kateview.h"
#include "katebuffer.h"
#include "katetextline.h"

#include "katecmd.h"

KateAction::KateAction(Action a, PointStruc &cursor, int len, const QString &text)
  : action(a), cursor(cursor), len(len), text(text) {
}

KateActionGroup::KateActionGroup(PointStruc &aStart, int type)
  : start(aStart), action(0L), undoType(type) {
}

KateActionGroup::~KateActionGroup() {
  KateAction *current, *next;

  current = action;
  while (current) {
    next = current->next;
    delete current;
    current = next;
  }
}

void KateActionGroup::insertAction(KateAction *a) {
  a->next = action;
  action = a;
}

const char * KateActionGroup::typeName(int type)
{
  // return a short text description of the given undo group type suitable for a menu
  // not the lack of i18n's, the caller is expected to handle translation
  switch (type) {
  case ugPaste : return "Paste Text";
  case ugDelBlock : return "Selection Overwrite";
  case ugIndent : return "Indent";
  case ugUnindent : return "Unindent";
  case ugComment : return "Comment";
  case ugUncomment : return "Uncomment";
  case ugReplace : return "Text Replace";
  case ugSpell : return "Spell Check";
  case ugInsChar : return "Typing";
  case ugDelChar : return "Delete Text";
  case ugInsLine : return "New Line";
  case ugDelLine : return "Delete Line";
  }
  return "";
}

const int KateDocument::maxAttribs = 32;

QStringList KateDocument::searchForList = QStringList();
QStringList KateDocument::replaceWithList = QStringList();

uint KateDocument::uniqueID = 0;

QPtrDict<KateDocument::KateDocPrivate>* KateDocument::d_ptr = 0;    


KateDocument::KateDocument(bool bSingleViewMode, bool bBrowserView,
                                           QWidget *parentWidget, const char *widgetName,
                                           QObject *, const char *)
  : Kate::Document (), DCOPObject ((QString("KateDocument%1").arg(uniqueID)).latin1()),
    myFont(KGlobalSettings::fixedFont()), myFontBold(KGlobalSettings::fixedFont()), myFontItalic(KGlobalSettings::fixedFont()), myFontBI(KGlobalSettings::fixedFont()),
    myFontMetrics (myFont), myFontMetricsBold (myFontBold), myFontMetricsItalic (myFontItalic), myFontMetricsBI (myFontBI),
    hlManager(HlManager::self ())
{
  d(this)->hlSetByUser = false;  
  PreHighlightedTill=0;
  RequestPreHighlightTill=0;
  setInstance( KateFactory::instance() );

  m_bSingleViewMode=bSingleViewMode;
  m_bBrowserView = bBrowserView;

  m_url = KURL();

  // NOTE: QFont::CharSet doesn't provide all the charsets KDE supports
  // (esp. it doesn't distinguish between UTF-8 and iso10646-1) 
  myEncoding = QString::fromLatin1(QTextCodec::codecForLocale()->name());
  maxLength = -1;

  setFont (KGlobalSettings::fixedFont());

  myDocID = uniqueID;
  uniqueID++;

  myDocName = QString ("");
  fileInfo = new QFileInfo ();

  myCmd = new KateCmd (this);

  connect(this,SIGNAL(modifiedChanged ()),this,SLOT(slotModChanged ()));

  buffer = new KWBuffer;
  connect(buffer, SIGNAL(linesChanged(int)), this, SLOT(slotBufferChanged()));
//  connect(buffer, SIGNAL(textChanged()), this, SIGNAL(textChanged()));
  connect(buffer, SIGNAL(needHighlight(long,long)),this,SLOT(slotBufferHighlight(long,long)));

  colors[0] = KGlobalSettings::baseColor();
  colors[1] = KGlobalSettings::highlightColor();

  m_attribs = new Attribute[maxAttribs];

  m_highlight = 0L;
  tabChars = 8;

  m_singleSelection = false;

  newDocGeometry = false;
  readOnly = false;
  newDoc = false;

  modified = false;

  undoList.setAutoDelete(true);
  undoState = 0;
  undoSteps = 50;

  pseudoModal = 0L;
  clear();

  setHighlight(0); //calls updateFontData()
  // if the user changes the highlight with the dialog, notify the doc
  connect(hlManager,SIGNAL(changed()),SLOT(hlChanged()));

  newDocGeometry = false;

  readConfig();

  if ( m_bSingleViewMode )
  {
    KTextEditor::View *view = createView( parentWidget, widgetName );
    view->show();
    setWidget( view );
  }
}

void KateDocument::setDontChangeHlOnSave()
{
  d(this)->hlSetByUser = true;
}

void KateDocument::setFont (QFont font)
{
  kdDebug()<<"Kate:: setFont"<<endl;
  int oldwidth=myFontMetrics.width('W');  //Quick & Dirty Hack (by JoWenn) //Remove in KDE 3.0
  myFont = font;
  myFontBold = QFont (font);
  myFontBold.setBold (true);

  myFontItalic = QFont (font);
  myFontItalic.setItalic (true);

  myFontBI = QFont (font);
  myFontBI.setBold (true);
  myFontBI.setItalic (true);

  myFontMetrics = CachedFontMetrics (myFont);
  myFontMetricsBold = CachedFontMetrics (myFontBold);
  myFontMetricsItalic = CachedFontMetrics (myFontItalic);
  myFontMetricsBI = CachedFontMetrics (myFontBI);
  int newwidth=myFontMetrics.width('W'); //Quick & Dirty Hack (by JoWenn)  //Remove in KDE 3.0
  maxLength=maxLength*(float)newwidth/(float)oldwidth; //Quick & Dirty Hack (by JoWenn)  //Remove in KDE 3.0

  updateFontData();
  updateViews(); //Quick & Dirty Hack (by JoWenn) //Remove in KDE 3.0

}

long  KateDocument::needPreHighlight(long till)
{
  int max=numLines()-1;
  if (till>max)
    {
      till=max;
    }
  if (PreHighlightedTill>=till) return -1;

  long tmp=RequestPreHighlightTill;
  if (RequestPreHighlightTill<till)
    {
      RequestPreHighlightTill=till;
      if (tmp<=PreHighlightedTill) QTimer::singleShot(10,this,SLOT(doPreHighlight()));
    }
  return RequestPreHighlightTill;
}

void KateDocument::doPreHighlight()
{
  int from = PreHighlightedTill;
  int till = PreHighlightedTill+200;
  int max = numLines()-1;
  if (till > max)
    {
      till = max;
    }
  PreHighlightedTill = till;
  updateLines(from,till);
  emit preHighlightChanged(PreHighlightedTill);
  if (PreHighlightedTill<RequestPreHighlightTill)
    QTimer::singleShot(10,this,SLOT(doPreHighlight()));
}

KateDocument::~KateDocument()
{
  m_highlight->release();

  if ( !m_bSingleViewMode )
  {
    m_views.setAutoDelete( true );
    m_views.clear();
    m_views.setAutoDelete( false );
  }
  delete_d(this);
}

bool KateDocument::openFile()
{
  fileInfo->setFile (m_file);
  setMTime();

  if (!fileInfo->exists() || !fileInfo->isReadable())
    return false;

  buffer->clear();
  buffer->insertFile(0, m_file, KGlobal::charsets()->codecForName(myEncoding));

  setMTime();

  if (myWordWrap)
    wrapText (myWordWrapAt);

  int hl = hlManager->wildcardFind( m_file );

  if (hl == -1)
  {
    // fill the detection buffer with the contents of the text
    const int HOWMANY = 1024;
    QByteArray buf(HOWMANY);
    int bufpos = 0, len;
    for (int i=0; i < buffer->count(); i++)
    {
      TextLine::Ptr textLine = buffer->line(i);
      len = textLine->length();
      if (bufpos + len > HOWMANY) len = HOWMANY - bufpos;
      memcpy(&buf[bufpos], textLine->getText(), len);
      bufpos += len;
      if (bufpos >= HOWMANY) break;
    }

    hl = hlManager->mimeFind( buf, m_file );
  }

  setHighlight(hl);

  updateLines();
  updateViews();

  emit fileNameChanged();
  
  return true;
}

bool KateDocument::saveFile()
{
  QFile f( m_file );
  if ( !f.open( IO_WriteOnly ) )
    return false; // Error

  QTextStream stream(&f);

  stream.setEncoding(QTextStream::RawUnicode); // disable Unicode headers
  stream.setCodec(KGlobal::charsets()->codecForName(myEncoding)); // this line sets the mapper to the correct codec

  int maxLine = numLines();
  int line = 0;
  while(true)
  {
    stream << getTextLine(line)->getString();
    line++;
    if (line >= maxLine) break;

    if (eolMode == KateDocument::eolUnix) stream << "\n";
    else if (eolMode == KateDocument::eolDos) stream << "\r\n";
    else if (eolMode == KateDocument::eolMacintosh) stream << '\r';
  };
  f.close();

  fileInfo->setFile (m_file);
  setMTime();

  if (!(d(this)->hlSetByUser))
  {
  int hl = hlManager->wildcardFind( m_file );

  if (hl == -1)
  {
    // fill the detection buffer with the contents of the text
    const int HOWMANY = 1024;
    QByteArray buf(HOWMANY);
    int bufpos = 0, len;
    for (int i=0; i < buffer->count(); i++)
    {
      TextLine::Ptr textLine = buffer->line(i);
      len = textLine->length();
      if (bufpos + len > HOWMANY) len = HOWMANY - bufpos;
      memcpy(&buf[bufpos], textLine->getText(), len);
      bufpos += len;
      if (bufpos >= HOWMANY) break;
    }

    hl = hlManager->mimeFind( buf, m_file );
  }

  setHighlight(hl);
  }
  emit fileNameChanged ();

  return (f.status() == IO_Ok);
}

KTextEditor::View *KateDocument::createView( QWidget *parent, const char *name )
{
  return new KateView( this, parent, name);
}

QString KateDocument::textLine( int line ) const
{
  TextLine::Ptr l = getTextLine( line );
  if ( !l )
    return QString();

  return l->getString();
}

void KateDocument::replaceLine(const QString& s,int line)
{
  remove_Line(line,false);
  insert_Line(s,line,true);
}

void KateDocument::insertLine( const QString &str, int l ) {
  insert_Line(str,l,true);
}

void KateDocument::insert_Line(const QString& s,int line, bool update)
{
  kdDebug(13020)<<"KateDocument::insertLine "<<s<<QString("	%1").arg(line)<<endl;
  TextLine::Ptr TL=new TextLine();
  TL->append(s.unicode(),s.length());
  buffer->insertLine(line,TL);
  if (update)
  {
    newDocGeometry=true;
    updateLines(line);
    updateViews();
  }
}

void KateDocument::insertAt( const QString &s, int line, int col, bool  )
{
  VConfig c;
  c.view = 0; // ### FIXME
  c.cursor.x = col;
  c.cursor.y = line;
  c.cXPos = 0; // ### FIXME
  c.flags = 0; // ### FIXME
  insert( c, s );
}

void KateDocument::removeLine( int line ) {
  remove_Line(line,true);
}

void KateDocument::remove_Line(int line,bool update)
{
  kdDebug(13020)<<"KateDocument::removeLine "<<QString("%1").arg(line)<<endl;
  buffer->removeLine(line);
//  newDocGeometry=true;
//  if line==0)
  if (update)
  {
    updateLines(line);
    updateViews();
  }
}

int KateDocument::length() const
{
  return text().length();
}

void KateDocument::setSelection( int , int , int , int )
{
}

bool KateDocument::hasSelection() const
{
  return (selectEnd >= selectStart);
}

QString KateDocument::selection() const
{
  uint flags = 0;
  TextLine::Ptr textLine;
  int len, z, start, end, i;

  len = 1;
  if (!(flags & KateView::cfVerticalSelect)) {
    for (z = selectStart; z <= selectEnd; z++) {
      textLine = getTextLine(z);
      len += textLine->numSelected();
      if (textLine->isSelected()) len++;
    }
    QString s;
    len = 0;
    for (z = selectStart; z <= selectEnd; z++) {
      textLine = getTextLine(z);
      end = 0;
      do {
        start = textLine->findUnselected(end);
        end = textLine->findSelected(start);
        for (i = start; i < end; i++) {
          s[len] = textLine->getChar(i);
          len++;
        }
      } while (start < end);
      if (textLine->isSelected()) {
        s[len] = '\n';
        len++;
      }
    }
//    s[len] = '\0';
    return s;
  } else {
    for (z = selectStart; z <= selectEnd; z++) {
      textLine = getTextLine(z);
      len += textLine->numSelected() + 1;
    }
    QString s;
    len = 0;
    for (z = selectStart; z <= selectEnd; z++) {
      textLine = getTextLine(z);
      end = 0;
      do {
        start = textLine->findUnselected(end);
        end = textLine->findSelected(start);
        for (i = start; i < end; i++) {
          s[len] = textLine->getChar(i);
          len++;
        }
      } while (start < end);
      s[len] = '\n';
      len++;
    }
//    s[len] = '\0';       //  the final \0 is not counted in length()
    return s;
  }
}

int KateDocument::numLines() const
{
  return buffer->count();
}


TextLine::Ptr KateDocument::getTextLine(int line) const
{
  // This is a hack to get this stuff working.
  return buffer->line(line);
}

int KateDocument::textLength(int line) {
  TextLine::Ptr textLine = getTextLine(line);
  if (!textLine) return 0;
  return textLine->length();
}

void KateDocument::setTabWidth(int chars) {
  if (tabChars == chars) return;
  if (chars < 1) chars = 1;
  if (chars > 16) chars = 16;
  tabChars = chars;
  updateFontData();

  maxLength = -1;
  for (int i=0; i < buffer->count(); i++)
  {
    TextLine::Ptr textLine = buffer->line(i);
    int len = textWidth(textLine,textLine->length());
    if (len > maxLength) {
      maxLength = len;
      longestLine = textLine;
    }
  }
}

void KateDocument::setReadWrite( bool rw )
{
  setReadOnly( !rw );
  KTextEditor::Document::setReadWrite( rw );
}

bool KateDocument::isReadWrite() const
{
  return !isReadOnly();
}

void KateDocument::setReadOnly(bool m) {
  KTextEditor::View *view;

  if (m != readOnly) {
    readOnly = m;
//    if (readOnly) recordReset();
    for (view = m_views.first(); view != 0L; view = m_views.next() ) {
      emit static_cast<KateView *>( view )->newStatus();
    }
  }
}

bool KateDocument::isReadOnly() const {
  return readOnly;
}

void KateDocument::setNewDoc( bool m )
{
//  KTextEditor::View *view;

  if ( m != newDoc )
  {
    newDoc = m;
////    if (readOnly) recordReset();
//    for (view = m_views.first(); view != 0L; view = m_views.next() ) {
//      emit static_cast<KateView *>( view )->newStatus();
//    }
  }
}

bool KateDocument::isNewDoc() const {
  return newDoc;
}

void KateDocument::setModified(bool m) {
  KTextEditor::View *view;

  if (m != modified) {
    modified = m;
    for (view = m_views.first(); view != 0L; view = m_views.next() ) {
      emit static_cast<KateView *>( view )->newStatus();
    }
    emit modifiedChanged ();
  }
}

bool KateDocument::isModified() const {
  return modified;
}

void KateDocument::readConfig()
{
  KConfig *config = KateFactory::instance()->config();
  config->setGroup("Kate Document");

  myWordWrap = config->readBoolEntry("Word Wrap On", false);
  myWordWrapAt = config->readNumEntry("Word Wrap At", 80);
  if (myWordWrap)
    wrapText (myWordWrapAt);

  setTabWidth(config->readNumEntry("TabWidth", 8));
  setUndoSteps(config->readNumEntry("UndoSteps", 50));
  m_singleSelection = config->readBoolEntry("SingleSelection", false);
  myEncoding = config->readEntry("Encoding", QString::fromLatin1(QTextCodec::codecForLocale()->name()));
  setFont (config->readFontEntry("Font", &myFont));

  colors[0] = config->readColorEntry("Color Background", &colors[0]);
  colors[1] = config->readColorEntry("Color Selected", &colors[1]);
  
  config->sync();
}

void KateDocument::writeConfig()
{
  KConfig *config = KateFactory::instance()->config();
  config->setGroup("Kate Document");

  config->writeEntry("Word Wrap On", myWordWrap);
  config->writeEntry("Word Wrap At", myWordWrapAt);
  config->writeEntry("TabWidth", tabChars);
  config->writeEntry("UndoSteps", undoSteps);
  config->writeEntry("SingleSelection", m_singleSelection);
  config->writeEntry("Encoding", myEncoding);
  config->writeEntry("Font", myFont);
  config->writeEntry("Color Background", colors[0]);
  config->writeEntry("Color Selected", colors[1]);

  config->sync();
}

void KateDocument::readSessionConfig(KConfig *config)
{
  m_url = config->readEntry("URL"); // ### doesn't this break the encoding? (Simon)
  setHighlight(hlManager->nameFind(config->readEntry("Highlight")));
  // anders: restore bookmarks if possible
  QValueList<int> l = config->readIntListEntry("Bookmarks");
  if ( l.count() ) {
    for (uint i=0; i < l.count(); i++) {
      if ( numLines() < l[i] ) break;
      getTextLine( l[i] )->addMark( Bookmark );
    }
  }
}

void KateDocument::writeSessionConfig(KConfig *config)
{
  config->writeEntry("URL", m_url.url() ); // ### encoding?? (Simon)
  config->writeEntry("Highlight", m_highlight->name());
  // anders: save bookmarks
  QList<Kate::Mark> l = marks();
  QValueList<int> ml;
  for (uint i=0; i < l.count(); i++) {
    if ( l.at(i)->type == 1) // only save bookmarks
     ml << l.at(i)->line;
  }
  if ( ml.count() )
    config->writeEntry("Bookmarks", ml);
}


void KateDocument::setHighlight(int n) {
  Highlight *h;

//  hlNumber = n;

  h = hlManager->getHl(n);
  if (h == m_highlight) {
    updateLines();
  } else {
    if (m_highlight != 0L) m_highlight->release();
    h->use();
    m_highlight = h;
    makeAttribs();
  }
  PreHighlightedTill=0;
  RequestPreHighlightTill=0;
  emit(highlightChanged());
}

void KateDocument::makeAttribs() {

  m_numAttribs = hlManager->makeAttribs(m_highlight, m_attribs, maxAttribs);
  updateFontData();
  updateLines();
}

void KateDocument::updateFontData() {
  int maxAscent, maxDescent;
  int tabWidth;
  KateView *view;

  maxAscent = myFontMetrics.ascent();
  maxDescent = myFontMetrics.descent();
  tabWidth = myFontMetrics.width(' ');

  fontHeight = maxAscent + maxDescent + 1;
  fontAscent = maxAscent;
  m_tabWidth = tabChars*tabWidth;

  for (view = views.first(); view != 0L; view = views.next() ) {
    view->myViewInternal->drawBuffer->resize(view->width(),fontHeight);
    view->tagAll();
    view->updateCursor();
  }
}

void KateDocument::hlChanged() { //slot
  makeAttribs();
  updateViews();
}


void KateDocument::addView(KTextEditor::View *view) {
  views.append( static_cast<KateView *>( view ) );
  KTextEditor::Document::addView( view );
  connect( static_cast<KateView *>( view ), SIGNAL( destroyed() ), this, SLOT( slotViewDestroyed() ) );
}

void KateDocument::removeView(KTextEditor::View *view) {
//  if (undoView == view) recordReset();
  disconnect( static_cast<KateView *>( view ), SIGNAL( destroyed() ), this, SLOT( slotViewDestroyed() ) );
  views.removeRef( static_cast<KateView *>( view ) );
  KTextEditor::Document::removeView( view );
}

void KateDocument::slotViewDestroyed()
{
  views.removeRef( static_cast<const KateView *>( sender() ) );
}

bool KateDocument::ownedView(KateView *view) {
  // do we own the given view?
  return (views.containsRef(view) > 0);
}

bool KateDocument::isLastView(int numViews) {
  return ((int) views.count() == numViews);
}

int KateDocument::textWidth(const TextLine::Ptr &textLine, int cursorX) {
  int x;
  int z;
  QChar ch;
  Attribute *a;

  x = 0;
  for (z = 0; z < cursorX; z++) {
    ch = textLine->getChar(z);
    a = &m_attribs[textLine->getAttr(z)];

    if (ch == '\t')
      x += m_tabWidth - (x % m_tabWidth);
    else if (a->bold && a->italic)
      x += myFontMetricsBI.width(ch);
    else if (a->bold)
      x += myFontMetricsBold.width(ch);
    else if (a->italic)
      x += myFontMetricsItalic.width(ch);
    else
      x += myFontMetrics.width(ch);
  }
  return x;
}

int KateDocument::textWidth(PointStruc &cursor) {
  if (cursor.x < 0)
     cursor.x = 0;
  if (cursor.y < 0)
     cursor.y = 0;
  if (cursor.y >= numLines())
     cursor.y = lastLine();
  return textWidth(getTextLine(cursor.y),cursor.x);
}

int KateDocument::textWidth(bool wrapCursor, PointStruc &cursor, int xPos) {
  int len;
  int x, oldX;
  int z;
  QChar ch;
  Attribute *a;

  if (cursor.y < 0) cursor.y = 0;
  if (cursor.y > lastLine()) cursor.y = lastLine();
  TextLine::Ptr textLine = getTextLine(cursor.y);
  len = textLine->length();

  x = oldX = z = 0;
  while (x < xPos && (!wrapCursor || z < len)) {
    oldX = x;
    ch = textLine->getChar(z);
    a = &m_attribs[textLine->getAttr(z)];

    if (ch == '\t')
      x += m_tabWidth - (x % m_tabWidth);
    else if (a->bold && a->italic)
      x += myFontMetricsBI.width(ch);
    else if (a->bold)
      x += myFontMetricsBold.width(ch);
    else if (a->italic)
      x += myFontMetricsItalic.width(ch);
    else
      x += myFontMetrics.width(ch);

    z++;
  }
  if (xPos - oldX < x - xPos && z > 0) {
    z--;
    x = oldX;
  }
  cursor.x = z;
  return x;
}


int KateDocument::textPos(const TextLine::Ptr &textLine, int xPos) {
  int x, oldX;
  int z;
  QChar ch;
  Attribute *a;

  x = oldX = z = 0;
  while (x < xPos) { // && z < len) {
    oldX = x;
    ch = textLine->getChar(z);
    a = &m_attribs[textLine->getAttr(z)];

    if (ch == '\t')
      x += m_tabWidth - (x % m_tabWidth);
    else if (a->bold && a->italic)
      x += myFontMetricsBI.width(ch);
    else if (a->bold)
      x += myFontMetricsBold.width(ch);
    else if (a->italic)
      x += myFontMetricsItalic.width(ch);
    else
      x += myFontMetrics.width(ch);

    z++;
  }
  if (xPos - oldX < x - xPos && z > 0) {
    z--;
   // newXPos = oldX;
  }// else newXPos = x;
  return z;
}

int KateDocument::textWidth() {
   return int(maxLength + 8);
}

int KateDocument::textHeight() {
  return numLines()*fontHeight;
}

void KateDocument::insert(VConfig &c, const QString &s) {
  int pos;
  QChar ch;
  QString buf;

  if (s.isEmpty()) return;

  recordStart(c, KateActionGroup::ugPaste);

  pos = 0;
  if (!(c.flags & KateView::cfVerticalSelect)) {
    do {
      ch = s[pos];
      if (ch.isPrint() || ch == '\t') {
        buf += ch; // append char to buffer
      } else if (ch == '\n') {
        recordAction(KateAction::newLine, c.cursor); // wrap contents behind cursor to new line
        recordInsert(c, buf); // append to old line
//        c.cursor.x += buf.length();
        buf.truncate(0); // clear buffer
        c.cursor.y++;
        c.cursor.x = 0;
      }
      pos++;
    } while (pos < (int) s.length());
  } else {
    int xPos;

    xPos = textWidth(c.cursor);
    do {
      ch = s[pos];
      if (ch.isPrint() || ch == '\t') {
        buf += ch;
      } else if (ch == '\n') {
        recordInsert(c, buf);
        c.cursor.x += buf.length();
        buf.truncate(0);
        c.cursor.y++;
        if (c.cursor.y >= numLines())
          recordAction(KateAction::insLine, c.cursor);
        c.cursor.x = textPos(getTextLine(c.cursor.y), xPos);
      }
      pos++;
    } while (pos < (int) s.length());
  }
  recordInsert(c, buf);
  c.cursor.x += buf.length();
  recordEnd(c);
}

void KateDocument::insertFile(VConfig &c, QIODevice &dev)
{
  recordStart(c, KateActionGroup::ugPaste);

  QString buf;
  QChar ch, last;

  QTextStream stream( &dev );

  while ( !stream.atEnd() ) {
    stream >> ch;

    if (ch.isPrint() || ch == '\t') {
        buf += ch;
    } else if (ch == '\n' || ch == '\r') {
        if (last != '\r' || ch != '\n') {
          recordAction(KateAction::newLine, c.cursor);
          recordInsert(c, buf);
          buf.truncate(0);
          c.cursor.y++;
          c.cursor.x = 0;
        }
        last = ch;
    }
  }

  recordInsert(c, buf);
  recordEnd(c);
}

int KateDocument::currentColumn(PointStruc &cursor) {
  return getTextLine(cursor.y)->cursorX(cursor.x,tabChars);
}

bool KateDocument::insertChars(VConfig &c, const QString &chars) {
  int z, pos, l;
  bool onlySpaces;
  QChar ch;
  QString buf;

  TextLine::Ptr textLine = getTextLine(c.cursor.y);

  pos = 0;
  onlySpaces = true;
  for (z = 0; z < (int) chars.length(); z++) {
    ch = chars[z];
    if (ch == '\t' && c.flags & KateView::cfReplaceTabs) {
      l = tabChars - (textLine->cursorX(c.cursor.x, tabChars) % tabChars);
      while (l > 0) {
        buf.insert(pos, ' ');
        pos++;
        l--;
      }
    } else if (ch.isPrint() || ch == '\t') {
      buf.insert(pos, ch);
      pos++;
      if (ch != ' ') onlySpaces = false;
      if (c.flags & KateView::cfAutoBrackets) {
        if (ch == '(') buf.insert(pos, ')');
        if (ch == '[') buf.insert(pos, ']');
        if (ch == '{') buf.insert(pos, '}');
      }
    }
  }
  //pos = cursor increment

  //return false if nothing has to be inserted
  if (buf.isEmpty()) return false;

  //auto deletion of the marked text occurs not very often and can therefore
  //  be recorded separately
  if (c.flags &KateView:: cfDelOnInput) delMarkedText(c);

  recordStart(c, KateActionGroup::ugInsChar);
  recordReplace(c/*.cursor*/, (c.flags & KateView::cfOvr) ? buf.length() : 0, buf);
  c.cursor.x += pos;

  if (myWordWrap && myWordWrapAt > 0) {
    int line;
    const QChar *s;
//    int pos;
    PointStruc actionCursor;

    line = c.cursor.y;
    do {
      textLine = getTextLine(line);
      s = textLine->getText();
      l = textLine->length();
      for (z = myWordWrapAt; z < l; z++) if (!s[z].isSpace()) break; //search for text to wrap
      if (z >= l) break; // nothing more to wrap
      pos = myWordWrapAt;
      for (; z >= 0; z--) { //find wrap position
        if (s[z].isSpace()) {
          pos = z + 1;
          break;
        }
      }
      //pos = wrap position

      if (line == c.cursor.y && pos <= c.cursor.x) {
        //wrap cursor
        c.cursor.y++;
        c.cursor.x -= pos;
      }

      if (line == lastLine() || (getTextLine(line+1)->length() == 0) ) {
        //at end of doc: create new line
        actionCursor.x = pos;
        actionCursor.y = line;
        recordAction(KateAction::newLine,actionCursor);
      } else {
        //wrap
        actionCursor.y = line + 1;
        if (!s[l - 1].isSpace()) { //add space in next line if necessary
          actionCursor.x = 0;
          recordInsert(actionCursor, " ");
        }
        actionCursor.x = textLine->length() - pos;
        recordAction(KateAction::wordWrap, actionCursor);
      }
      line++;
    } while (true);
  }
  recordEnd(c);
  return true;
}

QString tabString(int pos, int tabChars) {
  QString s;
  while (pos >= tabChars) {
    s += '\t';
    pos -= tabChars;
  }
  while (pos > 0) {
    s += ' ';
    pos--;
  }
  return s;
}

void KateDocument::newLine(VConfig &c) {

  //auto deletion of marked text is done by the view to have a more
  // "low level" KateDocument::newLine method
  recordStart(c, KateActionGroup::ugInsLine);

  if (!(c.flags & KateView::cfAutoIndent)) {
    recordAction(KateAction::newLine,c.cursor);
    c.cursor.y++;
    c.cursor.x = 0;
  } else {
    TextLine::Ptr textLine = getTextLine(c.cursor.y);
    int pos = textLine->firstChar();
    if (c.cursor.x < pos) c.cursor.x = pos; // place cursor on first char if before

    int y = c.cursor.y;
    while ((y > 0) && (pos < 0)) { // search a not empty text line
      textLine = getTextLine(--y);
      pos = textLine->firstChar();
    }
    recordAction(KateAction::newLine, c.cursor);
    c.cursor.y++;
    c.cursor.x = 0;
    if (pos > 0) {
      pos = textLine->cursorX(pos, tabChars);
//      if (getTextLine(c.cursor.y)->length() > 0) {
        QString s = tabString(pos, (c.flags & KateView::cfSpaceIndent) ? 0xffffff : tabChars);
        recordInsert(c.cursor, s);
        pos = s.length();
//      }
//      recordInsert(c.cursor, QString(textLine->getText(), pos));
      c.cursor.x = pos;
    }
  }

  recordEnd(c);
}

void KateDocument::killLine(VConfig &c) {

  recordStart(c, KateActionGroup::ugDelLine);
  c.cursor.x = 0;
  recordDelete(c.cursor, 0xffffff);
  if (c.cursor.y < lastLine()) {
    recordAction(KateAction::killLine, c.cursor);
  }
  recordEnd(c);
}

void KateDocument::backspace(VConfig &c) {

  if (c.cursor.x <= 0 && c.cursor.y <= 0) return;

  if (c.cursor.x > 0) {
    recordStart(c, KateActionGroup::ugDelChar);
    if (!(c.flags & KateView::cfBackspaceIndents)) {
      // ordinary backspace
      c.cursor.x--;
      recordDelete(c.cursor, 1);
    } else {
      // backspace indents: erase to next indent position
      int l = 1; // del one char

      TextLine::Ptr textLine = getTextLine(c.cursor.y);
      int pos = textLine->firstChar();
      if (pos < 0 || pos >= c.cursor.x) {
        // only spaces on left side of cursor
        // search a line with less spaces
        int y = c.cursor.y;
        while (y > 0) {
          textLine = getTextLine(--y);
          pos = textLine->firstChar();
          if (pos >= 0 && pos < c.cursor.x) {
            l = c.cursor.x - pos; // del more chars
            break;
          }
        }
      }
      // break effectively jumps here
      c.cursor.x -= l;
      recordDelete(c.cursor, l);
    }
  } else {
    // c.cursor.x == 0: wrap to previous line
    recordStart(c, KateActionGroup::ugDelLine);
    c.cursor.y--;
    c.cursor.x = getTextLine(c.cursor.y)->length();
    recordAction(KateAction::delLine,c.cursor);
  }
  recordEnd(c);
}


void KateDocument::del(VConfig &c) {
  TextLine::Ptr textLine = getTextLine(c.cursor.y);
  int len =  (c.flags & KateView::cfRemoveSpaces) ? textLine->lastChar() : textLine->length();
  if (c.cursor.x < len/*getTextLine(c.cursor.y)->length()*/) {
    // delete one character
    recordStart(c, KateActionGroup::ugDelChar);
    recordDelete(c.cursor, 1);
    recordEnd(c);
  } else {
    if (c.cursor.y < lastLine()) {
      // wrap next line to this line
      textLine->truncate(c.cursor.x); // truncate spaces
      recordStart(c, KateActionGroup::ugDelLine);
      recordAction(KateAction::delLine,c.cursor);
      recordEnd(c);
    }
  }
}

void KateDocument::clear() {
  PointStruc cursor;
  KateView *view;

  setPseudoModal(0L);
  cursor.x = cursor.y = 0;
  for (view = views.first(); view != 0L; view = views.next() ) {
    view->updateCursor(cursor);
    view->tagAll();
  }

  eolMode = KateDocument::eolUnix;

  buffer->clear();
  longestLine = buffer->line(0);

  maxLength = 0;

  select.x = -1;

  selectStart = 0xffffff;
  selectEnd = 0;
  oldMarkState = false;

  setModified(false);

  undoList.clear();
  currentUndo = 0;
  newUndo();
}

void KateDocument::cut(VConfig &c) {

  if (selectEnd < selectStart) return;

  copy(c.flags);
  delMarkedText(c);
}

void KateDocument::copy(int flags) {

  if (selectEnd < selectStart) return;

  QString s = markedText(flags);
  if (!s.isEmpty()) {
//#if defined(_WS_X11_)
    if (m_singleSelection)
      disconnect(QApplication::clipboard(), SIGNAL(dataChanged()), this, 0);
//#endif
    QApplication::clipboard()->setText(s);
//#if defined(_WS_X11_)
    if (m_singleSelection) {
      connect(QApplication::clipboard(), SIGNAL(dataChanged()),
        this, SLOT(clipboardChanged()));
    }
//#endif
  }
}

void KateDocument::paste(VConfig &c) {
  QString s = QApplication::clipboard()->text();
  if (!s.isEmpty()) {
    insert(c, s);
  }
}

void KateDocument::toggleRect(int start, int end, int x1, int x2) {
  int z, line;
  bool t;

  if (x1 > x2) {
    z = x1;
    x1 = x2;
    x2 = z;
  }
  if (start > end) {
    z = start;
    start = end;
    end = z;
  }

  t = false;
  for (line = start; line < end; line++) {
    int x, oldX, s, e, newX1, newX2;
    QChar ch;
    Attribute *a;

    TextLine::Ptr textLine = getTextLine(line);

    //--- speed optimization
    //s = textPos(textLine, x1, newX1);
    x = oldX = z = 0;
    while (x < x1) { // && z < len) {
      oldX = x;
      ch = textLine->getChar(z);
      a = &m_attribs[textLine->getAttr(z)];

    if (ch == '\t')
      x += m_tabWidth - (x % m_tabWidth);
    else if (a->bold && a->italic)
      x += myFontMetricsBI.width(ch);
    else if (a->bold)
      x += myFontMetricsBold.width(ch);
    else if (a->italic)
      x += myFontMetricsItalic.width(ch);
    else
      x += myFontMetrics.width(ch);

      z++;
    }
    s = z;
    if (x1 - oldX < x - x1 && z > 0) {
      s--;
      newX1 = oldX;
    } else newX1 = x;
    //e = textPos(textLine, x2, newX2);
    while (x < x2) { // && z < len) {
      oldX = x;
      ch = textLine->getChar(z);
      a = &m_attribs[textLine->getAttr(z)];

    if (ch == '\t')
      x += m_tabWidth - (x % m_tabWidth);
    else if (a->bold && a->italic)
      x += myFontMetricsBI.width(ch);
    else if (a->bold)
      x += myFontMetricsBold.width(ch);
    else if (a->italic)
      x += myFontMetricsItalic.width(ch);
    else
      x += myFontMetrics.width(ch);

      z++;
    }
    e = z;
    if (x2 - oldX < x - x2 && z > 0) {
      e--;
      newX2 = oldX;
    } else newX2 = x;
    //---

    if (e > s) {
      textLine->toggleSelect(s, e);
      tagLineRange(line, newX1, newX2);
      t = true;
    }
  }
  if (t) {
    end--;
//    tagLines(start, end);

    if (start < selectStart) selectStart = start;
    if (end > selectEnd) selectEnd = end;
    emit selectionChanged();
  }
}

void KateDocument::selectTo(VConfig &c, PointStruc &cursor, int cXPos) {
  //c.cursor = old cursor position
  //cursor = new cursor position

  if (c.cursor.x != select.x || c.cursor.y != select.y) {
    //new selection

    if (!(c.flags & KateView::cfKeepSelection)) deselectAll();
//      else recordReset();

    anchor = c.cursor;
    aXPos = c.cXPos;
  }

  if (!(c.flags & KateView::cfVerticalSelect)) {
    //horizontal selections
    int x, y, sXPos;
    int ex, ey, eXPos;
    bool sel;

    if (cursor.y > c.cursor.y || (cursor.y == c.cursor.y && cursor.x > c.cursor.x)) {
      x = c.cursor.x;
      y = c.cursor.y;
      sXPos = c.cXPos;
      ex = cursor.x;
      ey = cursor.y;
      eXPos = cXPos;
      sel = true;
    } else {
      x = cursor.x;
      y = cursor.y;
      sXPos = cXPos;
      ex = c.cursor.x;
      ey = c.cursor.y;
      eXPos = c.cXPos;
      sel = false;
    }

//    tagLines(y, ye);
    if (y < ey) {
      //tagLineRange(y, sXPos, 0xffffff);
      tagLines(y, ey -1);
      tagLineRange(ey, 0, eXPos);
    } else tagLineRange(y, sXPos, eXPos);

    if (y < selectStart) selectStart = y;
    if (ey > selectEnd) selectEnd = ey;

    TextLine::Ptr textLine = getTextLine(y);

    if (c.flags & KateView::cfXorSelect) {
      //xor selection with old selection
      while (y < ey) {
        textLine->toggleSelectEol(x);
        x = 0;
        y++;
        textLine = getTextLine(y);
      }
      textLine->toggleSelect(x, ex);
    } else {
      //set selection over old selection

      if (anchor.y > y || (anchor.y == y && anchor.x > x)) {
        if (anchor.y < ey || (anchor.y == ey && anchor.x < ex)) {
          sel = !sel;
          while (y < anchor.y) {
            textLine->selectEol(sel, x);
            x = 0;
            y++;
            textLine = getTextLine(y);
          }
          textLine->select(sel, x, anchor.x);
          x = anchor.x;
        }
        sel = !sel;
      }
      while (y < ey) {
        textLine->selectEol(sel, x);
        x = 0;
        y++;
        textLine = getTextLine(y);
      }
      textLine->select(sel, x, ex);
    }
  } else {
    //vertical (block) selections
//    int ax, sx, ex;

//    ax = textWidth(anchor);
//    sx = textWidth(start);
//    ex = textWidth(end);

    toggleRect(c.cursor.y + 1, cursor.y + 1, aXPos, c.cXPos);
    toggleRect(anchor.y, cursor.y + 1, c.cXPos, cXPos);
  }
  select = cursor;
  optimizeSelection();
  emit selectionChanged();
}


void KateDocument::selectAll() {
  int z;
  TextLine::Ptr textLine;

  select.x = -1;

//  if (selectStart != 0 || selectEnd != lastLine()) recordReset();

  selectStart = 0;
  selectEnd = lastLine();

  tagLines(selectStart,selectEnd);

  for (z = selectStart; z < selectEnd; z++) {
    textLine = getTextLine(z);
    textLine->selectEol(true,0);
  }
  textLine = getTextLine(z);
  textLine->select(true,0,textLine->length());
  emit selectionChanged();
}

void KateDocument::deselectAll() {
  select.x = -1;
  if (selectEnd < selectStart) return;

//  recordReset();

  tagLines(selectStart,selectEnd);

  for (int z = selectStart; z <= selectEnd; z++) {
    TextLine::Ptr textLine = getTextLine(z);
    textLine->selectEol(false,0);
  }
  selectStart = 0xffffff;
  selectEnd = 0;
  emit selectionChanged();
}

void KateDocument::invertSelection() {
  TextLine::Ptr textLine;

  select.x = -1;

//  if (selectStart != 0 || selectEnd != lastLine()) recordReset();

  selectStart = 0;
  selectEnd = lastLine();

  tagLines(selectStart,selectEnd);

  for (int z = selectStart; z < selectEnd; z++) {
    textLine = getTextLine(z);
    textLine->toggleSelectEol(0);
  }
  textLine = getTextLine(selectEnd);
  textLine->toggleSelect(0,textLine->length());
  optimizeSelection();
  emit selectionChanged();
}

void KateDocument::selectWord(PointStruc &cursor, int flags) {
  int start, end, len;

  TextLine::Ptr textLine = getTextLine(cursor.y);
  len = textLine->length();
  start = end = cursor.x;
  while (start > 0 && m_highlight->isInWord(textLine->getChar(start - 1))) start--;
  while (end < len && m_highlight->isInWord(textLine->getChar(end))) end++;
  if (end <= start) return;
  if (!(flags & KateView::cfKeepSelection)) deselectAll();
//    else recordReset();

  textLine->select(true, start, end);

  anchor.x = start;
  select.x = end;
  anchor.y = select.y = cursor.y;
  tagLines(cursor.y, cursor.y);
  if (cursor.y < selectStart) selectStart = cursor.y;
  if (cursor.y > selectEnd) selectEnd = cursor.y;
  emit selectionChanged();
}

void KateDocument::selectLength(PointStruc &cursor, int length, int flags) {
  int start, end;

  TextLine::Ptr textLine = getTextLine(cursor.y);
  start = cursor.x;
  end = start + length;
  if (end <= start) return;
  if (!(flags & KateView::cfKeepSelection)) deselectAll();

  textLine->select(true, start, end);

  anchor.x = start;
  select.x = end;
  anchor.y = select.y = cursor.y;
  tagLines(cursor.y, cursor.y);
  if (cursor.y < selectStart) selectStart = cursor.y;
  if (cursor.y > selectEnd) selectEnd = cursor.y;
  emit selectionChanged();
}

void KateDocument::doIndent(VConfig &c, int change) {

  c.cursor.x = 0;

  recordStart(c, (change < 0) ? KateActionGroup::ugUnindent
    : KateActionGroup::ugIndent);

  if (selectEnd < selectStart) {
    // single line
    optimizeLeadingSpace(c.cursor.y, c.flags, change);
  } else {
    // entire selection
    TextLine::Ptr textLine;
    int line, z;
    QChar ch;

    if (c.flags & KateView::cfKeepIndentProfile && change < 0) {
      // unindent so that the existing indent profile doesn´t get screwed
      // if any line we may unindent is already full left, don't do anything
      for (line = selectStart; line <= selectEnd; line++) {
        textLine = getTextLine(line);
        if (textLine->isSelected() || textLine->numSelected()) {
          for (z = 0; z < tabChars; z++) {
            ch = textLine->getChar(z);
            if (ch == '\t') break;
            if (ch != ' ') {
              change = 0;
              goto jumpOut;
            }
          }
        }
      }
      jumpOut:;
    }

    for (line = selectStart; line <= selectEnd; line++) {
      textLine = getTextLine(line);
      if (textLine->isSelected() || textLine->numSelected()) {
        optimizeLeadingSpace(line, c.flags, change);
      }
    }
  }
  // recordEnd now removes empty undo records
  recordEnd(c.view, c.cursor, c.flags | KateView::cfPersistent);
}

/*
  Optimize the leading whitespace for a single line.
  If change is > 0, it adds indentation units (tabChars)
  if change is == 0, it only optimizes
  If change is < 0, it removes indentation units
  This will be used to indent, unindent, and optimal-fill a line.
  If excess space is removed depends on the flag cfKeepExtraSpaces
  which has to be set by the user
*/
void KateDocument::optimizeLeadingSpace(int line, int flags, int change) {
  int len;
  int chars, space, okLen;
  QChar ch;
  int extra;
  QString s;
  PointStruc cursor;

  TextLine::Ptr textLine = getTextLine(line);
  len = textLine->length();
  space = 0; // length of space at the beginning of the textline
  okLen = 0; // length of space which does not have to be replaced
  for (chars = 0; chars < len; chars++) {
    ch = textLine->getChar(chars);
    if (ch == ' ') {
      space++;
      if (flags & KateView::cfSpaceIndent && okLen == chars) okLen++;
    } else if (ch == '\t') {
      space += tabChars - space % tabChars;
      if (!(flags & KateView::cfSpaceIndent) && okLen == chars) okLen++;
    } else break;
  }

  space += change*tabChars; // modify space width
  // if line contains only spaces it will be cleared
  if (space < 0 || chars == len) space = 0;

  extra = space % tabChars; // extra spaces which don´t fit the indentation pattern
  if (flags & KateView::cfKeepExtraSpaces) chars -= extra;

  if (flags & KateView::cfSpaceIndent) {
    space -= extra;
    ch = ' ';
  } else {
    space /= tabChars;
    ch = '\t';
  }

  // don´t replace chars which are already ok
  cursor.x = QMIN(okLen, QMIN(chars, space));
  chars -= cursor.x;
  space -= cursor.x;
  if (chars == 0 && space == 0) return; //nothing to do

  s.fill(ch, space);

//printf("chars %d insert %d cursor.x %d\n", chars, insert, cursor.x);
  cursor.y = line;
  recordReplace(cursor, chars, s);
}

void KateDocument::doComment(VConfig &c, int change)
{
  c.flags |=KateView:: cfPersistent;

  recordStart(c, (change < 0) ? KateActionGroup::ugUncomment
    : KateActionGroup::ugComment);

  QString startComment = m_highlight->getCommentStart();
  QString startLineComment = m_highlight->getCommentSingleLineStart();
  QString endComment = m_highlight->getCommentEnd();

  int startCommentLen = startComment.length();
  int startLineCommentLen = startLineComment.length();
  int endCommentLen = endComment.length();

  if (change > 0)
  {
    if ( !hasMarkedText() )
    {
      if (startLineComment != "")
      {
        // Add a start comment mark
        c.cursor.x = 0;
        recordReplace(c.cursor, 0, startLineComment);
      }
      else  if ((startComment != "") && (endComment != ""))
      {
        // Add a start comment mark
        c.cursor.x = 0;
        recordReplace(c.cursor, 0, startComment);

        // Add an end comment mark
        TextLine* textline = getTextLine(c.cursor.y);
        c.cursor.x = textline->length();
        recordReplace(c.cursor, 0, endComment);
        c.cursor.x = 0;
      }
    }
    else if ((startComment != "") && (endComment != ""))
    {
      QString marked (c.view->markedText ());
      int preDeleteLine = -1, preDeleteCol = -1;
      c.view->getCursorPosition (&preDeleteLine, &preDeleteCol);

      if (marked.length() > 0)
        c.view->keyDelete ();

      int line = -1, col = -1;
      c.view->getCursorPosition (&line, &col);

      c.view->insertText (startComment + marked + endComment);
    }
  }
  else
  {
    if ( !hasMarkedText() )
    {
      TextLine* textline = getTextLine(c.cursor.y);

      if(textline->startingWith(startLineComment))
      {
        // Remove start comment mark
        c.cursor.x = 0;
        recordReplace(c.cursor, startLineCommentLen, "");
      }
      else if (textline->startingWith(startComment) && textline->endingWith(endComment))
      {
        // Remove start comment mark
        c.cursor.x = 0;
        recordReplace(c.cursor, startCommentLen, "");

        // Remove end comment mark
        if(endComment != "")
        {
          c.cursor.x = textline->length() - endCommentLen;
          recordReplace(c.cursor, endCommentLen, "");
          c.cursor.x = 0;
        }
      }
    }
    else
    {
      QString marked (c.view->markedText ());
      int preDeleteLine = -1, preDeleteCol = -1;
      c.view->getCursorPosition (&preDeleteLine, &preDeleteCol);

      int start = marked.find (startComment);
      int end = marked.findRev (endComment);

      if ((start > -1) && (end > -1))
      {
        marked.remove (start, startCommentLen);
        marked.remove (end-startCommentLen, endCommentLen);

        c.view->keyDelete ();

        int line = -1, col = -1;
        c.view->getCursorPosition (&line, &col);
        c.view->insertText (marked);
      }
    }
  }

  recordEnd(c.view, c.cursor, c.flags | KateView::cfPersistent);
}


QString KateDocument::text() const
{
  QString s;

  for (int i=0; i < buffer->count(); i++)
  {
    TextLine::Ptr textLine = buffer->line(i);
    s.insert(s.length(), textLine->getText(), textLine->length());
    if ( (i < (buffer->count()-1)) )
      s.append('\n');
  }

  return s;
}

QString KateDocument::getWord(PointStruc &cursor) {
  int start, end, len;

  TextLine::Ptr textLine = getTextLine(cursor.y);
  len = textLine->length();
  start = end = cursor.x;
  while (start > 0 && m_highlight->isInWord(textLine->getChar(start - 1))) start--;
  while (end < len && m_highlight->isInWord(textLine->getChar(end))) end++;
  len = end - start;
  return QString(&textLine->getText()[start], len);
}

void KateDocument::setText(const QString &s) {
  int pos;
  QChar ch;

  clear();

  int line=1;

  TextLine::Ptr textLine = buffer->line(0);
  for (pos = 0; pos <= (int) s.length(); pos++) {
    ch = s[pos];
    if (ch.isPrint() || ch == '\t') {
      textLine->append(&ch, 1);
    } else if (ch == '\n')
    {
      textLine = new TextLine();
      buffer->insertLine (line, textLine);
      line++;
    }
  }
  updateLines();
}


QString KateDocument::markedText(int flags) {
  TextLine::Ptr textLine;
  int len, z, start, end, i;

  len = 1;
  if (!(flags & KateView::cfVerticalSelect)) {
    for (z = selectStart; z <= selectEnd; z++) {
      textLine = getTextLine(z);
      len += textLine->numSelected();
      if (textLine->isSelected()) len++;
    }
    QString s;
    len = 0;
    for (z = selectStart; z <= selectEnd; z++) {
      textLine = getTextLine(z);
      end = 0;
      do {
        start = textLine->findUnselected(end);
        end = textLine->findSelected(start);
        for (i = start; i < end; i++) {
          s[len] = textLine->getChar(i);
          len++;
        }
      } while (start < end);
      if (textLine->isSelected()) {
        s[len] = '\n';
        len++;
      }
    }
//    s[len] = '\0';
    return s;
  } else {
    for (z = selectStart; z <= selectEnd; z++) {
      textLine = getTextLine(z);
      len += textLine->numSelected() + 1;
    }
    QString s;
    len = 0;
    for (z = selectStart; z <= selectEnd; z++) {
      textLine = getTextLine(z);
      end = 0;
      do {
        start = textLine->findUnselected(end);
        end = textLine->findSelected(start);
        for (i = start; i < end; i++) {
          s[len] = textLine->getChar(i);
          len++;
        }
      } while (start < end);
      s[len] = '\n';
      len++;
    }
//    s[len] = '\0';       //  the final \0 is not counted in length()
    return s;
  }
}

void KateDocument::delMarkedText(VConfig &c/*, bool undo*/) {
  int end = 0;

  if (selectEnd < selectStart) return;

  // the caller may have already started an undo record for the current action
//  if (undo)

  //auto deletion of the marked text occurs not very often and can therefore
  //  be recorded separately
  recordStart(c, KateActionGroup::ugDelBlock);

  for (c.cursor.y = selectEnd; c.cursor.y >= selectStart; c.cursor.y--) {
    TextLine::Ptr textLine = getTextLine(c.cursor.y);

    c.cursor.x = textLine->length();
    do {
      end = textLine->findRevUnselected(c.cursor.x);
      if (end == 0) break;
      c.cursor.x = textLine->findRevSelected(end);
      recordDelete(c.cursor, end - c.cursor.x);
    } while (true);
    end = c.cursor.x;
    c.cursor.x = textLine->length();
    if (textLine->isSelected()) recordAction(KateAction::delLine,c.cursor);
  }
  c.cursor.y++;
  /*if (end < c.cursor.x)*/ c.cursor.x = end;

  selectEnd = -1;
  select.x = -1;

  /*if (undo)*/ recordEnd(c);
}

void KateDocument::tagLineRange(int line, int x1, int x2) {
  int z;

  for (z = 0; z < (int) views.count(); z++) {
    views.at(z)->tagLines(line, line, x1, x2);
  }
}

void KateDocument::tagLines(int start, int end) {
  int z;

  for (z = 0; z < (int) views.count(); z++) {
    views.at(z)->tagLines(start, end, 0, 0xffffff);
  }
}

void KateDocument::tagAll() {
  int z;

  for (z = 0; z < (int) views.count(); z++) {
    views.at(z)->tagAll();
  }
}

void KateDocument::updateLines(int startLine, int endLine, int flags, int cursorY) {
  TextLine::Ptr textLine;
  int line, last_line;
  int ctxNum, endCtx;
//  kdDebug(13020)<<"******************KateDocument::updateLines Checkpoint 1"<<endl;
  if (buffer->line(startLine)==0) {kdDebug(13020)<<"********************No buffer for line " << startLine << " found**************"<<endl; return;};
//  kdDebug(13020)<<"KateDocument::updateLines Checkpoint 2"<<endl;
  last_line = lastLine();
//  if (endLine >= last_line) endLine = last_line;

  line = startLine;
  ctxNum = 0;
  if (line > 0) ctxNum = getTextLine(line - 1)->getContext();
  do {
//    kdDebug(13020)<<QString("**************Working on line: %1").arg(line)<<endl;
    textLine = getTextLine(line);
    if (textLine==0) kdDebug(13020)<<"****updateLines()>> error textLine==0"<<endl;
    if (line <= endLine && line != cursorY) {
      if (flags & KateView::cfRemoveSpaces) textLine->removeSpaces();
      updateMaxLength(textLine);
    }
    endCtx = textLine->getContext();
    ctxNum = m_highlight->doHighlight(ctxNum,textLine);
    textLine->setContext(ctxNum);
    line++;
  } while ((buffer->line(line)!=0) && (line <= endLine || endCtx != ctxNum));
//  kdDebug(13020)<<"updateLines :: while loop left"<<endl;
  tagLines(startLine, line - 1);
}


void KateDocument::updateMaxLength(TextLine::Ptr &textLine) {
  int len;

  len = textWidth(textLine,textLine->length());

  if (len > maxLength) {
    longestLine = textLine;
    maxLength = len;
    newDocGeometry = true;
  } else {
    if (!longestLine || (textLine == longestLine && len <= maxLength*3/4)) {
      maxLength = -1;
      for (int i = 0; i < numLines();i++) {
        textLine = getTextLine(i);
        len = textWidth(textLine,textLine->length());
        if (len > maxLength) {
          maxLength = len;
          longestLine = textLine;
        }
      }
      newDocGeometry = true;
    }
  }
}

void KateDocument::slotBufferChanged() {
  newDocGeometry = true;
  //updateLines();//JW
  updateViews();
}

void KateDocument::slotBufferHighlight(long start,long stop) {
  kdDebug(13020)<<"KateDocument::slotBufferHighlight"<<QString("%1-%2").arg(start).arg(stop)<<endl;
  updateLines(start,stop);
//  buffer->startLoadTimer();
}

void KateDocument::updateViews(KateView *exclude) {
  KateView *view;
  int flags;
  bool markState = hasMarkedText();

  flags = (newDocGeometry) ? KateView::ufDocGeometry : 0;
  for (view = views.first(); view != 0L; view = views.next() ) {
    if (view != exclude) view->updateView(flags);

    // notify every view about the changed mark state....
    if (oldMarkState != markState) emit view->newMarkStatus();
  }
  oldMarkState = markState;
  newDocGeometry = false;
}

QColor &KateDocument::cursorCol(int x, int y) {
  int attr;
  Attribute *a;

  TextLine::Ptr textLine = getTextLine(y);
  attr = textLine->getRawAttr(x);
  a = &m_attribs[attr & taAttrMask];
  if (attr & taSelected) return a->selCol; else return a->col;
}

void KateDocument::paintTextLine(QPainter &paint, int line, int xStart, int xEnd, bool showTabs)
{
  paintTextLine (paint, line, 0, xStart, xEnd, showTabs);
}

void KateDocument::paintTextLine(QPainter &paint, int line, int y, int xStart, int xEnd, bool showTabs)
{
  TextLine::Ptr textLine;
  int len;
  const QChar *s;
  int z, x;
  QChar ch;
  Attribute *a = 0L;
  int attr, nextAttr;
  int xs;
  int xc, zc;

  if (line > lastLine()) {
    paint.fillRect(0, y, xEnd - xStart,fontHeight, colors[0]);
    return;
  }

  textLine = getTextLine(line);
  len = textLine->length();
  s = textLine->getText();

  // skip to first visible character
  x = 0;
  z = 0;
  do {
    xc = x;
    zc = z;
    if (z == len) break;
    ch = s[z];//textLine->getChar(z);
    if (ch == '\t') {
      x += m_tabWidth - (x % m_tabWidth);
    } else {
     a = &m_attribs[textLine->getAttr(z)];

   if (a->bold && a->italic)
      x += myFontMetricsBI.width(ch);
    else if (a->bold)
      x += myFontMetricsBold.width(ch);
    else if (a->italic)
      x += myFontMetricsItalic.width(ch);
    else
      x += myFontMetrics.width(ch);
    }
    z++;
  } while (x <= xStart);

  // draw background
  xs = xStart;
  attr = textLine->getRawAttr(zc);
  while (x < xEnd)
  {
    nextAttr = textLine->getRawAttr(z);
    if ((nextAttr ^ attr) & taSelected)
    {
      if (attr & taSelected)
        paint.fillRect(xs - xStart, y, x - xs, fontHeight, colors[1]);
      else
        paint.fillRect(xs - xStart, y, x - xs, fontHeight, colors[0]);

      xs = x;
      attr = nextAttr;
    }

    if (z == len) break;

    ch = s[z];//textLine->getChar(z);

    if (ch == '\t')
      x += m_tabWidth - (x % m_tabWidth);
    else
    {
      a = &m_attribs[textLine->getAttr(z)];

      if (a->bold && a->italic)
        x += myFontMetricsBI.width(ch);
      else if (a->bold)
        x += myFontMetricsBold.width(ch);
      else if (a->italic)
        x += myFontMetricsItalic.width(ch);
      else
        x += myFontMetrics.width(ch);
    }
    z++;
  }

  if (attr & taSelected)
    paint.fillRect(xs - xStart, y, xEnd - xs, fontHeight, colors[1]);
  else
    paint.fillRect(xs - xStart, y, xEnd - xs, fontHeight, colors[0]);

  len = z; //reduce length to visible length

  // draw text
  x = xc;
  z = zc;
  y += fontAscent;// -1;
  attr = -1;
  while (z < len) {
    ch = s[z];//textLine->getChar(z);
    if (ch == '\t') {
      if (z > zc) {
        //this should cause no copy at all
        QConstString str((QChar *) &s[zc], z - zc /*+1*/);
        QString s = str.string();
        paint.drawText(x - xStart, y, s);

   if (a->bold && a->italic)
      x += myFontMetricsBI.width(s);
    else if (a->bold)
      x += myFontMetricsBold.width(s);
    else if (a->italic)
      x += myFontMetricsItalic.width(s);
    else
      x += myFontMetrics.width(s);
      }
      zc = z +1;

      if (showTabs) {
        nextAttr = textLine->getRawAttr(z);
        if (nextAttr != attr) {
          attr = nextAttr;
          a = &m_attribs[attr & taAttrMask];

          if (attr & taSelected) paint.setPen(a->selCol);
            else paint.setPen(a->col);

   if (a->bold && a->italic)
     paint.setFont(myFontBI);
    else if (a->bold)
      paint.setFont(myFontBold);
    else if (a->italic)
      paint.setFont(myFontItalic);
    else
      paint.setFont(myFont);
        }

//        paint.drawLine(x - xStart, y -2, x - xStart, y);
//        paint.drawLine(x - xStart, y, x - xStart + 2, y);
        paint.drawPoint(x - xStart, y);
        paint.drawPoint(x - xStart +1, y);
        paint.drawPoint(x - xStart, y -1);
      }
      x += m_tabWidth - (x % m_tabWidth);
    } else {
      nextAttr = textLine->getRawAttr(z);
      if (nextAttr != attr) {
        if (z > zc) {
          QConstString str((QChar *) &s[zc], z - zc /*+1*/);
          QString s = str.string();
          paint.drawText(x - xStart, y, s);

   if (a->bold && a->italic)
      x += myFontMetricsBI.width(s);
    else if (a->bold)
      x += myFontMetricsBold.width(s);
    else if (a->italic)
      x += myFontMetricsItalic.width(s);
    else
      x += myFontMetrics.width(s);
          zc = z;
        }
        attr = nextAttr;
        a = &m_attribs[attr & taAttrMask];

        if (attr & taSelected) paint.setPen(a->selCol);
          else paint.setPen(a->col);

   if (a->bold && a->italic)
     paint.setFont(myFontBI);
    else if (a->bold)
      paint.setFont(myFontBold);
    else if (a->italic)
      paint.setFont(myFontItalic);
    else
      paint.setFont(myFont);
      }
    }
    z++;
  }
  if (z > zc) {
    QConstString str((QChar *) &s[zc], z - zc /*+1*/);
    paint.drawText(x - xStart, y, str.string());
  }
}

// Applies the search context, and returns whether a match was found. If one is,
// the length of the string matched is also returned.
bool KateDocument::doSearch(SConfig &sc, const QString &searchFor) {
  int line, col;
  int searchEnd;
  int bufLen, tlen;
  QChar *t;
  TextLine::Ptr textLine;
  int pos, newPos;

  if (searchFor.isEmpty()) return false;

  bufLen = 0;
  t = 0L;

  line = sc.cursor.y;
  col = sc.cursor.x;
  if (!(sc.flags & KateView::sfBackward)) {
    //forward search
    if (sc.flags & KateView::sfSelected) {
      if (line < selectStart) {
        line = selectStart;
        col = 0;
      }
      searchEnd = selectEnd;
    } else searchEnd = lastLine();

    while (line <= searchEnd) {
      textLine = getTextLine(line);
      tlen = textLine->length();
      if (tlen > bufLen) {
        delete t;
        bufLen = (tlen + 255) & (~255);
        t = new QChar[bufLen];
      }
      memcpy(t, textLine->getText(), tlen*sizeof(QChar));
      if (sc.flags & KateView::sfSelected) {
        pos = 0;
        do {
          pos = textLine->findSelected(pos);
          newPos = textLine->findUnselected(pos);
          memset(&t[pos], 0, (newPos - pos)*sizeof(QChar));
          pos = newPos;
        } while (pos < tlen);
      }

      QString text(t, tlen);
      if (sc.flags & KateView::sfWholeWords) {
        // Until the end of the line...
        while (col < tlen) {
          // ...find the next match.
          col = sc.search(text, col);
          if (col != -1) {
            // Is the match delimited correctly?
            if (((col == 0) || (!m_highlight->isInWord(t[col]))) &&
              ((col + sc.matchedLength == tlen) || (!m_highlight->isInWord(t[col + sc.matchedLength])))) {
              goto found;
            }
            else {
              // Start again from the next character.
              col++;
            }
          }
          else {
            // No match.
            break;
          }
        }
      }
      else {
        // Non-whole-word search.
        col = sc.search(text, col);
        if (col != -1)
          goto found;
      }
      col = 0;
      line++;
    }
  } else {
    // backward search
    if (sc.flags & KateView::sfSelected) {
      if (line > selectEnd) {
        line = selectEnd;
        col = -1;
      }
      searchEnd = selectStart;
    } else searchEnd = 0;

    while (line >= searchEnd) {
      textLine = getTextLine(line);
      tlen = textLine->length();
      if (tlen > bufLen) {
        delete t;
        bufLen = (tlen + 255) & (~255);
        t = new QChar[bufLen];
      }
      memcpy(t, textLine->getText(), tlen*sizeof(QChar));
      if (sc.flags & KateView::sfSelected) {
        pos = 0;
        do {
          pos = textLine->findSelected(pos);
          newPos = textLine->findUnselected(pos);
          memset(&t[pos], 0, (newPos - pos)*sizeof(QChar));
          pos = newPos;
        } while (pos < tlen);
      }

      if (col < 0 || col > tlen) col = tlen;

      QString text(t, tlen);
      if (sc.flags & KateView::sfWholeWords) {
        // Until the beginning of the line...
        while (col >= 0) {
          // ...find the next match.
          col = sc.search(text, col);
          if (col != -1) {
            // Is the match delimited correctly?
            if (((col == 0) || (!m_highlight->isInWord(t[col]))) &&
              ((col + sc.matchedLength == tlen) || (!m_highlight->isInWord(t[col + sc.matchedLength])))) {
              goto found;
            }
            else {
              // Start again from the previous character.
              col--;
            }
          }
          else {
            // No match.
            break;
          }
        }
      }
      else {
        // Non-whole-word search.
        col = sc.search(text, col);
        if (col != -1)
          goto found;
      }
      col = -1;
      line--;
    }
  }
  sc.flags |= KateView::sfWrapped;
  return false;
found:
  if (sc.flags & KateView::sfWrapped) {
    if ((line > sc.startCursor.y || (line == sc.startCursor.y && col >= sc.startCursor.x))
      ^ ((sc.flags & KateView::sfBackward) != 0)) return false;
  }
  sc.cursor.x = col;
  sc.cursor.y = line;
  return true;
}

void KateDocument::tagLine(int line) {

  if (tagStart > line) tagStart = line;
  if (tagEnd < line) tagEnd = line;
}

void KateDocument::insLine(int line) {
  KateView *view;

  if (selectStart >= line) selectStart++;
  if (selectEnd >= line) selectEnd++;
  if (tagStart >= line) tagStart++;
  if (tagEnd >= line) tagEnd++;

  newDocGeometry = true;
  for (view = views.first(); view != 0L; view = views.next() ) {
    view->insLine(line);
  }
}

void KateDocument::delLine(int line) {
  KateView *view;

  if (selectStart >= line && selectStart > 0) selectStart--;
  if (selectEnd >= line) selectEnd--;
  if (tagStart >= line && tagStart > 0) tagStart--;
  if (tagEnd >= line) tagEnd--;

  newDocGeometry = true;
  for (view = views.first(); view != 0L; view = views.next() ) {
    view->delLine(line);
  }
}

void KateDocument::optimizeSelection() {
  TextLine::Ptr textLine;

  while (selectStart <= selectEnd) {
    textLine = getTextLine(selectStart);
    if (textLine->isSelected() || textLine->numSelected() > 0) break;
    selectStart++;
  }
  while (selectEnd >= selectStart) {
    textLine = getTextLine(selectEnd);
    if (textLine->isSelected() || textLine->numSelected() > 0) break;
    selectEnd--;
  }
  if (selectStart > selectEnd) {
    selectStart = 0xffffff;
    selectEnd = 0;
  }
}

void KateDocument::doAction(KateAction *a) {

  switch (a->action) {
    case KateAction::replace:
      doReplace(a);
      break;
    case KateAction::wordWrap:
      doWordWrap(a);
      break;
    case KateAction::wordUnWrap:
      doWordUnWrap(a);
      break;
    case KateAction::newLine:
      doNewLine(a);
      break;
    case KateAction::delLine:
      doDelLine(a);
      break;
    case KateAction::insLine:
      doInsLine(a);
      break;
    case KateAction::killLine:
      doKillLine(a);
      break;
/*    case KateAction::doubleLine:
      break;
    case KateAction::removeLine:
      break;*/
  }
}

void KateDocument::doReplace(KateAction *a) {
  TextLine::Ptr textLine;
  int l;

  //exchange current text with stored text in KateAction *a

  textLine = getTextLine(a->cursor.y);
  l = textLine->length() - a->cursor.x;
  if (l > a->len) l = a->len;

  QString oldText(&textLine->getText()[a->cursor.x], (l < 0) ? 0 : l);
  textLine->replace(a->cursor.x, a->len, a->text.unicode(), a->text.length());

  a->len = a->text.length();
  a->text = oldText;

  buffer->changeLine(a->cursor.y);

  tagLine(a->cursor.y);
}

void KateDocument::doWordWrap(KateAction *a) {
  TextLine::Ptr textLine;

  textLine = getTextLine(a->cursor.y - 1);
  a->len = textLine->length() - a->cursor.x;
  textLine->wrap(getTextLine(a->cursor.y),a->len);

  buffer->changeLine(a->cursor.y - 1);
  buffer->changeLine(a->cursor.y);

  tagLine(a->cursor.y - 1);
  tagLine(a->cursor.y);
  if (selectEnd == a->cursor.y - 1) selectEnd++;

  a->action = KateAction::wordUnWrap;
}

void KateDocument::doWordUnWrap(KateAction *a) {
  TextLine::Ptr textLine;

  textLine = getTextLine(a->cursor.y - 1);
//  textLine->setLength(a->len);
  textLine->unWrap(a->len, getTextLine(a->cursor.y),a->cursor.x);

  buffer->changeLine(a->cursor.y - 1);
  buffer->changeLine(a->cursor.y);

  tagLine(a->cursor.y - 1);
  tagLine(a->cursor.y);

  a->action = KateAction::wordWrap;
}

void KateDocument::doNewLine(KateAction *a) {
  TextLine::Ptr textLine, newLine;

  textLine = getTextLine(a->cursor.y);
  newLine = new TextLine(textLine->getRawAttr(), textLine->getContext());
  textLine->wrap(newLine,a->cursor.x);

  buffer->insertLine(a->cursor.y + 1, newLine);
  buffer->changeLine(a->cursor.y);

  insLine(a->cursor.y + 1);
  tagLine(a->cursor.y);
  tagLine(a->cursor.y + 1);
  if (selectEnd == a->cursor.y) selectEnd++;//addSelection(a->cursor.y + 1);

  a->action = KateAction::delLine;
}

void KateDocument::doDelLine(KateAction *a) {
  TextLine::Ptr textLine, nextLine;

  textLine = getTextLine(a->cursor.y);
  nextLine = getTextLine(a->cursor.y+1);
//  textLine->setLength(a->cursor.x);
  textLine->unWrap(a->cursor.x, nextLine,nextLine->length());
  textLine->setContext(nextLine->getContext());
  if (longestLine == nextLine) longestLine = 0L;

  buffer->changeLine(a->cursor.y);
  buffer->removeLine(a->cursor.y+1);

  tagLine(a->cursor.y);
  delLine(a->cursor.y + 1);

  a->action = KateAction::newLine;
}

void KateDocument::doInsLine(KateAction *a) {

  buffer->insertLine(a->cursor.y, new TextLine());

  insLine(a->cursor.y);

  a->action = KateAction::killLine;
}

void KateDocument::doKillLine(KateAction *a) {
  TextLine::Ptr textLine = getTextLine(a->cursor.y);
  if (longestLine == textLine) longestLine = 0L;

  buffer->removeLine(a->cursor.y);

  delLine(a->cursor.y);
  tagLine(a->cursor.y);

  a->action = KateAction::insLine;
}

void KateDocument::newUndo() {
  KTextEditor::View *view;
  int state;

  state = 0;
  if (currentUndo > 0) state |= 1;
  if (currentUndo < (int) undoList.count()) state |= 2;
  undoState = state;
  for (view = m_views.first(); view != 0L; view = m_views.next() ) {
    emit static_cast<KateView *>( view )->newUndo();
  }
}

void KateDocument::recordStart(VConfig &c, int newUndoType) {
  recordStart(c.view, c.cursor, c.flags, newUndoType);
}

void KateDocument::recordStart(KateView *, PointStruc &cursor, int flags,
  int newUndoType, bool keepModal, bool mergeUndo) {

  KateActionGroup *g;

//  if (newUndoType == KateActionGroup::ugNone) {
    // only a bug would cause this
//why should someone do this? we can't prevent all programming errors :) (jochen whilhelmy)
//    debug("KateDocument::recordStart() called with no undo group type!");
//    return;
//  }

  if (!keepModal) setPseudoModal(0L);

  //i optimized the group undo stuff a bit (jochen wilhelmy)
  //  recordReset() is not needed any more
  g = undoList.getLast();
  if (g != 0L && ((undoCount < 1024 && flags & KateView::cfGroupUndo
    && g->end.x == cursor.x && g->end.y == cursor.y) || mergeUndo)) {

    //undo grouping : same actions are put into one undo step
    //precondition : new action starts where old stops or mergeUndo flag
    if (g->undoType == newUndoType
      || (g->undoType == KateActionGroup::ugInsChar
        && newUndoType == KateActionGroup::ugInsLine)
      || (g->undoType == KateActionGroup::ugDelChar
        && newUndoType == KateActionGroup::ugDelLine)) {

      undoCount++;
      if (g->undoType != newUndoType) undoCount = 0xffffff;
      return;
    }
  }
  undoCount = 0;
/*
  if (undoView != view) {
    // always kill the current undo group if the editing view changes
    recordReset();
    undoType = newUndoType;
  } else if (newUndoType == undoType) {
printf("bla!!!\n");
    // same as current type, keep using it
    return;
  } else if  ( (undoType == KateActionGroup::ugInsChar && newUndoType == KateActionGroup::ugInsLine) ||
               (undoType == KateActionGroup::ugDelChar && newUndoType == KateActionGroup::ugDelLine) ) {
    // some type combinations can run together...
    undoType += 1000;
    return;
  } else {
    recordReset();
    undoType = newUndoType;
  }

  undoView = view;
*/
  while ((int) undoList.count() > currentUndo) undoList.removeLast();
  while ((int) undoList.count() > undoSteps) {
    undoList.removeFirst();
    currentUndo--;
  }

  g = new KateActionGroup(cursor, newUndoType);
  undoList.append(g);
//  currentUndo++;

  tagEnd = 0;
  tagStart = 0xffffff;
}

void KateDocument::recordAction(KateAction::Action action, PointStruc &cursor) {
  KateAction *a;

  a = new KateAction(action, cursor);
  doAction(a);
  undoList.getLast()->insertAction(a);
}

void KateDocument::recordInsert(VConfig &c, const QString &text) {
  recordReplace(c, 0, text);
}

void KateDocument::recordReplace(VConfig &c, int len, const QString &text) {
  if (c.cursor.x > 0 && !(c.flags & KateView::cfSpaceIndent)) {
    TextLine::Ptr textLine = getTextLine(c.cursor.y);
    if (textLine->length() == 0) {
      QString s = tabString(c.cursor.x, tabChars);
      int len = s.length();
      s += text;
      c.cursor.x = 0;
      recordReplace(c.cursor, len, s);
      c.cursor.x = len;
      return;
    }
  }
  recordReplace(c.cursor, len, text);
}

void KateDocument::recordInsert(PointStruc &cursor, const QString &text) {
  recordReplace(cursor, 0, text);
}

void KateDocument::recordDelete(PointStruc &cursor, int len) {
  recordReplace(cursor, len, QString::null);
}

void KateDocument::recordReplace(PointStruc &cursor, int len, const QString &text) {
  KateAction *a;
  TextLine::Ptr textLine;
  int l;

  if (len == 0 && text.isEmpty()) return;

  //try to append to last replace action
  a = undoList.getLast()->action;
  if (a == 0L || a->action != KateAction::replace
    || a->cursor.x + a->len != cursor.x || a->cursor.y != cursor.y) {

//if (a != 0L) printf("new %d %d\n", a->cursor.x + a->len, cursor.x);
    a = new KateAction(KateAction::replace, cursor);
    undoList.getLast()->insertAction(a);
  }

  //replace
  textLine = getTextLine(cursor.y);
  l = textLine->length() - cursor.x;
  if (l > len) l = len;
  a->text.insert(a->text.length(), &textLine->getText()[cursor.x], (l < 0) ? 0 : l);
  textLine->replace(cursor.x, len, text.unicode(), text.length());
  a->len += text.length();

  buffer->changeLine(a->cursor.y);
  updateMaxLength(textLine);
  tagLine(a->cursor.y);
}

void KateDocument::recordEnd(VConfig &c) {
  recordEnd(c.view, c.cursor, c.flags);
}

void KateDocument::recordEnd(KateView *view, PointStruc &cursor, int flags) {
  KateActionGroup *g;

  // clear selection if option "persistent selections" is off
//  if (!(flags & cfPersistent)) deselectAll();

  g = undoList.getLast();
  if (g->action == 0L) {
    // no action has been done: remove empty undo record
    undoList.removeLast();
    return;
  }
  // store end cursor position for redo
  g->end = cursor;
  currentUndo = undoList.count();

  if (tagStart <= tagEnd) {
    optimizeSelection();
    updateLines(tagStart, tagEnd, flags, cursor.y);
    setModified(true);
  }

  view->updateCursor(cursor, flags);

//  newUndo();
/*
  undoCount++;
  // we limit the number of individual undo operations for sanity - is 1K reasonable?
  // this is also where we handle non-group undo preference
  // if the undo type is singlular, we always finish it now
  if ( undoType == KateActionGroup::ugPaste ||
       undoType == KateActionGroup::ugDelBlock ||
       undoType > 1000 ||
       undoCount > 1024 || !(flags & cfGroupUndo) ) {
printf("recordend %d %d\n", undoType, undoCount);
    recordReset();
  }
*/

  // this should keep the flood of signals down a little...
  if (undoCount == 0) newUndo();
  emit textChanged();
}
/*
void KateDocument::recordReset()
{
  if (pseudoModal)
    return;

  // forces the next call of recordStart() to begin a new undo group
  // not used in normal editing, but used by markFound(), etc.
  undoType = KateActionGroup::ugNone;
  undoCount = 0;
  undoView = NULL;
  undoReported = false;
printf("recordreset\n");
}
*/

/*
void KateDocument::recordDel(PointStruc &cursor, TextLine::Ptr &textLine, int l) {
  int len;

  len = textLine->length() - cursor.x;
  if (len > l) len = l;
  if (len > 0) {
    insertUndo(new KateAction(KateAction::replace,cursor,&textLine->getText()[cursor.x],len));
  }
}
*/


void KateDocument::doActionGroup(KateActionGroup *g, int flags, bool undo) {
  KateAction *a, *next;

  setPseudoModal(0L);
  if (!(flags & KateView::cfPersistent)) deselectAll();
  tagEnd = 0;
  tagStart = 0xffffff;

  a = g->action;
  g->action = 0L;
  while (a) {
    doAction(a);
    next = a->next;
    g->insertAction(a);
    a = next;
  }
  optimizeSelection();
  if (tagStart <= tagEnd) updateLines(tagStart, tagEnd, flags);

  // the undo/redo functions set undo to true, all others should leave it
  // alone (default)
  if (!undo) {
    setModified(true);
    newUndo();
  }
}

int KateDocument::nextUndoType()
{
  KateActionGroup *g;

  if (currentUndo <= 0) return KateActionGroup::ugNone;
  g = undoList.at(currentUndo - 1);
  return g->undoType;
}

int KateDocument::nextRedoType()
{
  KateActionGroup *g;

  if (currentUndo >= (int) undoList.count()) return KateActionGroup::ugNone;
  g = undoList.at(currentUndo);
//  if (!g) return KateActionGroup::ugNone;
  return g->undoType;
}

void KateDocument::undoTypeList(QValueList<int> &lst)
{
  lst.clear();
  for (int i = currentUndo-1; i>=0 ;i--)
    lst.append(undoList.at(i)->undoType);
}

void KateDocument::redoTypeList(QValueList<int> &lst)
{
  lst.clear();
  for (int i = currentUndo+1; i<(int)undoList.count(); i++)
    lst.append(undoList.at(i)->undoType);
}

void KateDocument::undo(VConfig &c, int count) {
  KateActionGroup *g = 0L;
  int num;
  bool needUpdate = false; // don't update the cursor until completely done

  if (count <= 0) return;

  for (num = 0 ; num < count ; num++) {
    if (currentUndo <= 0) break;
    currentUndo--;
    g = undoList.at(currentUndo);
    doActionGroup(g, c.flags, true); // do not setModified() or newUndo()
    needUpdate = true;

//    if (num == 0) recordReset();
  }

  if (needUpdate) {
    // since we told doActionGroup() not to do this stuff, we need to do it now
    c.view->updateCursor(g->start);
    setModified(true);
    newUndo();
  }
}

void KateDocument::redo(VConfig &c, int count) {
  KateActionGroup *g = 0L;
  int num;
  bool needUpdate = false; // don't update the cursor until completely done

  if (count <= 0) return;

  for (num = 0 ; num < count ; num++) {
    if (currentUndo+1 > (int)undoList.count()) break;
    g = undoList.at(currentUndo);
    currentUndo++;
    doActionGroup(g, c.flags, true); // do not setModified() or newUndo()
    needUpdate = true;

//    if (num == 0) recordReset();
  }

  if (needUpdate) {
    // since we told doActionGroup() not to do this stuff, we need to do it now
    c.view->updateCursor(g->end);
    setModified(true);
    newUndo();
  }
}

void KateDocument::clearRedo() {
  // disable redos
  // this was added as an assist to the spell checker
  bool deleted = false;

  while ((int) undoList.count() > currentUndo) {
    deleted = true;
    undoList.removeLast();
  }

  if (deleted) newUndo();
}

void KateDocument::setUndoSteps(int steps) {
  if (steps < 5) steps = 5;
  undoSteps = steps;
}

void KateDocument::setPseudoModal(QWidget *w) {
//  QWidget *old = pseudoModal;

  // (glenebob)
  // this is a temporary hack to make the spell checker work a little
  // better - as kspell progresses, this sort of thing should become
  // obsolete or worked around more cleanly
  // this is relied upon *only* by the spell-check code
  if (pseudoModal && pseudoModal != (QWidget*)1L)
    delete pseudoModal;

//  pseudoModal = 0L;
//  if (old || w) recordReset();

  pseudoModal = w;
}


void KateDocument::newBracketMark(PointStruc &cursor, BracketMark &bm)
{
  TextLine::Ptr textLine;
  int x, line, count, attr;
  QChar bracket, opposite, ch;
  Attribute *a;

  bm.eXPos = -1; //mark bracked mark as invalid
  x = cursor.x -1; // -1 to look at left side of cursor
  if (x < 0) return;
  line = cursor.y; //current line
  count = 0; //bracket counter for nested brackets

  textLine = getTextLine(line);
  if (!textLine) return;

  bracket = textLine->getChar(x);
  attr = textLine->getAttr(x);

  if (bracket == '(' || bracket == '[' || bracket == '{')
  {
    //get opposite bracket
    opposite = ')';
    if (bracket == '[') opposite = ']';
    if (bracket == '{') opposite = '}';
    //get attribute of bracket (opposite bracket must have the same attribute)
    x++;
    while (line - cursor.y < 40) {
      //go to next line on end of line
      while (x >= (int) textLine->length()) {
        line++;
        if (line > lastLine()) return;
        textLine = getTextLine(line);
        x = 0;
      }
      if (textLine->getAttr(x) == attr) {
        //try to find opposite bracked
        ch = textLine->getChar(x);
        if (ch == bracket) count++; //same bracket : increase counter
        if (ch == opposite) {
          count--;
          if (count < 0) goto found;
        }
      }
      x++;
    }
  }
  else if (bracket == ')' || bracket == ']' || bracket == '}')
  {
    opposite = '(';
    if (bracket == ']') opposite = '[';
    if (bracket == '}') opposite = '{';
    x--;
    while (cursor.y - line < 20) {

      while (x < 0) {
        line--;
        if (line < 0) return;
        textLine = getTextLine(line);
        x = textLine->length() -1;
      }
      if (textLine->getAttr(x) == attr) {
        ch = textLine->getChar(x);
        if (ch == bracket) count++;
        if (ch == opposite) {
          count--;
          if (count < 0) goto found;
        }
      }
      x--;
    }
  }
  return;

found:
  //cursor position of opposite bracket
  bm.cursor.x = x;
  bm.cursor.y = line;
  //x position (start and end) of related bracket
  bm.sXPos = textWidth(textLine, x);
  a = &m_attribs[attr];

   if (a->bold && a->italic)
      bm.eXPos = bm.sXPos + myFontMetricsBI.width(bracket);
    else if (a->bold)
      bm.eXPos = bm.sXPos + myFontMetricsBold.width(bracket);
    else if (a->italic)
      bm.eXPos = bm.sXPos + myFontMetricsItalic.width(bracket);
    else
      bm.eXPos = bm.sXPos + myFontMetrics.width(bracket);
}

void KateDocument::clipboardChanged() { //slot
//#if defined(_WS_X11_)
  if (m_singleSelection) {
    disconnect(QApplication::clipboard(), SIGNAL(dataChanged()),
      this, SLOT(clipboardChanged()));
    deselectAll();
    updateViews();
  }
//#endif
}

void KateDocument::guiActivateEvent( KParts::GUIActivateEvent *ev )
{
  KParts::ReadWritePart::guiActivateEvent( ev );
  if ( ev->activated() )
    emit selectionChanged();
}

void KateDocument::setDocName (QString docName)
{
  myDocName = docName;
  emit nameChanged (this);
}

void KateDocument::setMTime()
{
    if (fileInfo && !fileInfo->fileName().isEmpty()) {
      fileInfo->refresh();
      mTime = fileInfo->lastModified();
    }
}

void KateDocument::isModOnHD(bool forceReload)
{
  if (fileInfo && !fileInfo->fileName().isEmpty()) {
    fileInfo->refresh();
    if (fileInfo->lastModified() > mTime) {
      if ( forceReload ||
           (KMessageBox::warningContinueCancel(0,
               (i18n("The file %1 has changed on disk.\nDo you want to reload it?\n\nIf you cancel you will lose these changes next time you save this file")).arg(url().filename()),
               i18n("File has changed on Disk"),
               i18n("Yes") ) == KMessageBox::Continue)
          )
        reloadFile();
      else
        setMTime();
    }
  }
}

void KateDocument::reloadFile()
{
  if (fileInfo && !fileInfo->fileName().isEmpty()) {
    KateDocument::openFile();
    setMTime();
  }
}

void KateDocument::slotModChanged()
{
  emit modStateChanged (this);
}

QList<Kate::Mark> KateDocument::marks ()
{
  QList<Kate::Mark> list;
  TextLine::Ptr line;

  for (int i=0; i < numLines(); i++)
  {
    line = getTextLine(i);
    if (line->mark() != 0)
    {
      Kate::Mark *mark=new Kate::Mark;
      mark->line = i;
      mark->type = line->mark();
      list.append (mark);
    }
  }

  return list;
}

void KateDocument::flush ()
{
  if (isReadOnly())
    return;

  m_url = KURL();
  fileInfo->setFile (QString());
  setMTime();

  clear();
  updateViews();

  emit fileNameChanged ();
}

void KateDocument::open (const QString &name)
{
  openURL (KURL (name));
}

void KateDocument::wrapText (uint col)
{
  int line = 0;
  int z = 0;

  while(true)
  {
    TextLine::Ptr l = getTextLine(line);

    if (l->length() > col)
    {
      TextLine::Ptr tl = new TextLine();
      buffer->insertLine(line+1,tl);
      const QChar *text = l->getText();

      for (z=col; z>0; z--)
      {
        if (z < 1) break;
        if (text[z].isSpace()) break;
      }

      if (z < 1) z=col;

      l->wrap (tl, z);
    }

    line++;
    if (line >= numLines()) break;
  };

  newDocGeometry=true;
  updateLines();
  updateViews();
}

void KateDocument::setWordWrap (bool on)
{
  if (on != myWordWrap && on)
    wrapText (myWordWrapAt);

  myWordWrap = on;
}

void KateDocument::setWordWrapAt (uint col)
{
  if (myWordWrapAt != col && myWordWrap)
    wrapText (myWordWrapAt);

  myWordWrapAt = col;
}

void KateDocument::applyWordWrap ()
{
  wrapText (myWordWrapAt);
}
