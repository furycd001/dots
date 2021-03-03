/***************************************************************************
                          kateview.cpp  -  description
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



#include "kateview.h"
#include "kateview.moc"

#include "../document/katedocument.h"
#include "../document/katecmd.h"
#include "../factory/katefactory.h"
#include "../document/katehighlight.h"
#include "kateviewdialog.h"
#include "../document/katedialogs.h"

#include <kurldrag.h>
#include <qfocusdata.h>
#include <kdebug.h>
#include <kprinter.h>
#include <kapp.h>
#include <qscrollbar.h>
#include <qiodevice.h>
#include <qpopupmenu.h>
#include <kpopupmenu.h>
#include <qkeycode.h>
#include <qintdict.h>
#include <klineeditdlg.h>
#include <qdropsite.h>
#include <qdragobject.h>
#include <kconfig.h>
#include <ksconfig.h>
#include <qfont.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qfileinfo.h>
#include <qfile.h>
#include <qevent.h>
#include <qdir.h>
#include <qvbox.h>
#include <qprintdialog.h>
#include <qpaintdevicemetrics.h>
#include <qdropsite.h>
#include <qdragobject.h>
#include <qiodevice.h>
#include <qbuffer.h>
#include <qfocusdata.h>
#include <kcursor.h>
#include <klocale.h>
#include <kglobal.h>
#include <kcharsets.h>
#include <kfiledialog.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <kstringhandler.h>
#include <kaction.h>
#include <kstdaction.h>
#include <kparts/event.h>
#include <kxmlgui.h>
#include <dcopclient.h>
#include <qregexp.h>
#include <kwin.h>
#include <kdialogbase.h>

#include "../document/katetextline.h"
#include "kateviewdialog.h"
#include "kateundohistory.h"

KateViewInternal::KateViewInternal(KateView *view, KateDocument *doc) : QWidget(view)
{
  waitForPreHighlight=-1;
  myView = view;
  myDoc = doc;

  iconBorderWidth  = 16;
  iconBorderHeight = 800;

  QWidget::setCursor(ibeamCursor);
  setBackgroundMode(NoBackground);
  KCursor::setAutoHideCursor( this, true, true );

  setFocusPolicy(StrongFocus);

  xScroll = new QScrollBar(QScrollBar::Horizontal,myView);
  yScroll = new QScrollBar(QScrollBar::Vertical,myView);
  connect(xScroll,SIGNAL(valueChanged(int)),SLOT(changeXPos(int)));
  connect(yScroll,SIGNAL(valueChanged(int)),SLOT(changeYPos(int)));
  connect(yScroll,SIGNAL(valueChanged(int)),myView,SIGNAL(scrollValueChanged(int)));
  connect( doc, SIGNAL (preHighlightChanged(long)),this,SLOT(slotPreHighlightUpdate(long)));

  xPos = 0;
  yPos = 0;

  scrollTimer = 0;

  cursor.x = 0;
  cursor.y = 0;
  cursorOn = false;
  cursorTimer = 0;
  cXPos = 0;
  cOldXPos = 0;

  startLine = 0;
  endLine = -1;

  exposeCursor = false;
  updateState = 0;
  numLines = 0;
  lineRanges = 0L;
  newXPos = -1;
  newYPos = -1;

  drawBuffer = new QPixmap ();
  drawBuffer->setOptimization (QPixmap::BestOptim);

  bm.sXPos = 0;
  bm.eXPos = -1;

  setAcceptDrops(true);
  dragInfo.state = diNone;
}


KateViewInternal::~KateViewInternal()
{
  delete [] lineRanges;
  delete drawBuffer;
}


void KateViewInternal::slotPreHighlightUpdate(long line)
{
  //kdDebug()<<QString("slotPreHighlightUpdate - Wait for: %1, line:  %2").arg(waitForPreHighlight).arg(line)<<endl;
  if (waitForPreHighlight!=-1)
    {
       if (line>=waitForPreHighlight)
         {
           waitForPreHighlight=-1;
           repaint();
         }
    }
}

void KateViewInternal::doCursorCommand(VConfig &c, int cmdNum) {

  switch (cmdNum) {
    case KateView::cmLeft:
      cursorLeft(c);
      break;
    case KateView::cmRight:
      cursorRight(c);
      break;
    case KateView::cmWordLeft:
      wordLeft(c);
      break;
    case KateView::cmWordRight:
      wordRight(c);
      break;
    case KateView::cmHome:
      home(c);
      break;
    case KateView::cmEnd:
      end(c);
      break;
    case KateView::cmUp:
      cursorUp(c);
      break;
    case KateView::cmDown:
      cursorDown(c);
      break;
    case KateView::cmScrollUp:
      scrollUp(c);
      break;
    case KateView::cmScrollDown:
      scrollDown(c);
      break;
    case KateView::cmTopOfView:
      topOfView(c);
      break;
    case KateView::cmBottomOfView:
      bottomOfView(c);
      break;
    case KateView::cmPageUp:
      pageUp(c);
      break;
    case KateView::cmPageDown:
      pageDown(c);
      break;
    case KateView::cmTop:
      top_home(c);
      break;
    case KateView::cmBottom:
      bottom_end(c);
      break;
  }
}

void KateViewInternal::doEditCommand(VConfig &c, int cmdNum) {

  switch (cmdNum) {
    case KateView::cmCopy:
      myDoc->copy(c.flags);
      return;
    case KateView::cmSelectAll:
      myDoc->selectAll();
      return;
    case KateView::cmDeselectAll:
      myDoc->deselectAll();
      return;
    case KateView::cmInvertSelection:
      myDoc->invertSelection();
      return;
  }
  if (myView->isReadOnly()) return;
  switch (cmdNum) {
    case KateView::cmReturn:
      if (c.flags & KateView::cfDelOnInput) myDoc->delMarkedText(c);
      myDoc->newLine(c);
      //emit returnPressed();
      //e->ignore();
      return;
    case KateView::cmDelete:
      if ((c.flags & KateView::cfDelOnInput) && myDoc->hasMarkedText())
        myDoc->delMarkedText(c);
      else myDoc->del(c);
      return;
    case KateView::cmBackspace:
      if ((c.flags & KateView::cfDelOnInput) && myDoc->hasMarkedText())
        myDoc->delMarkedText(c);
      else myDoc->backspace(c);
      return;
    case KateView::cmKillLine:
      myDoc->killLine(c);
      return;
    case KateView::cmCut:
      myDoc->cut(c);
      return;
    case KateView::cmPaste:
      if (c.flags & KateView::cfDelOnInput) myDoc->delMarkedText(c);
      myDoc->paste(c);
      return;
    case KateView::cmUndo:
      myDoc->undo(c);
      return;
    case KateView::cmRedo:
      myDoc->redo(c);
      return;
    case KateView::cmIndent:
      myDoc->indent(c);
      return;
    case KateView::cmUnindent:
      myDoc->unIndent(c);
      return;
    case KateView::cmCleanIndent:
      myDoc->cleanIndent(c);
      return;
    case KateView::cmComment:
      myDoc->comment(c);
      return;
    case KateView::cmUncomment:
      myDoc->unComment(c);
      return;
  }
}

void KateViewInternal::cursorLeft(VConfig &c) {

  cursor.x--;
  if (c.flags & KateView::cfWrapCursor && cursor.x < 0 && cursor.y > 0) {
    cursor.y--;
    cursor.x = myDoc->textLength(cursor.y);
  }
  cOldXPos = cXPos = myDoc->textWidth(cursor);
  changeState(c);
}

void KateViewInternal::cursorRight(VConfig &c) {

  if (c.flags & KateView::cfWrapCursor) {
    if (cursor.x >= myDoc->textLength(cursor.y)) {
      if (cursor.y == myDoc->lastLine()) return;
      cursor.y++;
      cursor.x = -1;
    }
  }
  cursor.x++;
  cOldXPos = cXPos = myDoc->textWidth(cursor);
  changeState(c);
}

void KateViewInternal::wordLeft(VConfig &c) {
  Highlight *highlight;

  highlight = myDoc->highlight();
  TextLine::Ptr textLine = myDoc->getTextLine(cursor.y);

  if (cursor.x > 0) {
    do {
      cursor.x--;
    } while (cursor.x > 0 && !highlight->isInWord(textLine->getChar(cursor.x)));
    while (cursor.x > 0 && highlight->isInWord(textLine->getChar(cursor.x -1)))
      cursor.x--;
  } else {
    if (cursor.y > 0) {
      cursor.y--;
      textLine = myDoc->getTextLine(cursor.y);
      cursor.x = textLine->length();
    }
  }

  cOldXPos = cXPos = myDoc->textWidth(cursor);
  changeState(c);
}

void KateViewInternal::wordRight(VConfig &c) {
  Highlight *highlight;
  int len;

  highlight = myDoc->highlight();
  TextLine::Ptr textLine = myDoc->getTextLine(cursor.y);
  len = textLine->length();

  if (cursor.x < len) {
    do {
      cursor.x++;
    } while (cursor.x < len && highlight->isInWord(textLine->getChar(cursor.x)));
    while (cursor.x < len && !highlight->isInWord(textLine->getChar(cursor.x)))
      cursor.x++;
  } else {
    if (cursor.y < myDoc->lastLine()) {
      cursor.y++;
      textLine = myDoc->getTextLine(cursor.y);
      cursor.x = 0;
    }
  }

  cOldXPos = cXPos = myDoc->textWidth(cursor);
  changeState(c);
}

void KateViewInternal::home(VConfig &c) {
  int lc;

  lc = (c.flags & KateView::cfSmartHome) ? myDoc->getTextLine(cursor.y)->firstChar() : 0;
  if (lc <= 0 || cursor.x == lc) {
    cursor.x = 0;
    cOldXPos = cXPos = 0;
  } else {
    cursor.x = lc;
    cOldXPos = cXPos = myDoc->textWidth(cursor);
  }

  changeState(c);
}

void KateViewInternal::end(VConfig &c) {
  cursor.x = myDoc->textLength(cursor.y);
  cOldXPos = cXPos = myDoc->textWidth(cursor);
  changeState(c);
}


void KateViewInternal::cursorUp(VConfig &c) {

  cursor.y--;
  cXPos = myDoc->textWidth(c.flags & KateView::cfWrapCursor,cursor,cOldXPos);
  changeState(c);
}


void KateViewInternal::cursorDown(VConfig &c) {
  int x;

  if (cursor.y == myDoc->lastLine()) {
    x = myDoc->textLength(cursor.y);
    if (cursor.x >= x) return;
    cursor.x = x;
    cXPos = myDoc->textWidth(cursor);
  } else {
    cursor.y++;
    cXPos = myDoc->textWidth(c.flags & KateView::cfWrapCursor, cursor, cOldXPos);
  }
  changeState(c);
}

void KateViewInternal::scrollUp(VConfig &c) {

  if (! yPos) return;

  newYPos = yPos - myDoc->fontHeight;
  if (cursor.y == (yPos + height())/myDoc->fontHeight -1) {
    cursor.y--;
    cXPos = myDoc->textWidth(c.flags & KateView::cfWrapCursor,cursor,cOldXPos);

    changeState(c);
  }
}

void KateViewInternal::scrollDown(VConfig &c) {

  if (endLine >= myDoc->lastLine()) return;

  newYPos = yPos + myDoc->fontHeight;
  if (cursor.y == (yPos + myDoc->fontHeight -1)/myDoc->fontHeight) {
    cursor.y++;
    cXPos = myDoc->textWidth(c.flags & KateView::cfWrapCursor,cursor,cOldXPos);
    changeState(c);
  }
}

void KateViewInternal::topOfView(VConfig &c) {

  cursor.y = (yPos + myDoc->fontHeight -1)/myDoc->fontHeight;
  cursor.x = 0;
  cOldXPos = cXPos = 0;
  changeState(c);
}

void KateViewInternal::bottomOfView(VConfig &c) {

  cursor.y = (yPos + height())/myDoc->fontHeight -1;
  if (cursor.y < 0) cursor.y = 0;
  if (cursor.y > myDoc->lastLine()) cursor.y = myDoc->lastLine();
  cursor.x = 0;
  cOldXPos = cXPos = 0;
  changeState(c);
}

void KateViewInternal::pageUp(VConfig &c) {
  int lines = (endLine - startLine - 1);

  if (lines <= 0) lines = 1;

  if (!(c.flags & KateView::cfPageUDMovesCursor) && yPos > 0) {
    newYPos = yPos - lines * myDoc->fontHeight;
    if (newYPos < 0) newYPos = 0;
  }
  cursor.y -= lines;
  cXPos = myDoc->textWidth(c.flags & KateView::cfWrapCursor, cursor, cOldXPos);
  changeState(c);
//  cursorPageUp(c);
}

void KateViewInternal::pageDown(VConfig &c) {

  int lines = (endLine - startLine - 1);

  if (!(c.flags & KateView::cfPageUDMovesCursor) && endLine < myDoc->lastLine()) {
    if (lines < myDoc->lastLine() - endLine)
      newYPos = yPos + lines * myDoc->fontHeight;
    else
      newYPos = yPos + (myDoc->lastLine() - endLine) * myDoc->fontHeight;
  }
  cursor.y += lines;
  cXPos = myDoc->textWidth(c.flags & KateView::cfWrapCursor,cursor,cOldXPos);
  changeState(c);
//  cursorPageDown(c);
}

// go to the top, same X position
void KateViewInternal::top(VConfig &c) {

//  cursor.x = 0;
  cursor.y = 0;
  cXPos = myDoc->textWidth(c.flags & KateView::cfWrapCursor,cursor,cOldXPos);
//  cOldXPos = cXPos = 0;
  changeState(c);
}

// go to the bottom, same X position
void KateViewInternal::bottom(VConfig &c) {

//  cursor.x = 0;
  cursor.y = myDoc->lastLine();
  cXPos = myDoc->textWidth(c.flags & KateView::cfWrapCursor,cursor,cOldXPos);
//  cOldXPos = cXPos = 0;
  changeState(c);
}

// go to the top left corner
void KateViewInternal::top_home(VConfig &c)
{
  cursor.y = 0;
  cursor.x = 0;
  cOldXPos = cXPos = 0;
  changeState(c);
}

// go to the bottom right corner
void KateViewInternal::bottom_end(VConfig &c) {

  cursor.y = myDoc->lastLine();
  cursor.x = myDoc->textLength(cursor.y);
  cOldXPos = cXPos = myDoc->textWidth(cursor);
  changeState(c);
}


void KateViewInternal::changeXPos(int p) {
  int dx;

  dx = xPos - p;
  xPos = p;
  if (QABS(dx) < width()) scroll(dx, 0); else update();
}

void KateViewInternal::changeYPos(int p) {
  int dy;

  dy = yPos - p;
  yPos = p;
  clearDirtyCache(height());

  if (QABS(dy) < height())
  {
    scroll(0, dy);
    leftBorder->scroll(0, dy);
  }
  else
    update();
}


void KateViewInternal::getVConfig(VConfig &c) {

  c.view = myView;
  c.cursor = cursor;
  c.cXPos = cXPos;
  c.flags = myView->configFlags;
}

void KateViewInternal::changeState(VConfig &c) {
  /*
   * we need to be sure to kill the selection on an attempted cursor
   * movement even if the cursor doesn't physically move,
   * but we need to be careful not to do some other things in this case,
   * like we don't want to expose the cursor
   */

