/*  -*- C++ -*-
    This file is part of the KDE libraries
    Copyright (C) 1997 Tim D. Gilman (tdgilman@best.org)
              (C) 1998-2001 Mirko Boehm (mirko@kde.org)
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
#ifndef KDATEPICKER_H
#define KDATEPICKER_H
#include <qdatetime.h>
#include <qrect.h>
#include <qframe.h>

class QLineEdit;
class QToolButton;
class KDateValidator;
class KDateTable;

/**
 * Provide a widget for calendar date input.
 * 
 *     Different from the
 *     previous versions, it now emits two types of signals, either
 *     @ref dateSelected() or @ref dateEntered() (see documentation for both
 *     signals). 
 * 
 *     A line edit has been added in the newer versions to allow the user 
 *     to select a date directly by entering numbers like 19990101
 *     or 990101. 
 *
 * @image kdatepicker.png KDatePicker
 *
 *     @version $Id$
 *     @author Tim Gilman, Mirko Boehm
 *
 * @short A date selection widget.
 **/
class KDatePicker: public QFrame {
  Q_OBJECT
public:
  /** The usual constructor.  The given date will be displayed 
   * initially.
   **/
  KDatePicker(QWidget *parent=0, 
	      QDate=QDate::currentDate(), 
	      const char *name=0);
  /**
   * The destructor. 
   **/
  virtual ~KDatePicker();

  /** The size hint for @ref KDatePickers. The size hint recommends the
   *   minimum size of the widget so that all elements may be placed
   *  without clipping. This sometimes looks ugly, so when using the
   *  size hint, try adding 28 to each of the reported numbers of
   *  pixels.
   **/
  QSize sizeHint() const; 

  /** Set the date.
   *
   *  @returns @p false and does not change anything 
   *      if the date given is invalid. 
   **/
  bool setDate(const QDate&);

  /** 
   * Retrieve the date.
   **/
  const QDate& getDate();

  /** 
   * Enable or disable the widget. 
   **/
  void setEnabled(bool);

  /** 
   * Set the font size of the widgets elements. 
   **/
  void setFontSize(int);
  /**
   * Font size of the widget elements.
   */
  int fontSize() const
    { return fontsize; }

protected:
  /// the resize event
  void resizeEvent(QResizeEvent*);
  /// the year forward button
  QToolButton *yearForward;
  /// the year backward button
  QToolButton *yearBackward;
  /// the month forward button
  QToolButton *monthForward;
  /// the month backward button
  QToolButton *monthBackward;
  /// the button for selecting the month directly
  QToolButton *selectMonth;
  /// the button for selecting the year directly
  QToolButton *selectYear;
  /// the line edit to enter the date directly
  QLineEdit *line;
  /// the validator for the line edit:
  KDateValidator *val;
  /// the date table 
  KDateTable *table;
  /// the size calculated during resize events
    //  QSize sizehint;
  /// the widest month string in pixels:
  QSize maxMonthRect;
protected slots:
  void dateChangedSlot(QDate);
  void tableClickedSlot();
  void monthForwardClicked();
  void monthBackwardClicked();
  void yearForwardClicked();
  void yearBackwardClicked();
  void selectMonthClicked();
  void selectYearClicked();
  void lineEnterPressed();
signals:
  /** This signal is emitted each time the selected date is changed. 
      Usually, this does not mean that the date has been entered,
      since the date also changes, for example, when another month is
      selected. 
      @see dateSelected  */
  void dateChanged(QDate);
  /** This signal is emitted each time a day has been selected by
      clicking on the table (hitting a day in the current month). It
      has the same meaning as dateSelected() in older versions of
      KDatePicker. */
  void dateSelected(QDate);
  /** This signal is emitted when enter is pressed and a VALID date
      has been entered before into the line edit. Connect to both
      dateEntered() and dateSelected() to receive all events where the 
      user really enters a date. */
  void dateEntered(QDate);
  /** This signal is emitted when the day has been selected by
      clicking on it in the table. */
  void tableClicked();

private:
  /// the font size for the widget
  int fontsize;

  class KDatePickerPrivate;
  KDatePickerPrivate *d;
};

#endif //  KDATEPICKER_H
