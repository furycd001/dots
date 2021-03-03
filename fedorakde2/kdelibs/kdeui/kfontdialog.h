/*
    $Id$

    Requires the Qt widget libraries, available at no cost at
    http://www.troll.no

    Copyright (C) 1997 Bernd Johannes Wuebben <wuebben@kde.org>
    Copyright (c) 1999 Preston Brown <pbrown@kde.org>
    Copyright (c) 1999 Mario Weilguni <mweilguni@kde.org>

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
#ifndef _K_FONT_DIALOG_H_
#define _K_FONT_DIALOG_H_

#include <qlineedit.h>
#include <kdialogbase.h>

class QComboBox;
class QFont;
class QGroupBox;
class QLabel;
class QStringList;
class KListBox;

// Just a marker for a feature that was added after 2.2-beta2.
// When KOffice depends on kdelibs-2.2, this can be removed.
#define KFONTCHOOSER_HAS_SETCOLOR

/**
 * A widget for interactive font selection.
 *
 * While @ref KFontChooser as an ordinary widget can be embedded in
 * custom dialogs and therefore is very flexible, in most cases
 * it is preferable to use the convenience functions in
 * @ref KFontDialog.
 *
 * @short A font selection widget.
 * @author Preston Brown <pbrown@kde.org>, Bernd Wuebben <wuebben@kde.org>
 * @version $Id$
 */
class KFontChooser : public QWidget
{
  Q_OBJECT

public:
  /**
   *  @li @p FamilyList - Identifies the family (leftmost) list.
   *  @li @p StyleList -  Identifies the style (center) list.
   *  @li @p SizeList -   Identifies the size (rightmost) list.
   */
  enum FontColumn { FamilyList=0x01, StyleList=0x02, SizeList=0x04,
    CharsetList=0x08 };

  /**
   * Constructs a font picker widget.
   *
   * @param parent The parent widget.
   * @param name The widget name.
   * @param onlyFixedFonts Only display fonts which have fixed-width
   *        character sizes.
   * @param fontList A list of fonts to display, in XLFD format.  If
   *        no list is formatted, the internal KDE font list is used.
   *        If that has not been created, X is queried, and all fonts
   *        available on the system are displayed.
   * @param visibleListSize The minimum number of visible entries in the
   *        fontlists.
   */
  KFontChooser(QWidget *parent = 0L, const char *name = 0L,
	       bool onlyFixed = false,
	       const QStringList &fontList = QStringList(),
	       bool makeFrame = true, int visibleListSize=8 );

  virtual ~KFontChooser();

  /**
   * Enable or disable a font column in the chooser.
   *
   * Use this
   * function if your application does not need or supports all font
   * properties.
   *
   * @param font Specifie the columns. An or'ed combination of
   *        @p FamilyList, @p StyleList and @p SizeList is possible.
   * @param state If @p false the columns are disabled.
   */
  void enableColumn( int column, bool state );

  /**
   * Set the currently selected font in the chooser.
   *
   * @param font The font to select.
   * @param onlyFixed Readjust the font list to display only fixed
   *        width fonts if @p true, or vice-versa.
   */
  void setFont( const QFont &font, bool onlyFixed = false );

  /**
   * @return The currently selected font in the chooser.
   */
  QFont font() const { return selFont; }

  /**
   * Set the color to use in the preview
   */
  void setColor( const QColor & col );

  /**
   * @return The color currently used in the preview (default: black)
   */
  QColor color() const;

  /**
   * Set the currently selected charset in the chooser.
   */
  void setCharset( const QString & charset );

  /**
   * @return The currently selected charset in the dialog.
   */
  QString charset() const;

  /**
   * @return The current text in the sample text input area.
   */
  QString sampleText() const { return sampleEdit->text(); }

  /**
   * Set the sample text.
   *
   * Normally you should not change this
   * text, but it can be better to do this if the default text is
   * too large for the edit area when using the default font of your
   * application.
   *
   * @param text The new sample text. The current will be removed.
   */
  void setSampleText( const QString &text )
  {
    sampleEdit->setText( text );
  }

  /**
   * Convert a @ref QFont into the corresponding X Logical Font
   * Description (XLFD).
   *
   * @param theFont The font to convert.
   * @return A string representing the given font in XLFD format.
   */
  static QString getXLFD( const QFont &theFont )
    { return theFont.rawName(); }

  /**
   * Create a list of font strings that match @p pattern.
   *
   * @param list The list is returned here.
   * @param pattern The font pattern.
   */
  static void getFontList( QStringList &list, const char *pattern );

  /**
   * Create a list of font strings.
   *
   * @param list The list is returned here.
   * @param fixed Flag, when true only fixed fonts are returned.
   */
  static void getFontList( QStringList &list, bool fixed );