//  if (cursor.x == c.cursor.x && cursor.y == c.cursor.y) return;
  bool nullMove = (cursor.x == c.cursor.x && cursor.y == c.cursor.y);

//  if (cursor.y != c.cursor.y || c.flags & KateView::cfMark) myDoc->recordReset();

  if (! nullMove) {

    exposeCursor = true;

    // mark old position of cursor as dirty
    if (cursorOn) {
      tagLines(c.cursor.y, c.cursor.y, c.cXPos -2, c.cXPos +3);
      cursorOn = false;
    }

    // mark old bracket mark position as dirty
    if (bm.sXPos < bm.eXPos) {
      tagLines(bm.cursor.y, bm.cursor.y, bm.sXPos, bm.eXPos);
    }
    // make new bracket mark
    myDoc->newBracketMark(cursor, bm);

    // remove trailing spaces when leaving a line
    if (c.flags & KateView::cfRemoveSpaces && cursor.y != c.cursor.y) {
      TextLine::Ptr textLine = myDoc->getTextLine(c.cursor.y);
      int newLen = textLine->lastChar();
      if (newLen != textLine->length()) {
        textLine->truncate(newLen);
        // if some spaces are removed, tag the line as dirty
        myDoc->tagLines(c.cursor.y, c.cursor.y);
      }
    }
  }

  if (c.flags & KateView::cfMark) {
    if (! nullMove)
      myDoc->selectTo(c, cursor, cXPos);
  } else {
    if (!(c.flags & KateView::cfPersistent))
      myDoc->deselectAll();
  }
}

void KateViewInternal::insLine(int line) {

  if (line <= cursor.y) {
    cursor.y++;
  }
  if (line < startLine) {
    startLine++;
    endLine++;
    yPos += myDoc->fontHeight;
  } else if (line <= endLine) {
    tagAll();
  }
}

void KateViewInternal::delLine(int line) {

  if (line <= cursor.y && cursor.y > 0) {
    cursor.y--;
  }
  if (line < startLine) {
    startLine--;
    endLine--;
    yPos -= myDoc->fontHeight;
  } else if (line <= endLine) {
    tagAll();
  }
}

void KateViewInternal::updateCursor() {
  cOldXPos = cXPos = myDoc->textWidth(cursor);
}


void KateViewInternal::updateCursor(PointStruc &newCursor) {
  updateCursor(newCursor, myView->config());
}

void KateViewInternal::updateCursor(PointStruc &newCursor, int flags) {

  if (!(flags & KateView::cfPersistent)) myDoc->deselectAll();

  exposeCursor = true;
  if (cursorOn) {
    tagLines(cursor.y, cursor.y, cXPos -2, cXPos +3);
    cursorOn = false;
  }

  if (bm.sXPos < bm.eXPos) {
    tagLines(bm.cursor.y, bm.cursor.y, bm.sXPos, bm.eXPos);
  }
  myDoc->newBracketMark(newCursor, bm);

  cursor = newCursor;
  cOldXPos = cXPos = myDoc->textWidth(cursor);
}

// init the line dirty cache
void KateViewInternal::clearDirtyCache(int height) {
  int lines, z;

  // calc start and end line of visible part
  startLine = yPos/myDoc->fontHeight;
  endLine = (yPos + height -1)/myDoc->fontHeight;

  updateState = 0;

  lines = endLine - startLine +1;
  if (lines > numLines) { // resize the dirty cache
    numLines = lines*2;
    delete [] lineRanges;
    lineRanges = new LineRange[numLines];
  }

  for (z = 0; z < lines; z++) { // clear all lines
    lineRanges[z].start = 0xffffff;
    lineRanges[z].end = -2;
  }
  newXPos = newYPos = -1;
}

void KateViewInternal::tagLines(int start, int end, int x1, int x2) {
  LineRange *r;
  int z;

  start -= startLine;
  if (start < 0) start = 0;
  end -= startLine;
  if (end > endLine - startLine) end = endLine - startLine;

  if (x1 <= 0) x1 = -2;
  if (x1 < xPos-2) x1 = xPos-2;
  if (x2 > width() + xPos-2) x2 = width() + xPos-2;
  if (x1 >= x2) return;

  r = &lineRanges[start];
  for (z = start; z <= end; z++) {
    if (x1 < r->start) r->start = x1;
    if (x2 > r->end) r->end = x2;
    r++;
    updateState |= 1;
  }
}

void KateViewInternal::tagAll() {
  updateState = 3;
}

void KateViewInternal::setPos(int x, int y) {
  newXPos = x;
  newYPos = y;
}

void KateViewInternal::center() {
  newXPos = 0;
  newYPos = cursor.y*myDoc->fontHeight - height()/2;
  if (newYPos < 0) newYPos = 0;
}

void KateViewInternal::updateView(int flags) {
  int fontHeight;
  int oldXPos, oldYPos;
  int w, h;
  int z;
  bool b;
  int xMax, yMax;
  int cYPos;
  int cXPosMin, cXPosMax, cYPosMin, cYPosMax;
  int dx, dy;
  int pageScroll;
  int scrollbarWidth = style().scrollBarExtent().width();

//debug("upView %d %d %d %d %d", exposeCursor, updateState, flags, newXPos, newYPos);
  if (exposeCursor || flags & KateView::ufDocGeometry) {
    emit myView->newCurPos();
  } else {
    if (updateState == 0 && newXPos < 0 && newYPos < 0) return;
  }

  if (cursorTimer) {
    killTimer(cursorTimer);
    cursorTimer = startTimer(KApplication::cursorFlashTime() / 2);
    cursorOn = true;
  }

  oldXPos = xPos;
  oldYPos = yPos;
/*  if (flags & ufPos) {
    xPos = newXPos;
    yPos = newYPos;
    exposeCursor = true;
  }*/
  if (newXPos >= 0) xPos = newXPos;
  if (newYPos >= 0) yPos = newYPos;

  fontHeight = myDoc->fontHeight;
  cYPos = cursor.y*fontHeight;

  z = 0;
  do {
    w = myView->width() - 4;
    h = myView->height() - 4;

    xMax = myDoc->textWidth() - w;
    b = (xPos > 0 || xMax > 0);
    if (b) h -= scrollbarWidth;
    yMax = myDoc->textHeight() - h;
    if (yPos > 0 || yMax > 0) {
      w -= scrollbarWidth;
      xMax += scrollbarWidth;
      if (!b && xMax > 0) {
        h -= scrollbarWidth;
        yMax += scrollbarWidth;
      }
    }

    if (!exposeCursor) break;
//    if (flags & KateView::ufNoScroll) break;
/*
    if (flags & KateView::ufCenter) {
      cXPosMin = xPos + w/3;
      cXPosMax = xPos + (w*2)/3;
      cYPosMin = yPos + h/3;
      cYPosMax = yPos + ((h - fontHeight)*2)/3;
    } else {*/
      cXPosMin = xPos + 4;
      cXPosMax = xPos + w - 8;
      cYPosMin = yPos;
      cYPosMax = yPos + (h - fontHeight);
//    }

    if (cXPos < cXPosMin) {
      xPos -= cXPosMin - cXPos;
    }
    if (xPos < 0) xPos = 0;
    if (cXPos > cXPosMax) {
      xPos += cXPos - cXPosMax;
    }
    if (cYPos < cYPosMin) {
      yPos -= cYPosMin - cYPos;
    }
    if (yPos < 0) yPos = 0;
    if (cYPos > cYPosMax) {
      yPos += cYPos - cYPosMax;
    }

    z++;
  } while (z < 2);

  if (xMax < xPos) xMax = xPos;
  if (yMax < yPos) yMax = yPos;

  if (xMax > 0) {
    pageScroll = w - (w % fontHeight) - fontHeight;
    if (pageScroll <= 0)
      pageScroll = fontHeight;

    xScroll->blockSignals(true);
    xScroll->setGeometry(2,h + 2,w,scrollbarWidth);
    xScroll->setRange(0,xMax);
    xScroll->setValue(xPos);
    xScroll->setSteps(fontHeight,pageScroll);
    xScroll->blockSignals(false);
    xScroll->show();
  } else xScroll->hide();

  if (yMax > 0) {
    pageScroll = h - (h % fontHeight) - fontHeight;
    if (pageScroll <= 0)
      pageScroll = fontHeight;

    yScroll->blockSignals(true);
    yScroll->setGeometry(w + 2,2,scrollbarWidth,h);
    yScroll->setRange(0,yMax);
    yScroll->setValue(yPos);
    yScroll->setSteps(fontHeight,pageScroll);
    yScroll->blockSignals(false);
    yScroll->show();
  } else yScroll->hide();

  if (w != width() || h != height()) {
    clearDirtyCache(h);
    resize(w,h);
  } else {
    dx = oldXPos - xPos;
    dy = oldYPos - yPos;

    b = updateState == 3;
    if (flags & KateView::ufUpdateOnScroll) {
      b |= dx || dy;
    } else {
      b |= QABS(dx)*3 > w*2 || QABS(dy)*3 > h*2;
    }

    if (b) {
      clearDirtyCache(h);
      update();
    } else {
      if (dy)
        leftBorder->scroll(0, dy);
      if (updateState > 0) paintTextLines(oldXPos, oldYPos);
      clearDirtyCache(h);

      if (dx || dy) {
        scroll(dx,dy);
//        kapp->syncX();
//        scroll2(dx - dx/2,dy - dy/2);
//      } else {
      }
      if (cursorOn) paintCursor();
      if (bm.eXPos > bm.sXPos) paintBracketMark();
    }
  }
  exposeCursor = false;
//  updateState = 0;
}


