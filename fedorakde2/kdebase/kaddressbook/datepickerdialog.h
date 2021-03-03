/* -*- C++ -*-
 * A dialog to enter a date.
 *
 * the KDE addressbook
 * copyright:  (C) Mirko Sucker, 1998
 * license:    GNU Public License, Version 2
 * mail to:    Mirko Sucker <mirko.sucker@unibw-hamburg.de>
 * requires:   C++-compiler, STL, string class, Qt > 1.40
 *             NANA (for debugging)
 * $Revision: 1.2 $
 */

#ifndef DATEPICKERDIALOG_H_INCL
#define DATEPICKERDIALOG_H_INCL

#include <qdialog.h>
#include <qdatetime.h>

#ifndef KAB_WIDGETS_H
#define KAB_WIDGETS_H
#endif // KAB_WIDGETS_H

#include <qlabel.h>
#include <qcolor.h>
#include <qtooltip.h>
class QPushButton;
#include <kdatepik.h>


class DateLabel : public QLabel
{
  // ############################################################################
  Q_OBJECT
  // ----------------------------------------------------------------------------
public:
  DateLabel(QWidget* parent=0, const char* name=0, 
	    const QDate& dateToSet=QDate::currentDate());
  // ----------------------------------------------------------------------------
public slots:
  virtual void setDate(); // uses KDatePicker
  virtual void setDate(QDate);
  const QDate& getDate(); 
  void enableChangeDialog(bool state=true);
  // ----------------------------------------------------------------------------
protected:
  QDate date;
  bool changeDate; // if true, doubleclick shows a dialog 
  // Events
  void mouseDoubleClickEvent(QMouseEvent*);
  // ----------------------------------------------------------------------------
signals:
  void dateSelected(QDate);
  // ############################################################################
};

class DatePickerDialog : public QDialog
{
  // ############################################################################
  Q_OBJECT
  // ----------------------------------------------------------------------------
public:
  DatePickerDialog(QString title, QWidget* parent=0, const char* name=0);
  ~DatePickerDialog();
  const QDate& getDate();
  bool setDate(const QDate&);
  // ----------------------------------------------------------------------------
protected:
  DateLabel* dateLabel;
  QPushButton* ok;
  QPushButton* cancel;
  KDatePicker* datePicker;
  void initializeGeometry();
  // ############################################################################
};

#endif // DATEPICKERDIALOG_H_INCL
