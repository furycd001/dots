/* This file is part of the KDE libraries
    Copyright (C) 2000 David Faure <faure@kde.org>, Alexander Neundorf <neundorf@kde.org>

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

#ifndef KEDITLISTBOX_H
#define KEDITLISTBOX_H

#include <qgroupbox.h>
#include <qlistbox.h>

class KLineEdit;
class QPushButton;

/**
 * An editable listbox
 *
 * This class provides a editable listbox ;-), this means
 * a listbox which is accompanied by a line edit to enter new
 * items into the listbox and pushbuttons to add and remove
 * items from the listbox and two buttons to move items up and down.
 */
class KEditListBox : public QGroupBox
{
   Q_OBJECT
   public:

      /**
       * Enumeration of the buttons, the listbox offers. Specify them in the
       * constructor in the buttons parameter.
       */
      enum Button { Add = 1, Remove = 2, UpDown = 4, All = Add|Remove|UpDown };

      /**
       * Create an editable listbox.
       *
       * If @p checkAtEntering is true, after every character you type
       * in the line edit KEditListBox will enable or disable
       * the Add-button, depending whether the current content of the
       * line edit is already in the listbox. Maybe this can become a
       * performance hit with large lists on slow machines.
       * If @p checkAtEntering is false,
       * it will be checked if you press the Add-button. It is not
       * possible to enter items twice into the listbox.
       */
      KEditListBox(QWidget *parent = 0, const char *name = 0,
		   bool checkAtEntering=false, int buttons = All );
      /**
       * Create an editable listbox.
       *
       * The same as the other constructor, additionally it takes
       * @title, which will be the title of the frame around the listbox.
       */
      KEditListBox(const QString& title, QWidget *parent = 0,
		   const char *name = 0, bool checkAtEntering=false,
		   int buttons = All );
      virtual ~KEditListBox();

      /**
       * Return a pointer to the embedded QListBox.
       */
      QListBox* listBox() const     { return m_listBox; }
      /**
       * Return a pointer to the embedded QLineEdit.
       */
      KLineEdit* lineEdit() const     { return m_lineEdit; }
      /**
       * Return a pointer to the Add button
       */
      QPushButton* addButton() const     { return servNewButton; }
      /**
       * Return a pointer to the Remove button
       */
      QPushButton* removeButton() const     { return servRemoveButton; }
      /**
       * Return a pointer to the Up button
       */
      QPushButton* upButton() const     { return servUpButton; }
      /**
       * Return a pointer to the Down button
       */
      QPushButton* downButton() const     { return servDownButton; }

      /**
       * See @ref QListBox::count()
       */
      int count() const   { return int(m_listBox->count()); }
      /**
       * See @ref QListBox::insertStringList()
       */
      void insertStringList(const QStringList& list, int index=-1);
      /**
       * See @ref QListBox::insertStringList()
       */
      void insertStrList(const QStrList* list, int index=-1);
      /**
       * See @ref QListBox::insertStrList()
       */
      void insertStrList(const QStrList& list, int index=-1);
      /**
       * See @ref QListBox::insertStrList()
       */
      void insertStrList(const char ** list, int numStrings=-1, int index=-1);
      /**
       * See @ref QListBox::insertItem()
       */
      void insertItem(const QString& text, int index=-1) {m_listBox->insertItem(text,index);}
      /**
       * Clears both the listbox and the line edit.
       */
      void clear();
      /**
       * See @ref QListBox::text()
       */
      QString text(int index) const { return m_listBox->text(index); }
      /**
       * See @ref QListBox::currentItem()
       */
      int currentItem() const { return m_listBox->currentItem(); }
      /**
       * See @ref QListBox::currentText()
       */
      QString currentText() const  { return m_listBox->currentText(); }

      /**
       * @returns a stringlist of all items in the listbox
       */
      QStringList items() const;

   signals:
      void changed();

   protected slots:
      //the names should be self-explaining
      void moveItemUp();
      void moveItemDown();
      void addItem();
      void removeItem();
      void enableMoveButtons(int index);
      void enableAddButton(const QString& text);

   protected:
      //### should be private, there are accessors for all those.
      QListBox *m_listBox;
      QPushButton *servUpButton, *servDownButton;
      QPushButton *servNewButton, *servRemoveButton;
      KLineEdit *m_lineEdit;
   private:
      //this is called in both ctors, to avoid code duplication
      void init( bool checkAtEntering, int buttons );

      //our lovely private d-pointer
      class PrivateData;
      PrivateData *d;
};

#endif