void KateViewInternal::paintTextLines(int xPos, int yPos) {
//  int xStart, xEnd;
  int line;//, z;
  int h;
  LineRange *r;

  if (!drawBuffer) return;
  if (drawBuffer->isNull()) return;

  QPainter paint;
  paint.begin(drawBuffer);

  h = myDoc->fontHeight;
  r = lineRanges;
  for (line = startLine; line <= endLine; line++) {
    if (r->start < r->end) {
//debug("painttextline %d %d %d", line, r->start, r->end);
      myDoc->paintTextLine(paint, line, r->start, r->end, myView->configFlags & KateView::cfShowTabs);
      bitBlt(this, r->start - (xPos-2), line*h - yPos, drawBuffer, 0, 0,
        r->end - r->start, h);
        leftBorder->paintLine(line);
    }
    r++;
  }

  paint.end();
}

void KateViewInternal::paintCursor() {
  int h, y, x;
  static int cx = 0, cy = 0, ch = 0;

  h = myDoc->fontHeight;
  y = h*cursor.y - yPos;
  x = cXPos - (xPos-2);

  if(myDoc->myFont != font()) setFont(myDoc->myFont);
  if(cx != x || cy != y || ch != h){
    cx = x;
    cy = y;
    ch = h;
    setMicroFocusHint(cx, cy, 0, ch - 2);
  }

  QPainter paint;
  if (cursorOn) {
    paint.begin(this);
    paint.setClipping(false);
    paint.setPen(myDoc->cursorCol(cursor.x,cursor.y));

    h += y - 1;
    paint.drawLine(x, y, x, h);

 paint.end();
  } else { if (drawBuffer && !drawBuffer->isNull()) {
    paint.begin(drawBuffer);
    myDoc->paintTextLine(paint, cursor.y, cXPos - 2, cXPos + 3, myView->configFlags & KateView::cfShowTabs);
    bitBlt(this,x - 2,y, drawBuffer, 0, 0, 5, h);
 paint.end(); }
  }

}

void KateViewInternal::paintBracketMark() {
  int y;

  y = myDoc->fontHeight*(bm.cursor.y +1) - yPos -1;

  QPainter paint;
  paint.begin(this);
  paint.setPen(myDoc->cursorCol(bm.cursor.x, bm.cursor.y));

  paint.drawLine(bm.sXPos - (xPos-2), y, bm.eXPos - (xPos-2) -1, y);
  paint.end();
}

void KateViewInternal::placeCursor(int x, int y, int flags) {
  VConfig c;

  getVConfig(c);
  c.flags |= flags;
  cursor.y = (yPos + y)/myDoc->fontHeight;
  cXPos = cOldXPos = myDoc->textWidth(c.flags & KateView::cfWrapCursor, cursor,xPos-2 + x);
  changeState(c);
}

// given physical coordinates, report whether the text there is selected
bool KateViewInternal::isTargetSelected(int x, int y) {

  y = (yPos + y) / myDoc->fontHeight;

  TextLine::Ptr line = myDoc->getTextLine(y);
  if (!line)
    return false;

  x = myDoc->textPos(line, x);

  return line->isSelected(x);
}

void KateViewInternal::focusInEvent(QFocusEvent *) {
//  debug("got focus %d",cursorTimer);

  if (!cursorTimer) {
    cursorTimer = startTimer(KApplication::cursorFlashTime() / 2);
    cursorOn = true;
    paintCursor();
  }
}

void KateViewInternal::focusOutEvent(QFocusEvent *) {
//  debug("lost focus %d", cursorTimer);

  if (cursorTimer) {
    killTimer(cursorTimer);
    cursorTimer = 0;
  }

  if (cursorOn) {
    cursorOn = false;
    paintCursor();
  }
}

void KateViewInternal::keyPressEvent(QKeyEvent *e) {
  VConfig c;
//  int ascii;

/*  if (e->state() & AltButton) {
    e->ignore();
    return;
  }*/
//  debug("ascii %i, key %i, state %i",e->ascii(), e->key(), e->state());

  getVConfig(c);
//  ascii = e->ascii();

  if (!myView->isReadOnly()) {
    if (c.flags & KateView::cfTabIndents && myDoc->hasMarkedText()) {
      if (e->key() == Qt::Key_Tab) {
        myDoc->indent(c);
        myDoc->updateViews();
        return;
      }
      if (e->key() == Qt::Key_Backtab) {
        myDoc->unIndent(c);
        myDoc->updateViews();
        return;
      }
    }
    if ( !(e->state() & ControlButton ) && myDoc->insertChars(c, e->text())) {
      myDoc->updateViews();
      e->accept();
      return;
    }
  }
  e->ignore();
}

void KateViewInternal::mousePressEvent(QMouseEvent *e) {

  if (e->button() == LeftButton) {

    if (isTargetSelected(e->x(), e->y())) {
      // we have a mousedown on selected text
      // we initialize the drag info thingy as pending from this position

      dragInfo.state = diPending;
      dragInfo.start.x = e->x();
      dragInfo.start.y = e->y();
    } else {
      // we have no reason to ever start a drag from here
      dragInfo.state = diNone;

      int flags;

      flags = 0;
      if (e->state() & ShiftButton) {
        flags |= KateView::cfMark;
        if (e->state() & ControlButton) flags |= KateView::cfMark | KateView::cfKeepSelection;
      }
      placeCursor(e->x(), e->y(), flags);
      scrollX = 0;
      scrollY = 0;
      if (!scrollTimer) scrollTimer = startTimer(50);
      myDoc->updateViews();
    }
  }
  if (e->button() == MidButton) {
    placeCursor(e->x(), e->y());
    if (! myView->isReadOnly())
      myView->paste();
  }
  if (myView->rmbMenu && e->button() == RightButton) {
    myView->rmbMenu->popup(mapToGlobal(e->pos()));
  }
  myView->mousePressEvent(e); // this doesn't do anything, does it?
  // it does :-), we need this for KDevelop, so please don't uncomment it again -Sandy
}

void KateViewInternal::mouseDoubleClickEvent(QMouseEvent *e) {

  if (e->button() == LeftButton) {
    VConfig c;
    getVConfig(c);
    myDoc->selectWord(c.cursor, c.flags);
    myDoc->updateViews();
  }
}

void KateViewInternal::mouseReleaseEvent(QMouseEvent *e) {

  if (e->button() == LeftButton) {
    if (dragInfo.state == diPending) {
      // we had a mouse down in selected area, but never started a drag
      // so now we kill the selection
      placeCursor(e->x(), e->y(), 0);
      myDoc->updateViews();
    } else if (dragInfo.state == diNone) {
      if (myView->config() & KateView::cfMouseAutoCopy) myView->copy();
      killTimer(scrollTimer);
      scrollTimer = 0;
    }
    dragInfo.state = diNone;
  }
}

void KateViewInternal::mouseMoveEvent(QMouseEvent *e) {

  if (e->state() & LeftButton) {
    int flags;
    int d;
    int x = e->x(),
        y = e->y();

    if (dragInfo.state == diPending) {
      // we had a mouse down, but haven't confirmed a drag yet
      // if the mouse has moved sufficiently, we will confirm

      if (x > dragInfo.start.x + 4 || x < dragInfo.start.x - 4 ||
          y > dragInfo.start.y + 4 || y < dragInfo.start.y - 4) {
        // we've left the drag square, we can start a real drag operation now
        doDrag();
      }
      return;
    } else if (dragInfo.state == diDragging) {
      // this isn't technically needed because mouseMoveEvent is suppressed during
      // Qt drag operations, replaced by dragMoveEvent
      return;
    }

    mouseX = e->x();
    mouseY = e->y();
    scrollX = 0;
    scrollY = 0;
    d = myDoc->fontHeight;
    if (mouseX < 0) {
      mouseX = 0;
      scrollX = -d;
    }
    if (mouseX > width()) {
      mouseX = width();
      scrollX = d;
    }
    if (mouseY < 0) {
      mouseY = 0;
      scrollY = -d;
    }
    if (mouseY > height()) {
      mouseY = height();
      scrollY = d;
    }
//debug("modifiers %d", ((KGuiCmdApp *) kapp)->getModifiers());
    flags = KateView::cfMark;
    if (e->state() & ControlButton) flags |= KateView::cfKeepSelection;
    placeCursor(mouseX, mouseY, flags);
    myDoc->updateViews(/*ufNoScroll*/);
  }
}



void KateViewInternal::wheelEvent( QWheelEvent *e )
{
  if( yScroll->isVisible() == true )
  {
    QApplication::sendEvent( yScroll, e );
  }
}



void KateViewInternal::paintEvent(QPaintEvent *e) {
  int xStart, xEnd;
  int h;
  int line, y, yEnd;

  QRect updateR = e->rect();

  if (!drawBuffer) return;
  if (drawBuffer->isNull()) return;

  QPainter paint;
  paint.begin(drawBuffer);

  xStart = xPos-2 + updateR.x();
  xEnd = xStart + updateR.width();

  h = myDoc->fontHeight;
  line = (yPos + updateR.y()) / h;
  y = line*h - yPos;
  yEnd = updateR.y() + updateR.height();
  waitForPreHighlight=myDoc->needPreHighlight(waitForPreHighlight=line+((long)(yEnd-y)/h)+5);

  while (y < yEnd)
  {
    TextLine *textLine;
    int ctxNum = 0;
    myDoc->paintTextLine(paint, line, xStart, xEnd, myView->configFlags & KateView::cfShowTabs);
    bitBlt(this, updateR.x(), y, drawBuffer, 0, 0, updateR.width(), h);
    leftBorder->paintLine(line);
    line++;
    y += h;
  }
  paint.end();

  if (cursorOn) paintCursor();
  if (bm.eXPos > bm.sXPos) paintBracketMark();
}

void KateViewInternal::resizeEvent(QResizeEvent *)
{
  drawBuffer->resize (width(), myDoc->fontHeight);
  leftBorder->resize(iconBorderWidth, height());
}

void KateViewInternal::timerEvent(QTimerEvent *e) {
  if (e->timerId() == cursorTimer) {
    cursorOn = !cursorOn;
    paintCursor();
  }
  if (e->timerId() == scrollTimer && (scrollX | scrollY)) {
    xScroll->setValue(xPos + scrollX);
    yScroll->setValue(yPos + scrollY);

    placeCursor(mouseX, mouseY, KateView::cfMark);
    myDoc->updateViews(/*ufNoScroll*/);
  }
}

/////////////////////////////////////
// Drag and drop handlers
//

// call this to start a drag from this view
void KateViewInternal::doDrag()
{
  dragInfo.state = diDragging;
  dragInfo.dragObject = new QTextDrag(myDoc->markedText(0), this);
  dragInfo.dragObject->dragCopy();
}

void KateViewInternal::dragEnterEvent( QDragEnterEvent *event )
{
  event->accept( (QTextDrag::canDecode(event) && ! myView->isReadOnly()) || QUriDrag::canDecode(event) );
}

void KateViewInternal::dropEvent( QDropEvent *event )
{
  if ( QUriDrag::canDecode(event) ) {

      emit dropEventPass(event);

  } else if ( QTextDrag::canDecode(event) && ! myView->isReadOnly() ) {

    QString   text;

    if (QTextDrag::decode(event, text)) {
      bool      priv, selected;

      // is the source our own document?
      priv = myDoc->ownedView((KateView*)(event->source()));
      // dropped on a text selection area?
      selected = isTargetSelected(event->pos().x(), event->pos().y());

      if (priv && selected) {
        // this is a drag that we started and dropped on our selection
        // ignore this case
        return;
      }

      VConfig c;
      PointStruc cursor;

      getVConfig(c);
      cursor = c.cursor;

      if (priv) {
        // this is one of mine (this document), not dropped on the selection
        if (event->action() == QDropEvent::Move) {
          myDoc->delMarkedText(c);
          getVConfig(c);
          cursor = c.cursor;
        } else {
        }
        placeCursor(event->pos().x(), event->pos().y());
        getVConfig(c);
        cursor = c.cursor;
      } else {
        // this did not come from this document
        if (! selected) {
          placeCursor(event->pos().x(), event->pos().y());
          getVConfig(c);
          cursor = c.cursor;
        }
      }
      myDoc->insert(c, text);
      cursor = c.cursor;

      updateCursor(cursor);
      myDoc->updateViews();
    }
  }
}

uint KateView::uniqueID = 0;