  /**
   * Reimplemented for internal reasons.
   */
  virtual QSize sizeHint( void ) const;

private slots:
  void family_chosen_slot(const QString&);
  void size_chosen_slot(const QString&);
  void style_chosen_slot(const QString&);
  void displaySample(const QFont &font);
  void charset_chosen_slot(const QString&);
  void showXLFDArea(bool);

signals:
  /**
   * connect to this to monitor the font as it is selected.
   */
  void fontSelected( const QFont &font );

private:
  void fillFamilyListBox(bool onlyFixedFonts = false);
  void fillCharsetsCombo();
  // This one must be static since getFontList( QStringList, char*) is so
  static void addFont( QStringList &list, const char *xfont );

  void setupDisplay();

  // pointer to an optinally supplied list of fonts to
  // inserted into the fontdialog font-family combo-box
  QStringList  fontList;

  QGroupBox    *xlfdBox;

  QLineEdit    *sampleEdit;
  QLineEdit    *xlfdEdit;

  QLabel       *familyLabel;
  QLabel       *styleLabel;
  QLabel       *sizeLabel;
  KListBox     *familyListBox;
  KListBox     *styleListBox;
  KListBox     *sizeListBox;
  QComboBox    *charsetsCombo;

  QFont        selFont;

  bool usingFixed;

  class KFontChooserPrivate;
  KFontChooserPrivate *d;
};

/**
 * The KFontDialog provides a dialog for interactive font selection.
 * It is basically a thin wrapper around the @ref KFontChooser widget,
 * which can also be used standalone. In most cases, the simplest
 * use of this class is the static method @ref KFontDialog::getFont(),
 * which pops up the dialog, allows the user to select a font, and
 * returns when the dialog is closed.
 *
 * Example:
 *
 * <pre>
 *      QFont myFont;
 *      int result = KFontDialog::getFont( myFont );
 *      if ( result == KFontDialog::Accepted )
 *            ...
 * </pre>
 *
 * @image kfontdialog.png KFontDialog
 *
 * @short A font selection dialog.
 * @author Preston Brown <pbrown@kde.org>, Bernd Wuebben <wuebben@kde.org>
 * @version $Id$
 */
class KFontDialog : public KDialogBase  {
    Q_OBJECT

public:
  /**
   * Constructs a font selection dialog.
   *
   * @param parent The parent widget of the dialog, if any.
   * @param name The name of the dialog.
   * @param modal Specifies whether the dialog is modal or not.
   * @param onlyFixed only display fonts which have fixed-width
   *        character sizes.
   * @param fontlist a list of fonts to display, in XLFD format.  If
   *        no list is formatted, the internal KDE font list is used.
   *        If that has not been created, X is queried, and all fonts
   *        available on the system are displayed.
   * @param makeFrame Draws a frame with titles around the contents.
   *
   */
  KFontDialog( QWidget *parent = 0L, const char *name = 0,
	       bool onlyFixed = false, bool modal = false,
	       const QStringList &fontlist = QStringList(),
	       bool makeFrame = true );

  /**
   * Sets the currently selected font in the dialog.
   *
   * @param font The font to select.
   * @param onlyFixed readjust the font list to display only fixed
   *        width fonts if true, or vice-versa
   */
  void setFont( const QFont &font, bool onlyFixed = false )
    { chooser->setFont(font, onlyFixed); }

  /**
   * @return The currently selected font in the dialog.
   */
  QFont font() const { return chooser->font(); }

  /**
   * Sets the currently selected charset in the dialog.
   */
  void setCharset( const QString & charset )
    { chooser->setCharset( charset ); }

  /**
   * @return The currently selected charset in the dialog.
   */
  QString charset() const { return chooser->charset(); }

  /**
   * Creates a modal font dialog, lets the user choose a font,
   * and returns when the dialog is closed.
   *
   * @param theFont a reference to the font to write the chosen font
   *        into.
   * @param onlyFixed if true, only select from fixed-width fonts.
   * @param parent Parent widget of the dialog. Specifying a widget different
   *        from 0 (Null) improves centering (looks better).
   * @param makeFrame Draws a frame with titles around the contents.
   *
   * @returns @ref QDialog::result().
   */
  static int getFont( QFont &theFont, bool onlyFixed = false,
		      QWidget *parent = 0L, bool makeFrame = true );

  /**
   * When you are not only interested in the font selected, but also
   * in the example string typed in, you can call this method.
   *
   * @param theFont a reference to the font to write the chosen font
   *        into.
   * @param theString a reference to the example text that was typed.
   * @param onlyFixed if true, only select from fixed-width fonts.
   * @param parent Parent widget of the dialog. Specifying a widget different
   *        from 0 (Null) improves centering (looks better).
   * @param makeFrame Draws a frame with titles around the contents.
   * @return The result of the dialog.
   */
  static int getFontAndText( QFont &theFont, QString &theString,
			     bool onlyFixed = false, QWidget *parent = 0L,
			     bool makeFrame = true );

signals:
  /**
   * Emitted whenever the currently selected font changes.
   * Connect to this to monitor the font as it is selected if you are
   * not running modal.
   */
  void fontSelected( const QFont &font );

protected:
  KFontChooser *chooser;

private:
  class KFontDialogPrivate;
  KFontDialogPrivate *d;

};

#endif
