/* This file is part of the KDE libraries
    Copyright (C) 1997 Mario Weilguni (mweilguni@sime.com)

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

#ifndef __KBUTTONBOX__H__
#define __KBUTTONBOX__H__

#include <qwidget.h>
#include <qpushbutton.h>

/**
 * Container widget for buttons. 
 * 
 * This class uses Qt layout control to place the buttons; can handle
 * both vertical and horizontal button placement.  The default border
 * is now @p 0 (making it easier to deal with layouts). The space
 * between buttons is now more Motif compliant.
 *
 * @author Mario Weilguni <mweilguni@sime.com>
 * @version $Id$
 **/

class KButtonBox : public QWidget
{
  Q_OBJECT

public:
  /**
    * Create an empty container for buttons.
    *
    * If @p _orientation is @p Vertical, the buttons inserted with 
    * @ref addButton() are laid out from top to bottom, otherwise they 
    * are laid out from left to right.
    */
  KButtonBox(QWidget *parent, Orientation _orientation = Horizontal,
	     int border = 0, int _autoborder = 6);

  /**
    * Free private data field
    */
  ~KButtonBox();

  /**
    * @return The minimum size needed to fit all buttons.
    *
    * This size is
    * calculated by the width/height of all buttons plus border/autoborder.
    */
  virtual QSize sizeHint() const;
  /**
   * @reimplemented
   */
  virtual QSizePolicy sizePolicy() const;
  /**
   * @reimplemented
   */
  virtual void resizeEvent(QResizeEvent *);

  /**
    * Add a new @ref QPushButton.  
    *
    * @param noexpand If @p noexpand is @p false, the width
    * of the button is adjusted to fit the other buttons (the maximum
    * of all buttons is taken). If @p noexpand is @p true, the width of this
    * button will be set to the minimum width needed for the given text).
    *
    * @return A pointer to the new button.
    */
  QPushButton *addButton(const QString& text, bool noexpand = FALSE);

  /**
    * Add a new @ref QPushButton.  
    *
    * @param receiver An object to connect to.
    * @param slot A Qt slot to connect the 'clicked()' signal to.
    * @param noexpand If @p noexpand is @p false, the width
    * of the button is adjusted to fit the other buttons (the maximum
    * of all buttons is taken). If @p noexpand @p true, the width of this
    * button will be set to the minimum width needed for the given text).
    *
    * @return A pointer to the new button.
    */
  QPushButton *addButton(const QString& text, QObject * receiver, const char * slot, bool noexpand = FALSE);

  /**
    * Add a stretch to the buttonbox. 
    *
    * Can be used to separate buttons.  That is, if you add the
    * buttons OK and Cancel, add a stretch, and then add the button Help,
    * the buttons OK and Cancel will be left-aligned (or top-aligned
    * for vertical) whereas Help will be right-aligned (or
    * bottom-aligned for vertical).
    *
    * @see QBoxLayout */
  void addStretch(int scale = 1);

  /**
    * This function must be called @em once after all buttons have been
    * inserted.
    *
    * It will start layout control.
    */
  void layout();

public: // as PrivateData needs Item, it has to be exported
  class Item;
protected:
  class PrivateData;  

  /**
    * @return the best size for a button. Checks all buttons and takes
    * the maximum width/height.
    */
  QSize bestButtonSize() const;
  void  placeButtons();
  QSize buttonSizeHint(QPushButton *) const;
  
  PrivateData *data;
};

#endif