KateView::KateView(KateDocument *doc, QWidget *parent, const char * name) : Kate::View (doc, parent, name), DCOPObject( (QString("KateView%1-%2").arg(doc->docID()).arg(uniqueID)).latin1())
{
  setInstance( KateFactory::instance() );

  myViewID = uniqueID;
  uniqueID++;

  active = false;
  myIconBorder = false;

  myDoc = doc;
  myViewInternal = new KateViewInternal (this,doc);
  myViewInternal->move(2, 2);
  myViewInternal->leftBorder = new KateIconBorder(this, myViewInternal);
  myViewInternal->leftBorder->setGeometry(2, 2, myViewInternal->iconBorderWidth, myViewInternal->iconBorderHeight);
  myViewInternal->leftBorder->hide();

  doc->addView( this );

  connect(myViewInternal,SIGNAL(dropEventPass(QDropEvent *)),this,SLOT(dropEventPassEmited(QDropEvent *)));

  // some defaults
  configFlags = KateView::cfAutoIndent | KateView::cfBackspaceIndents
    | KateView::cfTabIndents | KateView::cfKeepIndentProfile
    | KateView::cfRemoveSpaces
    | KateView::cfDelOnInput | KateView::cfMouseAutoCopy | KateView::cfWrapCursor
    | KateView::cfGroupUndo | KateView::cfShowTabs | KateView::cfSmartHome;

  searchFlags = 0;
  replacePrompt = 0L;
  rmbMenu = 0L;

  //KSpell initial values
  kspell.kspell = 0;
  kspell.ksc = new KSpellConfig; //default KSpellConfig to start
  kspell.kspellon = FALSE;

  setFocusProxy( myViewInternal );
  myViewInternal->setFocus();
  resize(parent->width() -4, parent->height() -4);

  printer = new KPrinter();

  myViewInternal->installEventFilter( this );

  if (!doc->hasBrowserExtension())
  {
    setXMLFile( "katepartui.rc" );
  }
  else
  {
    (void)new KateBrowserExtension( myDoc, this );
    myDoc->setXMLFile( "katepartbrowserui.rc" );
  }

  setupActions();

  connect( this, SIGNAL( newStatus() ), this, SLOT( slotUpdate() ) );
  connect( this, SIGNAL( newUndo() ), this, SLOT( slotNewUndo() ) );
  connect( doc, SIGNAL( fileNameChanged() ), this, SLOT( slotFileStatusChanged() ) );
  connect( doc, SIGNAL( highlightChanged() ), this, SLOT( slotHighlightChanged() ) );

  if ( doc->hasBrowserExtension() )
  {
    connect( this, SIGNAL( dropEventPass(QDropEvent*) ), this, SLOT( slotDropEventPass(QDropEvent*) ) );
  }

  readConfig();
  setHighlight->setCurrentItem(getHl());
  slotUpdate();
}

KateView::~KateView()
{
  if (kspell.kspell)
  {
    kspell.kspell->setAutoDelete(true);
    kspell.kspell->cleanUp(); // need a way to wait for this to complete
  }
  delete kspell.ksc;

  if (myDoc && !myDoc->m_bSingleViewMode)
    myDoc->removeView( this );

  delete myViewInternal;

  delete printer;
}

void KateView::setupActions()
{
    KStdAction::close( this, SLOT(flush()), actionCollection(), "file_close" );

    KStdAction::save(this, SLOT(save()), actionCollection());

    // setup edit menu
    editUndo = KStdAction::undo(this, SLOT(undo()), actionCollection());
    editRedo = KStdAction::redo(this, SLOT(redo()), actionCollection());
    editUndoHist = new KAction(i18n("Undo/Redo &History..."), 0, this, SLOT(undoHistory()),
                               actionCollection(), "edit_undoHistory");
    KStdAction::cut(this, SLOT(cut()), actionCollection());
    KStdAction::copy(this, SLOT(copy()), actionCollection());
    KStdAction::paste(this, SLOT(paste()), actionCollection());

    if ( myDoc->hasBrowserExtension() )
    {
      KStdAction::saveAs(this, SLOT(saveAs()), myDoc->actionCollection());
      KStdAction::find(this, SLOT(find()), myDoc->actionCollection(), "find");
      KStdAction::findNext(this, SLOT(findAgain()), myDoc->actionCollection(), "find_again");
      KStdAction::findPrev(this, SLOT(findPrev()), myDoc->actionCollection(), "find_prev");
      KStdAction::gotoLine(this, SLOT(gotoLine()), myDoc->actionCollection(), "goto_line" );
      new KAction(i18n("&Configure Editor..."), 0, this, SLOT(configDialog()),myDoc->actionCollection(), "set_confdlg");
      setHighlight = new KSelectAction(i18n("&Highlight Mode"), 0, myDoc->actionCollection(), "set_highlight");
      KStdAction::selectAll(this, SLOT(selectAll()), myDoc->actionCollection(), "select_all");
      new KAction(i18n("&Deselect All"), 0, this, SLOT(deselectAll()),
                myDoc->actionCollection(), "unselect_all");
      new KAction(i18n("Invert &Selection"), 0, this, SLOT(invertSelection()),
                myDoc->actionCollection(), "invert_select");

      new KAction(i18n("Increase Font Sizes"), "viewmag+", 0, this, SLOT(slotIncFontSizes()),
                myDoc->actionCollection(), "incFontSizes");
      new KAction(i18n("Decrease Font Sizes"), "viewmag-", 0, this, SLOT(slotDecFontSizes()),
                myDoc->actionCollection(), "decFontSizes");
    }
    else
    {
      KStdAction::saveAs(this, SLOT(saveAs()), actionCollection());
      KStdAction::find(this, SLOT(find()), actionCollection());
      KStdAction::findNext(this, SLOT(findAgain()), actionCollection());
      KStdAction::findPrev(this, SLOT(findPrev()), actionCollection(), "edit_find_prev");
      KStdAction::gotoLine(this, SLOT(gotoLine()), actionCollection());
      new KAction(i18n("&Configure Editor..."), 0, this, SLOT(configDialog()),actionCollection(), "set_confdlg");
      setHighlight = new KSelectAction(i18n("&Highlight Mode"), 0, actionCollection(), "set_highlight");
      KStdAction::selectAll(this, SLOT(selectAll()), actionCollection());
      new KAction(i18n("&Deselect All"), 0, this, SLOT(deselectAll()),
                actionCollection(), "edit_deselectAll");
      new KAction(i18n("Invert &Selection"), 0, this, SLOT(invertSelection()),
                actionCollection(), "edit_invertSelection");

      new KAction(i18n("Increase Font Sizes"), "viewmag+", 0, this, SLOT(slotIncFontSizes()),
                actionCollection(), "incFontSizes");
      new KAction(i18n("Decrease Font Sizes"), "viewmag-", 0, this, SLOT(slotDecFontSizes()),
                actionCollection(), "decFontSizes");
    }

  new KAction(i18n("Apply Word Wrap"), 0, myDoc, SLOT(applyWordWrap()), actionCollection(), "edit_apply_wordwrap");

  KStdAction::replace(this, SLOT(replace()), actionCollection());

  new KAction(i18n("Editing Co&mmand"), Qt::CTRL+Qt::Key_M, this, SLOT(slotEditCommand()),
                                  actionCollection(), "edit_cmd");

    // setup bookmark menu
    bookmarkToggle = new KAction(i18n("Toggle &Bookmark"), Qt::CTRL+Qt::Key_B, this, SLOT(toggleBookmark()), actionCollection(), "edit_bookmarkToggle");
    bookmarkClear = new KAction(i18n("Clear Bookmarks"), 0, this, SLOT(clearBookmarks()), actionCollection(), "edit_bookmarksClear");

    // connect settings menu aboutToshow
    bookmarkMenu = new KActionMenu(i18n("&Bookmarks"), actionCollection(), "bookmarks");
    connect(bookmarkMenu->popupMenu(), SIGNAL(aboutToShow()), this, SLOT(bookmarkMenuAboutToShow()));

    new KToggleAction(i18n("Show &IconBorder"), Key_F6, this, SLOT(toggleIconBorder()), actionCollection(), "view_border");

    // setup Tools menu
    KStdAction::spelling(this, SLOT(spellcheck()), actionCollection());
    new KAction(i18n("&Indent"), "indent", Qt::CTRL+Qt::Key_I, this, SLOT(indent()),
                              actionCollection(), "tools_indent");
    new KAction(i18n("&Unindent"), "unindent", Qt::CTRL+Qt::Key_U, this, SLOT(unIndent()),
                                actionCollection(), "tools_unindent");
    new KAction(i18n("&Clean Indentation"), 0, this, SLOT(cleanIndent()),
                                   actionCollection(), "tools_cleanIndent");
    new KAction(i18n("C&omment"), CTRL+Qt::Key_NumberSign, this, SLOT(comment()),
                               actionCollection(), "tools_comment");
    new KAction(i18n("Unco&mment"), CTRL+SHIFT+Qt::Key_NumberSign, this, SLOT(uncomment()),
                                 actionCollection(), "tools_uncomment");

    setVerticalSelection = new KToggleAction(i18n("&Vertical Selection"), Key_F4, this, SLOT(toggleVertical()),
                                             actionCollection(), "set_verticalSelect");

    connect(setHighlight, SIGNAL(activated(int)), this, SLOT(setHl(int)));
    QStringList list;
    for (int z = 0; z < HlManager::self()->highlights(); z++)
        list.append(HlManager::self()->hlName(z));
    setHighlight->setItems(list);

    setEndOfLine = new KSelectAction(i18n("&End Of Line"), 0, actionCollection(), "set_eol");
    connect(setEndOfLine, SIGNAL(activated(int)), this, SLOT(setEol(int)));
    list.clear();
    list.append("&Unix");
    list.append("&Windows/Dos");
    list.append("&Macintosh");
    setEndOfLine->setItems(list);
}

void KateView::slotUpdate()
{
    int cfg = config();

    setVerticalSelection->setChecked(cfg & KateView::cfVerticalSelect);

    slotNewUndo();
}
void KateView::slotFileStatusChanged()
{
  int eol = getEol();
  eol = eol>=1 ? eol : 0;

    setEndOfLine->setCurrentItem(eol);
}
void KateView::slotNewUndo()
{
    int state = undoState();

    editUndoHist->setEnabled(state & 1 || state & 2);

    QString t = i18n("Und&o");   // it would be nicer to fetch the original string
    if (state & 1) {
        editUndo->setEnabled(true);
        t += ' ';
        t += i18n(undoTypeName(nextUndoType()));
    } else {
        editUndo->setEnabled(false);
    }
    editUndo->setText(t);

    t = i18n("Re&do");   // it would be nicer to fetch the original string
    if (state & 2) {
        editRedo->setEnabled(true);
        t += ' ';
        t += i18n(undoTypeName(nextRedoType()));
    } else {
        editRedo->setEnabled(false);
    }
    editRedo->setText(t);
}

void KateView::slotHighlightChanged()
{
    setHighlight->setCurrentItem(getHl());
}

void KateView::slotDropEventPass( QDropEvent * ev )
{
    KURL::List lstDragURLs;
    bool ok = KURLDrag::decode( ev, lstDragURLs );

    KParts::BrowserExtension * ext = KParts::BrowserExtension::childObject( doc() );
    if ( ok && ext )
        emit ext->openURLRequest( lstDragURLs.first() );
}

void KateView::keyPressEvent( QKeyEvent *ev )
{
    switch ( ev->key() )
    {
        case Key_Left:
            if ( ev->state() & ShiftButton )
            {
                if ( ev->state() & ControlButton )
                    shiftWordLeft();
                else
                    shiftCursorLeft();
            }
            else if ( ev->state() & ControlButton )
                wordLeft();
            else
                cursorLeft();
            break;
        case Key_Right:
            if ( ev->state() & ShiftButton )
            {
                if ( ev->state() & ControlButton )
                    shiftWordRight();
                else
                    shiftCursorRight();
            }
            else if ( ev->state() & ControlButton )
                wordRight();
            else
                cursorRight();
            break;
        case Key_Home:
            if ( ev->state() & ShiftButton )
            {
                if ( ev->state() & ControlButton )
                    shiftTop();
                else
                    shiftHome();
            }
            else if ( ev->state() & ControlButton )
                top();
            else
                home();
            break;
        case Key_End:
            if ( ev->state() & ShiftButton )
            {
                if ( ev->state() & ControlButton )
                    shiftBottom();
                else
                    shiftEnd();
            }
            else if ( ev->state() & ControlButton )
                bottom();
            else
                end();
            break;
        case Key_Up:
            if ( ev->state() & ShiftButton )
                shiftUp();
            else if ( ev->state() & ControlButton )
                scrollUp();
            else
                up();
            break;
        case Key_Down:
            if ( ev->state() & ShiftButton )
                shiftDown();
            else if ( ev->state() & ControlButton )
                scrollDown();
            else
                down();
            break;
        case Key_PageUp:
            if ( ev->state() & ShiftButton )
                shiftPageUp();
            else if ( ev->state() & ControlButton )
                topOfView();
            else
                pageUp();
            break;
        case Key_PageDown:
            if ( ev->state() & ShiftButton )
                shiftPageDown();
            else if ( ev->state() & ControlButton )
                bottomOfView();
            else
                pageDown();
            break;
        case Key_Return:
        case Key_Enter:
            keyReturn();
            break;
        case Key_Delete:
            if ( ev->state() & ControlButton )
            {
               VConfig c;
               shiftWordRight();
               myViewInternal->getVConfig(c);
               myDoc->delMarkedText(c);
               myViewInternal->update();
            }
            else keyDelete();
            break;
        case Key_Backspace:
            if ( ev->state() & ControlButton )
            {
               VConfig c;
               shiftWordLeft();
               myViewInternal->getVConfig(c);
               myDoc->delMarkedText(c);
               myViewInternal->update();
            }
            else backspace();
            break;
        case Key_Insert:
            toggleInsert();
            break;
        case Key_K:
            if ( ev->state() & ControlButton )
            {
                killLine();
                break;
            }
        default:
            KTextEditor::View::keyPressEvent( ev );
            return;
            break;
    }
    ev->accept();
}

void KateView::customEvent( QCustomEvent *ev )
{
    if ( KParts::GUIActivateEvent::test( ev ) && static_cast<KParts::GUIActivateEvent *>( ev )->activated() )
    {
        installPopup(static_cast<QPopupMenu *>(factory()->container("rb_popup", this) ) );
        return;
    }

    KTextEditor::View::customEvent( ev );
    return;
}

void KateView::setCursorPosition( int line, int col, bool /*mark*/ )
{
  setCursorPositionInternal( line, col );
}

void KateView::getCursorPosition( int *line, int *col )
{
  if ( line )
    *line = currentLine();

  if ( col )
    *col = currentColumn();
}


int KateView::currentLine() {
  return myViewInternal->cursor.y;
}

int KateView::currentColumn() {
  return myDoc->currentColumn(myViewInternal->cursor);
}

int KateView::currentCharNum() {
  return myViewInternal->cursor.x;
}

void KateView::setCursorPositionInternal(int line, int col) {
  PointStruc cursor;

  cursor.x = col;
  cursor.y = line;
  myViewInternal->updateCursor(cursor);
  myViewInternal->center();
//  myViewInternal->updateView(ufPos, 0, line*myDoc->fontHeight - height()/2);
//  myDoc->updateViews(myViewInternal); //uptade all other views except this one
  myDoc->updateViews();
}

int KateView::config() {
  int flags;

  flags = configFlags;
  if (myDoc->singleSelection()) flags |= KateView::cfSingleSelection;
  return flags;
}

void KateView::setConfig(int flags) {
  bool updateView;

  // cfSingleSelection is a doc-property
  myDoc->setSingleSelection(flags & KateView::cfSingleSelection);
  flags &= ~KateView::cfSingleSelection;

  if (flags != configFlags) {
    // update the view if visibility of tabs has changed
    updateView = (flags ^ configFlags) & KateView::cfShowTabs;
    configFlags = flags;
    emit newStatus();
    if (updateView) myViewInternal->update();
  }
}

int KateView::tabWidth() {
  return myDoc->tabChars;
}

void KateView::setTabWidth(int w) {
  myDoc->setTabWidth(w);
  myDoc->updateViews();
}

void KateView::setEncoding (QString e) {
  myDoc->setEncoding (e);
  myDoc->updateViews();
}

int KateView::undoSteps() {
  return myDoc->undoSteps;
}

void KateView::setUndoSteps(int s) {
  myDoc->setUndoSteps(s);
}

bool KateView::isReadOnly() {
  return myDoc->readOnly;
}

bool KateView::isModified() {
  return myDoc->modified;
}

void KateView::setReadOnly(bool m) {
  myDoc->setReadOnly(m);
}

void KateView::setModified(bool m) {
  myDoc->setModified(m);
}

bool KateView::isLastView() {
  return myDoc->isLastView(1);
}

KateDocument *KateView::doc() {
  return myDoc;
}

int KateView::undoState() {
  if (isReadOnly())
    return 0;
  else
    return myDoc->undoState;
}

int KateView::nextUndoType() {
  return myDoc->nextUndoType();
}

int KateView::nextRedoType() {
  return myDoc->nextRedoType();
}

void KateView::undoTypeList(QValueList<int> &lst)
{
  myDoc->undoTypeList(lst);
}

void KateView::redoTypeList(QValueList<int> &lst)
{
  myDoc->redoTypeList(lst);
}

const char * KateView::undoTypeName(int type) {
  return KateActionGroup::typeName(type);
}

QColor* KateView::getColors()
{
  return myDoc->colors;
}

void KateView::applyColors()
{
   myDoc->tagAll();
   myDoc->updateViews();
}

bool KateView::isOverwriteMode() const
{
  return ( configFlags & KateView::cfOvr );
}

void KateView::setOverwriteMode( bool b )
{
  if ( isOverwriteMode() && !b )
    setConfig( configFlags ^ KateView::cfOvr );
  else
    setConfig( configFlags | KateView::cfOvr );
}

void KateView::toggleInsert() {
  setConfig(configFlags ^ KateView::cfOvr);
}

void KateView::toggleVertical()
{
  setConfig(configFlags ^ KateView::cfVerticalSelect);
}


int KateView::numLines() {
  return myDoc->numLines();
}

QString KateView::text() {
  return myDoc->text();
}

QString KateView::currentTextLine() {
  TextLine::Ptr textLine = myDoc->getTextLine(myViewInternal->cursor.y);
  return QString(textLine->getText(), textLine->length());
}

QString KateView::textLine(int num) {
  TextLine::Ptr textLine = myDoc->getTextLine(num);
  return QString(textLine->getText(), textLine->length());
}

QString KateView::currentWord() {
  return myDoc->getWord(myViewInternal->cursor);
}

QString KateView::word(int x, int y) {
  PointStruc cursor;
  cursor.y = (myViewInternal->yPos + y)/myDoc->fontHeight;
  if (cursor.y < 0 || cursor.y > myDoc->lastLine()) return QString();
  cursor.x = myDoc->textPos(myDoc->getTextLine(cursor.y), myViewInternal->xPos-2 + x);
  return myDoc->getWord(cursor);
}

void KateView::setText(const QString &s) {
  myDoc->setText(s);
  myDoc->updateViews();
}

void KateView::insertText(const QString &s, bool /*mark*/) {
  VConfig c;
  myViewInternal->getVConfig(c);
  myDoc->insert(c, s);
  myDoc->updateViews();
}

bool KateView::hasMarkedText() {
  return myDoc->hasMarkedText();
}

QString KateView::markedText() {
  return myDoc->markedText(configFlags);
}

bool KateView::canDiscard() {
  int query;

  if (isModified()) {
    query = KMessageBox::warningYesNoCancel(this,
      i18n("The current Document has been modified.\nWould you like to save it?"));
    switch (query) {
      case KMessageBox::Yes: //yes
        if (save() == CANCEL) return false;
        if (isModified()) {
            query = KMessageBox::warningContinueCancel(this,
               i18n("Could not save the document.\nDiscard it and continue?"),
	       QString::null, i18n("&Discard"));
          if (query == KMessageBox::Cancel) return false;
        }
        break;
      case KMessageBox::Cancel: //cancel
        return false;
    }
  }
  return true;
}

void KateView::flush()
{
  if (canDiscard()) myDoc->flush();
}

KateView::fileResult KateView::save() {
  int query = KMessageBox::Yes;
  if (isModified()) {
    if (!myDoc->url().fileName().isEmpty() && ! isReadOnly()) {
      // If document is new but has a name, check if saving it would
      // overwrite a file that has been created since the new doc
      // was created:
      if( myDoc->isNewDoc() )
      {
        query = checkOverwrite( myDoc->url() );
        if( query == KMessageBox::Cancel )
          return CANCEL;
      }
      if( query == KMessageBox::Yes )
      myDoc->saveAs(myDoc->url());
      else  // Do not overwrite already existing document:
        return saveAs();
    } // New, unnamed document:
    else
      return saveAs();
  }
  return OK;
}

/*
 * Check if the given URL already exists. Currently used by both save() and saveAs()
 *
 * Asks the user for permission and returns the message box result and defaults to
 * KMessageBox::Yes in case of doubt
 */
int KateView::checkOverwrite( KURL u )
{
  int query = KMessageBox::Yes;

  if( u.isLocalFile() )
  {
    QFileInfo info;
    QString name( u.path() );
    info.setFile( name );
    if( info.exists() )
      query = KMessageBox::warningYesNoCancel( this,
        i18n( "A Document with this Name already exists.\nDo you want to overwrite it?" ) );
  }
  return query;
}

KateView::fileResult KateView::saveAs() {
  KURL url;
  int query;

  do {
    query = KMessageBox::Yes;

    url = KFileDialog::getSaveURL(myDoc->url().url(), QString::null,this);
    if (url.isEmpty()) return CANCEL;

    query = checkOverwrite( url );
  }
  while (query != KMessageBox::Yes);

  if( query == KMessageBox::Cancel )
    return CANCEL;

  myDoc->saveAs(url);
  return OK;
}

void KateView::doCursorCommand(int cmdNum) {
  VConfig c;
  myViewInternal->getVConfig(c);
  if (cmdNum & selectFlag) c.flags |= KateView::cfMark;
  if (cmdNum & multiSelectFlag) c.flags |= KateView::cfMark | KateView::cfKeepSelection;
  cmdNum &= ~(selectFlag | multiSelectFlag);
  myViewInternal->doCursorCommand(c, cmdNum);
  myDoc->updateViews();
}

void KateView::doEditCommand(int cmdNum) {
  VConfig c;
  myViewInternal->getVConfig(c);
  myViewInternal->doEditCommand(c, cmdNum);
  myDoc->updateViews();
}

void KateView::undoMultiple(int count) {
  if (isReadOnly())
    return;

  VConfig c;
  myViewInternal->getVConfig(c);
  myDoc->undo(c, count);
  myDoc->updateViews();
}

void KateView::redoMultiple(int count) {
  if (isReadOnly())
    return;

  VConfig c;
  myViewInternal->getVConfig(c);
  myDoc->redo(c, count);
  myDoc->updateViews();
}

void KateView::undoHistory()
{
  UndoHistory   *undoH;

  undoH = new UndoHistory(this, this, "UndoHistory", true);

  undoH->setCaption(i18n("Undo/Redo History"));

  connect(this,SIGNAL(newUndo()),undoH,SLOT(newUndo()));
  connect(undoH,SIGNAL(undo(int)),this,SLOT(undoMultiple(int)));
  connect(undoH,SIGNAL(redo(int)),this,SLOT(redoMultiple(int)));

  undoH->exec();

  delete undoH;
}

static void kwview_addToStrList(QStringList &list, const QString &str) {
  if (list.count() > 0) {
    if (list.first() == str) return;
    QStringList::Iterator it;
    it = list.find(str);
    if (*it != 0L) list.remove(it);
    if (list.count() >= 16) list.remove(list.fromLast());
  }
  list.prepend(str);
}

void KateView::find() {
  SearchDialog *searchDialog;

  if (!myDoc->hasMarkedText()) searchFlags &= ~KateView::sfSelected;

  searchDialog = new SearchDialog(this, myDoc->searchForList, myDoc->replaceWithList,
  searchFlags & ~KateView::sfReplace);

  // If the user has marked some text we use that otherwise
  // use the word under the cursor.
   QString str;
   if (myDoc->hasMarkedText())
     str = markedText();

   if (str.isEmpty())
     str = currentWord();

   if (!str.isEmpty())
   {
     str.replace(QRegExp("^\n"), "");
     int pos=str.find("\n");
     if (pos>-1)
       str=str.left(pos);
     searchDialog->setSearchText( str );
   }

  myViewInternal->focusOutEvent(0L);// QT bug ?
  if (searchDialog->exec() == QDialog::Accepted) {
    kwview_addToStrList(myDoc->searchForList, searchDialog->getSearchFor());
    searchFlags = searchDialog->getFlags() | (searchFlags & KateView::sfPrompt);
    initSearch(s, searchFlags);
    findAgain(s);
  }
  delete searchDialog;
}

void KateView::replace() {
  SearchDialog *searchDialog;

  if (isReadOnly()) return;

  if (!myDoc->hasMarkedText()) searchFlags &= ~KateView::sfSelected;
  searchDialog = new SearchDialog(this, myDoc->searchForList, myDoc->replaceWithList,
    searchFlags | KateView::sfReplace);

  // If the user has marked some text we use that otherwise
  // use the word under the cursor.
   QString str;
   if (myDoc->hasMarkedText())
     str = markedText();

   if (str.isEmpty())
     str = currentWord();

   if (!str.isEmpty())
   {
     str.replace(QRegExp("^\n"), "");
     int pos=str.find("\n");
     if (pos>-1)
       str=str.left(pos);
     searchDialog->setSearchText( str );
   }

  myViewInternal->focusOutEvent(0L);// QT bug ?
  if (searchDialog->exec() == QDialog::Accepted) {
//    myDoc->recordReset();
    kwview_addToStrList(myDoc->searchForList, searchDialog->getSearchFor());
    kwview_addToStrList(myDoc->replaceWithList, searchDialog->getReplaceWith());
    searchFlags = searchDialog->getFlags();
    initSearch(s, searchFlags);
    replaceAgain();
  }
  delete searchDialog;
}

void KateView::gotoLine() {
  GotoLineDialog *dlg;
  PointStruc cursor;

  dlg = new GotoLineDialog(this, myViewInternal->cursor.y + 1, myDoc->numLines());
//  dlg = new GotoLineDialog(myViewInternal->cursor.y + 1, this);

  if (dlg->exec() == QDialog::Accepted) {
//    myDoc->recordReset();
    cursor.x = 0;
    cursor.y = dlg->getLine() - 1;
    myDoc->needPreHighlight(cursor.y);
    myViewInternal->updateCursor(cursor);
    myViewInternal->center();
    myViewInternal->updateView(KateView::ufUpdateOnScroll);
    myDoc->updateViews(this); //uptade all other views except this one
  }
  delete dlg;
}


void KateView::initSearch(SConfig &s, int flags) {

  s.flags = flags;
  s.setPattern(myDoc->searchForList.first());

  if (!(s.flags & KateView::sfFromBeginning)) {
    // If we are continuing a backward search, make sure we do not get stuck
    // at an existing match.
    s.cursor = myViewInternal->cursor;
    TextLine::Ptr textLine = myDoc->getTextLine(s.cursor.y);
    QString const txt(textLine->getText(),textLine->length());
    const QString searchFor= myDoc->searchForList.first();
    int pos = s.cursor.x-searchFor.length()-1;
    if ( pos < 0 ) pos = 0;
    pos= txt.find(searchFor, pos, s.flags & KateView::sfCaseSensitive);
    if ( s.flags & KateView::sfBackward )
    {
      if ( pos <= s.cursor.x )  s.cursor.x= pos-1;
    }
    else
      if ( pos == s.cursor.x )  s.cursor.x++;
  } else {
    if (!(s.flags & KateView::sfBackward)) {
      s.cursor.x = 0;
      s.cursor.y = 0;
    } else {
      s.cursor.x = -1;
      s.cursor.y = myDoc->lastLine();
    }
    s.flags |= KateView::sfFinished;
  }
  if (!(s.flags & KateView::sfBackward)) {
    if (!(s.cursor.x || s.cursor.y))
      s.flags |= KateView::sfFinished;
  }
  s.startCursor = s.cursor;
}

void KateView::continueSearch(SConfig &s) {

  if (!(s.flags & KateView::sfBackward)) {
    s.cursor.x = 0;
    s.cursor.y = 0;
  } else {
    s.cursor.x = -1;
    s.cursor.y = myDoc->lastLine();
  }
  s.flags |= KateView::sfFinished;
  s.flags &= ~KateView::sfAgain;
}

void KateView::findAgain(SConfig &s) {
  int query;
  PointStruc cursor;
  QString str;

  QString searchFor = myDoc->searchForList.first();

  if( searchFor.isEmpty() ) {
    find();
    return;
  }

  do {
    query = KMessageBox::Cancel;
    if (myDoc->doSearch(s,searchFor)) {
      cursor = s.cursor;
      if (!(s.flags & KateView::sfBackward))
        s.cursor.x += s.matchedLength;
      myViewInternal->updateCursor(s.cursor); //does deselectAll()
      exposeFound(cursor,s.matchedLength,(s.flags & KateView::sfAgain) ? 0 : KateView::ufUpdateOnScroll,false);
    } else {
      if (!(s.flags & KateView::sfFinished)) {
        // ask for continue
        if (!(s.flags & KateView::sfBackward)) {
          // forward search
          str = i18n("End of document reached.\n"
                "Continue from the beginning?");
          query = KMessageBox::warningContinueCancel(this,
                           str, i18n("Find"), i18n("Continue"));
        } else {
          // backward search
          str = i18n("Beginning of document reached.\n"
                "Continue from the end?");
          query = KMessageBox::warningContinueCancel(this,
                           str, i18n("Find"), i18n("Continue"));
        }
        continueSearch(s);
      } else {
        // wrapped
        KMessageBox::sorry(this,
          i18n("Search string '%1' not found!").arg(KStringHandler::csqueeze(searchFor)),
          i18n("Find"));
      }
    }
  } while (query == KMessageBox::Continue);
}

void KateView::replaceAgain() {
  if (isReadOnly())
    return;

  replaces = 0;
  if (s.flags & KateView::sfPrompt) {
    doReplaceAction(-1);
  } else {
    doReplaceAction(KateView::srAll);
  }
}

void KateView::doReplaceAction(int result, bool found) {
  int rlen;
  PointStruc cursor;
  bool started;

  QString searchFor = myDoc->searchForList.first();
  QString replaceWith = myDoc->replaceWithList.first();
  rlen = replaceWith.length();

  switch (result) {
    case KateView::srYes: //yes
      myDoc->recordStart(this, s.cursor, configFlags,
        KateActionGroup::ugReplace, true);
      myDoc->recordReplace(s.cursor, s.matchedLength, replaceWith);
      replaces++;
      if (s.cursor.y == s.startCursor.y && s.cursor.x < s.startCursor.x)
        s.startCursor.x += rlen - s.matchedLength;
      if (!(s.flags & KateView::sfBackward)) s.cursor.x += rlen;
      myDoc->recordEnd(this, s.cursor, configFlags | KateView::cfPersistent);
      break;
    case KateView::srNo: //no
      if (!(s.flags & KateView::sfBackward)) s.cursor.x += s.matchedLength;
      break;
    case KateView::srAll: //replace all
      deleteReplacePrompt();
      do {
        started = false;
        while (found || myDoc->doSearch(s,searchFor)) {
          if (!started) {
            found = false;
            myDoc->recordStart(this, s.cursor, configFlags,
              KateActionGroup::ugReplace);
            started = true;
          }
          myDoc->recordReplace(s.cursor, s.matchedLength, replaceWith);
          replaces++;
          if (s.cursor.y == s.startCursor.y && s.cursor.x < s.startCursor.x)
            s.startCursor.x += rlen - s.matchedLength;
          if (!(s.flags & KateView::sfBackward)) s.cursor.x += rlen;
        }
        if (started) myDoc->recordEnd(this, s.cursor,
          configFlags | KateView::cfPersistent);
      } while (!askReplaceEnd());
      return;
    case KateView::srCancel: //cancel
      deleteReplacePrompt();
      return;
    default:
      replacePrompt = 0L;
  }

  do {
    if (myDoc->doSearch(s,searchFor)) {
      //text found: highlight it, show replace prompt if needed and exit
      cursor = s.cursor;
      if (!(s.flags & KateView::sfBackward)) cursor.x += s.matchedLength;
      myViewInternal->updateCursor(cursor); //does deselectAll()
      exposeFound(s.cursor,s.matchedLength,(s.flags & KateView::sfAgain) ? 0 : KateView::ufUpdateOnScroll,true);
      if (replacePrompt == 0L) {
        replacePrompt = new ReplacePrompt(this);
        myDoc->setPseudoModal(replacePrompt);//disable();
        connect(replacePrompt,SIGNAL(clicked()),this,SLOT(replaceSlot()));
        replacePrompt->show(); //this is not modal
      }
      return; //exit if text found
    }
    //nothing found: repeat until user cancels "repeat from beginning" dialog
  } while (!askReplaceEnd());
  deleteReplacePrompt();
}

void KateView::exposeFound(PointStruc &cursor, int slen, int flags, bool replace) {
  int x1, x2, y1, y2, xPos, yPos;

  VConfig c;
  myViewInternal->getVConfig(c);
  myDoc->selectLength(cursor,slen,c.flags);

  TextLine::Ptr textLine = myDoc->getTextLine(cursor.y);
  x1 = myDoc->textWidth(textLine,cursor.x)        -10;
  x2 = myDoc->textWidth(textLine,cursor.x + slen) +20;
  y1 = myDoc->fontHeight*cursor.y                 -10;
  y2 = y1 + myDoc->fontHeight                     +30;

  xPos = myViewInternal->xPos;
  yPos = myViewInternal->yPos;

  if (x1 < 0) x1 = 0;
  if (replace) y2 += 90;

  if (x1 < xPos || x2 > xPos + myViewInternal->width()) {
    xPos = x2 - myViewInternal->width();
  }
  if (y1 < yPos || y2 > yPos + myViewInternal->height()) {
    xPos = x2 - myViewInternal->width();
    yPos = myDoc->fontHeight*cursor.y - height()/3;
  }
  myViewInternal->setPos(xPos, yPos);
  myViewInternal->updateView(flags);// | ufPos,xPos,yPos);
  myDoc->updateViews(this);
}

void KateView::deleteReplacePrompt() {
  myDoc->setPseudoModal(0L);
}

bool KateView::askReplaceEnd() {
  QString str;
  int query;

  myDoc->updateViews();
  if (s.flags & KateView::sfFinished) {
    // replace finished
    str = i18n("%1 replacement(s) made").arg(replaces);
    KMessageBox::information(this, str, i18n("Replace"));
    return true;
  }

  // ask for continue
  if (!(s.flags & KateView::sfBackward)) {
    // forward search
    str = i18n("%1 replacement(s) made.\n"
               "End of document reached.\n"
               "Continue from the beginning?").arg(replaces);
    query = KMessageBox::questionYesNo(this, str, i18n("Replace"),
		i18n("Continue"), i18n("Stop"));
  } else {
    // backward search
    str = i18n("%1 replacement(s) made.\n"
                "Beginning of document reached.\n"
                "Continue from the end?").arg(replaces);
    query = KMessageBox::questionYesNo(this, str, i18n("Replace"),
                i18n("Continue"), i18n("Stop"));
  }
  replaces = 0;
  continueSearch(s);
  return (query == KMessageBox::No);
}

void KateView::replaceSlot() {
  doReplaceAction(replacePrompt->result(),true);
}

void KateView::installPopup(QPopupMenu *rmb_Menu)
{
  rmbMenu = rmb_Menu;
}

void KateView::readConfig()
{
  KConfig *config = KateFactory::instance()->config();
  config->setGroup("Kate View");

  searchFlags = config->readNumEntry("SearchFlags", KateView::sfPrompt);
  configFlags = config->readNumEntry("ConfigFlags", configFlags) & ~KateView::cfMark;

  config->sync();
}

void KateView::writeConfig()
{
  KConfig *config = KateFactory::instance()->config();
  config->setGroup("Kate View");

  config->writeEntry("SearchFlags",searchFlags);
  config->writeEntry("ConfigFlags",configFlags);

  config->sync();
}

void KateView::readSessionConfig(KConfig *config)
{
  PointStruc cursor;

  myViewInternal->xPos = config->readNumEntry("XPos");
  myViewInternal->yPos = config->readNumEntry("YPos");
  cursor.x = config->readNumEntry("CursorX");
  cursor.y = config->readNumEntry("CursorY");
  myViewInternal->updateCursor(cursor);
  myIconBorder = config->readBoolEntry("IconBorder on");
  setIconBorder(myIconBorder);
}

void KateView::writeSessionConfig(KConfig *config)
{
  config->writeEntry("XPos",myViewInternal->xPos);
  config->writeEntry("YPos",myViewInternal->yPos);
  config->writeEntry("CursorX",myViewInternal->cursor.x);
  config->writeEntry("CursorY",myViewInternal->cursor.y);
  config->writeEntry("IconBorder on", myIconBorder);
}

void KateView::configDialog()
{
  KWin kwin;

  KDialogBase *kd = new KDialogBase(KDialogBase::IconList,
                                    i18n("Configure Editor"),
                                    KDialogBase::Ok | KDialogBase::Cancel |
                                    KDialogBase::Help ,
                                    KDialogBase::Ok, this, "tabdialog");

  // color options
  QVBox *page=kd->addVBoxPage(i18n("Colors"), QString::null,
                              BarIcon("colorize", KIcon::SizeMedium) );
  ColorConfig *colorConfig = new ColorConfig(page);
  QColor* colors = getColors();
  colorConfig->setColors(colors);

 page = kd->addVBoxPage(i18n("Fonts"), i18n("Fonts Settings"),
                              BarIcon("fonts", KIcon::SizeMedium) );
  FontConfig *fontConfig = new FontConfig(page);
  fontConfig->setFont (myDoc->getFont());

  // indent options
  page=kd->addVBoxPage(i18n("Indent"), QString::null,
                       BarIcon("rightjust", KIcon::SizeMedium) );
  IndentConfigTab *indentConfig = new IndentConfigTab(page, this);

  // select options
  page=kd->addVBoxPage(i18n("Select"), QString::null,
                       BarIcon("misc") );
  SelectConfigTab *selectConfig = new SelectConfigTab(page, this);

  // edit options
  page=kd->addVBoxPage(i18n("Edit"), QString::null,
                       BarIcon("edit", KIcon::SizeMedium ) );
  EditConfigTab *editConfig = new EditConfigTab(page, this);

  // spell checker
  page = kd->addVBoxPage( i18n("Spelling"), i18n("Spell checker behavior"),
                          BarIcon("spellcheck", KIcon::SizeMedium) );
  KSpellConfig *ksc = new KSpellConfig(page, 0L, ksConfig(), false );

  kwin.setIcons(kd->winId(), kapp->icon(), kapp->miniIcon());

  HighlightDialogPage *hlPage;
  HlManager *hlManager;
  HlDataList hlDataList;
  ItemStyleList defaultStyleList;

  hlManager = HlManager::self();

  defaultStyleList.setAutoDelete(true);
  hlManager->getDefaults(defaultStyleList);

  hlDataList.setAutoDelete(true);
  //this gets the data from the KConfig object
  hlManager->getHlDataList(hlDataList);

  page=kd->addVBoxPage(i18n("Highlighting"),i18n("Highlighting configuration"),
                        BarIcon("edit",KIcon::SizeMedium));
  hlPage = new HighlightDialogPage(hlManager, &defaultStyleList, &hlDataList, 0, page);

 if (kd->exec()) {
    // color options
    colorConfig->getColors(colors);
    myDoc->setFont (fontConfig->getFont());

    applyColors();
    // indent options
    indentConfig->getData(this);
    // select options
    selectConfig->getData(this);
    // edit options
    editConfig->getData(this);
    // spell checker
    ksc->writeGlobalSettings();
    setKSConfig(*ksc);
    hlManager->setHlDataList(hlDataList);
    hlManager->setDefaults(defaultStyleList);
    hlPage->saveData();
  }

  delete kd;
}

int KateView::getHl() {
  return myDoc->highlightNum();
}

void KateView::setDontChangeHlOnSave()
{
    myDoc->setDontChangeHlOnSave();
}

void KateView::setHl(int n) {
  myDoc->setHighlight(n);
  myDoc->setDontChangeHlOnSave();
  myDoc->updateViews();
}

int KateView::getEol() {
  return myDoc->eolMode;
}

void KateView::setEol(int eol) {
  if (isReadOnly())
    return;

  myDoc->eolMode = eol;
  myDoc->setModified(true);
}



void KateView::paintEvent(QPaintEvent *e) {
  int x, y;

  QRect updateR = e->rect();                    // update rectangle
//  debug("Update rect = ( %i, %i, %i, %i )",
//    updateR.x(),updateR.y(), updateR.width(), updateR.height() );

  int ux1 = updateR.x();
  int uy1 = updateR.y();
  int ux2 = ux1 + updateR.width();
  int uy2 = uy1 + updateR.height();

  QPainter paint;
  paint.begin(this);

  QColorGroup g = colorGroup();
  x = width();
  y = height();

  paint.setPen(g.dark());
  if (uy1 <= 0) paint.drawLine(0,0,x-2,0);
  if (ux1 <= 0) paint.drawLine(0,1,0,y-2);

  paint.setPen(black);
  if (uy1 <= 1) paint.drawLine(1,1,x-3,1);
  if (ux1 <= 1) paint.drawLine(1,2,1,y-3);

  paint.setPen(g.midlight());
  if (uy2 >= y-1) paint.drawLine(1,y-2,x-3,y-2);
  if (ux2 >= x-1) paint.drawLine(x-2,1,x-2,y-2);

  paint.setPen(g.light());
  if (uy2 >= y) paint.drawLine(0,y-1,x-2,y-1);
  if (ux2 >= x) paint.drawLine(x-1,0,x-1,y-1);

  x -= 2 + 16;
  y -= 2 + 16;
  if (ux2 > x && uy2 > y) {
    paint.fillRect(x,y,16,16,g.background());
  }
  paint.end();
}

void KateView::resizeEvent(QResizeEvent *) {

//  debug("Resize %d, %d",e->size().width(),e->size().height());

//myViewInternal->resize(width() -20, height() -20);
  myViewInternal->tagAll();
  myViewInternal->updateView(0/*ufNoScroll*/);
}



//  Spellchecking methods

void KateView::spellcheck()
{
  if (isReadOnly())
    return;

  kspell.kspell= new KSpell (this, "KateView: Spellcheck", this,
                      SLOT (spellcheck2 (KSpell *)));

  connect (kspell.kspell, SIGNAL(death()),
          this, SLOT(spellCleanDone()));

  connect (kspell.kspell, SIGNAL (progress (unsigned int)),
          this, SIGNAL (spellcheck_progress (unsigned int)) );
  connect (kspell.kspell, SIGNAL (misspelling (QString , QStringList *, unsigned)),
          this, SLOT (misspelling (QString, QStringList *, unsigned)));
  connect (kspell.kspell, SIGNAL (corrected (QString, QString, unsigned)),
          this, SLOT (corrected (QString, QString, unsigned)));
  connect (kspell.kspell, SIGNAL (done(const QString&)),
          this, SLOT (spellResult (const QString&)));
}

void KateView::spellcheck2(KSpell *)
{
  myDoc->setReadOnly (TRUE);

  // this is a hack, setPseudoModal() has been hacked to recognize 0x01
  // as special (never tries to delete it)
  // this should either get improved (with a #define or something),
  // or kspell should provide access to the spell widget.
  myDoc->setPseudoModal((QWidget*)0x01);

  kspell.spell_tmptext = text();


  kspell.kspellon = TRUE;
  kspell.kspellMispellCount = 0;
  kspell.kspellReplaceCount = 0;
  kspell.kspellPristine = ! myDoc->isModified();

  kspell.kspell->setProgressResolution (1);

  kspell.kspell->check(kspell.spell_tmptext);
}

void KateView::misspelling (QString origword, QStringList *, unsigned pos)
{
  int line;
  unsigned int cnt;

  // Find pos  -- CHANGEME: store the last found pos's cursor
  //   and do these searched relative to that to
  //   (significantly) increase the speed of the spellcheck

  for (cnt = 0, line = 0 ; line <= myDoc->lastLine() && cnt <= pos ; line++)
    cnt += myDoc->getTextLine(line)->length()+1;

  // Highlight the mispelled word
  PointStruc cursor;
  line--;
  cursor.x = pos - (cnt - myDoc->getTextLine(line)->length()) + 1;
  cursor.y = line;
//  deselectAll(); // shouldn't the spell check be allowed within selected text?
  kspell.kspellMispellCount++;
  myViewInternal->updateCursor(cursor); //this does deselectAll() if no persistent selections
  VConfig c;
  myViewInternal->getVConfig(c);
  myDoc->selectLength(cursor,origword.length(),c.flags);
  myDoc->updateViews();
}

void KateView::corrected (QString originalword, QString newword, unsigned pos)
{
  //we'll reselect the original word in case the user has played with
  //the selection

  int line;
  unsigned int cnt=0;

  if(newword != originalword)
    {

      // Find pos
      for (line = 0 ; line <= myDoc->lastLine() && cnt <= pos ; line++)
        cnt += myDoc->getTextLine(line)->length() + 1;

      // Highlight the mispelled word
      PointStruc cursor;
      line--;
      cursor.x = pos - (cnt-myDoc->getTextLine(line)->length()) + 1;
      cursor.y = line;
      myViewInternal->updateCursor(cursor);
      VConfig c;
      myViewInternal->getVConfig(c);
      myDoc->selectLength(cursor, newword.length(),c.flags);

      myDoc->recordStart(this, cursor, configFlags,
        KateActionGroup::ugSpell, true, kspell.kspellReplaceCount > 0);
      myDoc->recordReplace(cursor, originalword.length(), newword);
      myDoc->recordEnd(this, cursor, configFlags | KateView::cfGroupUndo);

      kspell.kspellReplaceCount++;
    }

}

void KateView::spellResult (const QString &)
{
  deselectAll(); //!!! this should not be done with persistent selections

//  if (kspellReplaceCount) myDoc->recordReset();

  // we know if the check was cancelled
  // we can safely use the undo mechanism to backout changes
  // in case of a cancel, because we force the entire spell check
  // into one group (record)
  if (kspell.kspell->dlgResult() == 0) {
    if (kspell.kspellReplaceCount) {
      // backout the spell check
      VConfig c;
      myViewInternal->getVConfig(c);
      myDoc->undo(c);
      // clear the redo list, so the cancelled spell check can't be redo'ed <- say that word ;-)
      myDoc->clearRedo();
      // make sure the modified flag is turned back off
      // if we started with a clean buffer
      if (kspell.kspellPristine)
        myDoc->setModified(false);
    }
  }

  myDoc->setPseudoModal(0L);
  myDoc->setReadOnly (FALSE);

  myDoc->updateViews();

  kspell.kspell->cleanUp();
}

void KateView::spellCleanDone ()
{
  KSpell::spellStatus status = kspell.kspell->status();
  kspell.spell_tmptext = "";
  delete kspell.kspell;

  kspell.kspell = 0;
  kspell.kspellon = FALSE;

  if (status == KSpell::Error)
  {
     KMessageBox::sorry(this, i18n("ISpell could not be started.\n"
     "Please make sure you have ISpell properly configured and in your PATH."));
  }
  else if (status == KSpell::Crashed)
  {
     myDoc->setPseudoModal(0L);
     myDoc->setReadOnly (FALSE);

     myDoc->updateViews();
     KMessageBox::sorry(this, i18n("ISpell seems to have crashed."));
  }
  else
  {
     emit spellcheck_done();
  }
}

void KateView::dropEventPassEmited (QDropEvent* e)
{
  emit dropEventPass(e);
}

void KateView::printDlg ()
{
  if ( printer->setup( this ) )
  {
    QPainter paint( printer );
    QPaintDeviceMetrics pdm( printer );

    int y = 0;
    int lineCount = 0;
    while (  lineCount <= myDoc->lastLine()  )
    {
       if (y+myDoc->fontHeight >= pdm.height() )
      {
        printer->newPage();
        y=0;
      }

      myDoc->paintTextLine ( paint, lineCount, y, 0, pdm.width(), false );
      y += myDoc->fontHeight;

      lineCount++;
    }
  }
}

// Applies a new pattern to the search context.
void SConfig::setPattern(QString &newPattern) {
  bool regExp = (flags & KateView::sfRegularExpression);

  m_pattern = newPattern;
  if (regExp) {
    m_regExp.setCaseSensitive(flags & KateView::sfCaseSensitive);
    m_regExp.setPattern(m_pattern);
  }
}

// Applies the search context to the given string, and returns whether a match was found. If one is,
// the length of the string matched is also returned.
int SConfig::search(QString &text, int index) {
  bool regExp = (flags & KateView::sfRegularExpression);
  bool caseSensitive = (flags & KateView::sfCaseSensitive);

  if (flags & KateView::sfBackward) {
    if (regExp) {
      index = text.findRev(m_regExp, index);
    }
    else {
      index = text.findRev(m_pattern, index, caseSensitive);
    }
  }
  else {
    if (regExp) {
      index = text.find(m_regExp, index);
    }
    else {
      index = text.find(m_pattern, index, caseSensitive);
    }
  }

  // Work out the matched length.
  if (index != -1)
  {
    if (regExp) {
      m_regExp.match(text, index, &matchedLength, false);
    }
    else {
      matchedLength = m_pattern.length();
    }
  }
  return index;
}

void KateView::setActive (bool b)
{
  active = b;
}

bool KateView::isActive ()
{
  return active;
}

void KateView::setFocus ()
{
  QWidget::setFocus ();

  emit gotFocus (this);
}

bool KateView::eventFilter (QObject *object, QEvent *event)
{
  if ( object == myViewInternal )
    KCursor::autoHideEventFilter( object, event );

  if ( (event->type() == QEvent::FocusIn) )
    emit gotFocus (this);

  if ( (event->type() == QEvent::KeyPress) )
    {
        QKeyEvent * ke=(QKeyEvent *)event;

        if ((ke->key()==Qt::Key_Tab) || (ke->key()==Qt::Key_BackTab))
          {
            myViewInternal->keyPressEvent(ke);
            return true;
          }
    }
  return QWidget::eventFilter (object, event);
}

void KateView::findAgain (bool back)
{
  bool b= (searchFlags & sfBackward) > 0;
  initSearch(s, (searchFlags & ((b==back)?~sfBackward:~0) & ~sfFromBeginning)  // clear flag for forward searching
                | sfPrompt | sfAgain | ((b!=back)?sfBackward:0) );
  if (s.flags & sfReplace)
    replaceAgain();
  else
    KateView::findAgain(s);
}

void KateView::slotEditCommand ()
{
  bool ok;
  QString cmd = KLineEditDlg::getText("Editing Command", "", &ok, this);

  if (ok)
    myDoc->cmd()->execCmd (cmd, this);
}

void KateView::setIconBorder (bool enable)
{
  myIconBorder = enable;

  if (myIconBorder)
  {
    myViewInternal->move(myViewInternal->iconBorderWidth+2, 2);
    myViewInternal->leftBorder->show();
  }
  else
  {
    myViewInternal->leftBorder->hide();
    myViewInternal->move(2, 2);
  }
}

void KateView::toggleIconBorder ()
{
  setIconBorder (!myIconBorder);
}

void KateView::gotoMark (Kate::Mark *mark)
{
  PointStruc cursor;

  cursor.x = 0;
  cursor.y = mark->line;
  myDoc->needPreHighlight(cursor.y);
  myViewInternal->updateCursor(cursor);
  myViewInternal->center();
  myViewInternal->updateView(KateView::ufUpdateOnScroll);
  myDoc->updateViews(this);
}

void KateView::toggleBookmark ()
{
  TextLine::Ptr line = myDoc->getTextLine (currentLine());

  if (line->mark()&KateDocument::Bookmark)
    line->delMark(KateDocument::Bookmark);
  else
    line->addMark(KateDocument::Bookmark);

  myDoc->tagLines (currentLine(), currentLine());
  myDoc->updateViews();
}

void KateView::clearBookmarks()
{
  QList<Kate::Mark> list = myDoc->marks();
  for (int i=0; (uint) i < list.count(); i++)
  {
    if (list.at(i)->type&KateDocument::Bookmark)
    {
      myDoc->getTextLine(list.at(i)->line)->delMark(KateDocument::Bookmark);
      myDoc->tagLines(list.at(i)->line, list.at(i)->line);
    }
  }

  myDoc->updateViews();
}

void KateView::bookmarkMenuAboutToShow()
{
  bookmarkMenu->popupMenu()->clear ();
  bookmarkToggle->plug (bookmarkMenu->popupMenu());
  bookmarkClear->plug (bookmarkMenu->popupMenu());
  bookmarkMenu->popupMenu()->insertSeparator ();

  list = myDoc->marks();
  for (int i=0; (uint) i < list.count(); i++)
  {
    if (list.at(i)->type&KateDocument::Bookmark)
    {
      QString bText = textLine(list.at(i)->line);
      bText.truncate(32);
      bText.append ("...");
      bookmarkMenu->popupMenu()->insertItem ( QString("%1 - \"%2\"").arg(list.at(i)->line).arg(bText), this, SLOT (gotoBookmark(int)), 0, i );
    }
  }
}

void KateView::gotoBookmark (int n)
{
  gotoMark (list.at(n));
}

int KateView::getHlCount ()
{
  return HlManager::self()->highlights();
}

QString KateView::getHlName (int z)
{
  return HlManager::self()->hlName(z);
}

QString KateView::getHlSection (int z)
{
  return HlManager::self()->hlSection (z);
}

void KateView::slotIncFontSizes ()
{
  QFont font = myDoc->getFont();
  font.setPointSize (font.pointSize()+1);
  myDoc->setFont (font);
}

void KateView::slotDecFontSizes ()
{
  QFont font = myDoc->getFont();
  font.setPointSize (font.pointSize()-1);
  myDoc->setFont (font);
}

KateBrowserExtension::KateBrowserExtension( KateDocument *doc, KateView *view )
: KParts::BrowserExtension( doc, "katepartbrowserextension" )
{
  m_doc = doc;
  m_view = view;
  connect( m_doc, SIGNAL( selectionChanged() ), this, SLOT( slotSelectionChanged() ) );
  emit enableAction( "print", true );
}

void KateBrowserExtension::copy()
{
  m_doc->copy( 0 );
}

void KateBrowserExtension::print()
{
  m_view->printDlg ();
}

void KateBrowserExtension::slotSelectionChanged()
{
  emit enableAction( "copy", m_doc->hasMarkedText() );
}

const char*bookmark_xpm[]={
"12 16 4 1",
"b c #808080",
"a c #000080",
"# c #0000ff",
". c None",
"............",
"............",
"........###.",
".......#...a",
"......#.##.a",
".....#.#..aa",
"....#.#...a.",
"...#.#.a.a..",
"..#.#.a.a...",
".#.#.a.a....",
"#.#.a.a.....",
"#.#a.a...bbb",
"#...a..bbb..",
".aaa.bbb....",
"............",
"............"};

const char* breakpoint_xpm[]={
"11 16 6 1",
"c c #c6c6c6",
". c None",
"# c #000000",
"d c #840000",
"a c #ffffff",
"b c #ff0000",
"...........",
"...........",
"...#####...",
"..#aaaaa#..",
".#abbbbbb#.",
"#abbbbbbbb#",
"#abcacacbd#",
"#abbbbbbbb#",
"#abcacacbd#",
"#abbbbbbbb#",
".#bbbbbbb#.",
"..#bdbdb#..",
"...#####...",
"...........",
"...........",
"..........."};

const char*breakpoint_bl_xpm[]={
"11 16 7 1",
"a c #c0c0ff",
"# c #000000",
"c c #0000c0",
"e c #0000ff",
"b c #dcdcdc",
"d c #ffffff",
". c None",
"...........",
"...........",
"...#####...",
"..#ababa#..",
".#bcccccc#.",
"#acccccccc#",
"#bcadadace#",
"#acccccccc#",
"#bcadadace#",
"#acccccccc#",
".#ccccccc#.",
"..#cecec#..",
"...#####...",
"...........",
"...........",
"..........."};

const char*breakpoint_gr_xpm[]={
"11 16 6 1",
"c c #c6c6c6",
"d c #2c2c2c",
"# c #000000",
". c None",
"a c #ffffff",
"b c #555555",
"...........",
"...........",
"...#####...",
"..#aaaaa#..",
".#abbbbbb#.",
"#abbbbbbbb#",
"#abcacacbd#",
"#abbbbbbbb#",
"#abcacacbd#",
"#abbbbbbbb#",
".#bbbbbbb#.",
"..#bdbdb#..",
"...#####...",
"...........",
"...........",
"..........."};

const char*ddd_xpm[]={
"11 16 4 1",
"a c #00ff00",
"b c #000000",
". c None",
"# c #00c000",
"...........",
"...........",
"...........",
"#a.........",
"#aaa.......",
"#aaaaa.....",
"#aaaaaaa...",
"#aaaaaaaaa.",
"#aaaaaaa#b.",
"#aaaaa#b...",
"#aaa#b.....",
"#a#b.......",
"#b.........",
"...........",
"...........",
"..........."};



KateIconBorder::KateIconBorder(KateView *view, KateViewInternal *internalView)
    : QWidget(view), myView(view), myInternalView(internalView)
{
  lmbSetsBreakpoints = true;
}

KateIconBorder::~KateIconBorder()
{
}

void KateIconBorder::paintLine(int i)
{
  if (!myView->myIconBorder) return;

  QPainter p(this);

    int fontHeight = myView->doc()->fontHeight;
    int y = i*fontHeight - myInternalView->yPos;
    p.fillRect(0, y, myInternalView->iconBorderWidth-2, fontHeight, colorGroup().background());
    p.setPen(white);
    p.drawLine(myInternalView->iconBorderWidth-2, y, myInternalView->iconBorderWidth-2, y + fontHeight);
    p.setPen(QColor(colorGroup().background()).dark());
    p.drawLine(myInternalView->iconBorderWidth-1, y, myInternalView->iconBorderWidth-1, y + fontHeight);

    TextLine *line = myView->doc()->getTextLine(i);
    if (!line)
        return;

    if (line->mark()&KateDocument::Bookmark)
        p.drawPixmap(2, y, QPixmap(bookmark_xpm));      /*
    if (line && (line->breakpointId() != -1)) {
        if (!line->breakpointEnabled())
            p.drawPixmap(2, y, QPixmap(breakpoint_gr_xpm));
        else if (line->breakpointPending())
            p.drawPixmap(2, y, QPixmap(breakpoint_bl_xpm));
        else
            p.drawPixmap(2, y, QPixmap(breakpoint_xpm));
    }
    if (line->isExecutionPoint())
        p.drawPixmap(2, y, QPixmap(ddd_xpm));    */
}


void KateIconBorder::paintEvent(QPaintEvent* e)
{
  if (!myView->myIconBorder) return;

    int lineStart = 0;
    int lineEnd = 0;

    QRect updateR = e->rect();

    KateDocument *doc = myView->doc();
    int h = doc->fontHeight;
    int yPos = myInternalView->yPos;
    if (h) {
  	lineStart = (yPos + updateR.y()) / h;
        lineEnd = QMAX((yPos + updateR.y() + updateR.height()) / h, (int)doc->numLines());
    }

    for(int i = lineStart; i <= lineEnd; ++i)
        paintLine(i);
}


void KateIconBorder::mousePressEvent(QMouseEvent* e)
{
    myInternalView->placeCursor( 0, e->y(), 0 );

    KateDocument *doc = myView->doc();
    int cursorOnLine = (e->y() + myInternalView->yPos) / doc->fontHeight;
    TextLine *line = doc->getTextLine(cursorOnLine);

    switch (e->button()) {
    case LeftButton:
        if (!line)
            break;
        else
        {
            if (line->mark()&KateDocument::Bookmark)
              line->delMark (KateDocument::Bookmark);
            else
              line->addMark (KateDocument::Bookmark);

            doc->tagLines(cursorOnLine, cursorOnLine);
            doc->updateViews();
        }
        break;
 /*   case RightButton:
        {
            if (!line)
                break;
            KPopupMenu popup;
            popup.setCheckable(true);
            popup.insertTitle(i18n("Breakpoints/Bookmarks"));
            int idToggleBookmark =     popup.insertItem(i18n("Toggle bookmark"));
            popup.insertSeparator();
            int idToggleBreakpoint =   popup.insertItem(i18n("Toggle breakpoint"));
            int idEditBreakpoint   =   popup.insertItem(i18n("Edit breakpoint"));
            int idEnableBreakpoint =   popup.insertItem(i18n("Disable breakpoint"));
            popup.insertSeparator();
            popup.insertSeparator();
            int idLmbSetsBreakpoints = popup.insertItem(i18n("LMB sets breakpoints"));
            int idLmbSetsBookmarks   = popup.insertItem(i18n("LMB sets bookmarks"));

            popup.setItemChecked(idLmbSetsBreakpoints, lmbSetsBreakpoints);
            popup.setItemChecked(idLmbSetsBookmarks, !lmbSetsBreakpoints);

            if (line->breakpointId() == -1) {
                popup.setItemEnabled(idEditBreakpoint, false);
                popup.setItemEnabled(idEnableBreakpoint, false);
                popup.changeItem(idEnableBreakpoint, i18n("Enable breakpoint"));
            }
            int res = popup.exec(mapToGlobal(e->pos()));
            if (res == idToggleBookmark) {
                line->toggleBookmark();
                doc->tagLines(cursorOnLine, cursorOnLine);
                doc->updateViews();
            } else if (res == idToggleBreakpoint)
                emit myView->toggledBreakpoint(cursorOnLine);
            else if (res == idEditBreakpoint)
                emit myView->editedBreakpoint(cursorOnLine);
            else if (res == idEnableBreakpoint)
                emit myView->toggledBreakpointEnabled(cursorOnLine+1);
            else if (res == idLmbSetsBreakpoints || res == idLmbSetsBookmarks)
                lmbSetsBreakpoints = !lmbSetsBreakpoints;
            break;
        }
    case MidButton:
        line->toggleBookmark();
        doc->tagLines(cursorOnLine, cursorOnLine);
        doc->updateViews();
        break;      */
    default:
        break;
    }
}



