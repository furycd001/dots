/* This file is part of the KDE libraries
   Copyright (C) 1998 Kurt Granroth <granroth@kde.org>
   Copyright (C) 2000 Peter Putzer <putzer@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KURLLABEL_H
#define KURLLABEL_H

#include <qlabel.h>

class QColor;
class QCursor;
class QPixmap;

/**
 * A label class that supports displaying an active hyperlink.
 *
 * KURLLabel is a drop-in replacement for QLabel that handles text
 * in a fashion similar to how an HTML widget handles hyperlinks.  The
 * text can be underlined (or not) and set to different colors.  It
 * can also "glow" (cycle colors) when the mouse passes over it.
 *
 * KURLLabel also provides signals for several events, including
 * the mouse leaving and entering the text area and all forms of
 * mouse clicking.
 *
 * A typical usage would be something like so:
 *
 * <PRE>
 *     KURLLabel *address = new KURLLabel(this);
 *     address->setText("My homepage");
 *     address->setURL("http://www.home.com/~me");
 *     connect(address, SIGNAL(leftClickedURL(const QString&)),
 *                      SLOT(processMyURL(const QString&)));
 * </PRE>
 *
 * In this example, the text "My homepage" would be displayed
 * as blue, underlined text.  When the mouse passed over it, 
 * it would "glow" red.  When the user clicks on the text, the
 * signal @ref leftClickedURL() would be emitted with "http://www.home.com/~me"
 * as its argument.
 *
 * @short A drop-in replacement for QLabel that displays hyperlinks.
 * @author Kurt Granroth <granroth@kde.org> (Interface)
 * @author Peter Putzer <putzer@kde.org> (Rewrite)
 * @version $Id$
 * 
 */
class KURLLabel : public QLabel
{
  Q_OBJECT
  Q_PROPERTY (QString url READ url WRITE setURL)
  Q_PROPERTY (QPixmap altPixmap READ altPixmap WRITE setAltPixmap)

public:
  /**
   * Default constructor.
   *
   * Use @ref #setURL() and @ref #setText() or @ref QListView::setPixmap()
   * to set the resp. properties.
   */
  KURLLabel (QWidget* parent = 0L, const char* name = 0L);

  /**
   * Convenience constructor.
   *
   * @param url is the URL emitted when the label is clicked.
   * @param text is the displayed string. If it's equal to QString::null
   * the @p url will be used instead.
   *
   * @p parent and @p name are passed to @ref QLabel.
   */
  KURLLabel (const QString& url, const QString& text = QString::null,
              QWidget* parent = 0L, const char* name = 0L);

  /**
   * Destructor.
   */
  virtual ~KURLLabel ();

  /**
   * Retreive the URL. 
   *
   * @return the URL.
   */
  const QString& url () const;

  /**
   * Retreive the tooltip text.
   *
   * @return the current tooltip text.
   */
  const QString& tipText () const;

  /**
   * @return true if a tooltip will be displayed.
   *
   * @see setTipText()
   */
  bool useTips () const;

  /**
   * @return true if the cursor will change while over the URL.
   *
   * @see setUseCursor ()
   */
  bool useCursor () const;

  /**
   * When this is on, the text will switch to the selected 
   * color whenever the mouse passes over it.
   */
  bool isGlowEnabled () const;

  /**
   * This feature is very similar to the "glow" feature in that the color of the
   * label switches to the selected color when the cursor passes
   * over it. In addition, underlining is turned on for as
   * long as the mouse is overhead. Note that if "glow" and
   * underlining are both already turned on, this feature
   * will have no visible effect.
   */
  bool isFloatEnabled () const;

  /**
   * @return the alternate pixmap (may be 0L if none was set).
   */
  const QPixmap* altPixmap () const;

public slots:
  /**
   * Turn on or off the underlining.
   *
   *  When this is on, the
   * text will be underlined.  By default, it is @p true.
   */
  void setUnderline (bool on = true);

  /**
   * Sets the URL for this label to @p url.
   *
   * @see #url
   */
  void setURL (const QString& url);

  /**
   * Overridden for internal reasons; the API remains unaffected.
   */
  virtual void setFont (const QFont&);

  /**
   * Turn on or off the tool tip feature.
   *
   * When this is on, the URL will be displayed as a
   * tooltip whenever the mouse passes passes over it.
   * By default, it is @p false.
   */
  void setUseTips (bool on = true);

  /**
   * Specifies what text to display when tooltips are turned on.
   *
   * If this is not used, the tip will default to the URL.
   *
   * @see setUseTips()
   */
  void setTipText (const QString& tip);

  /**
   * Set the highlight color.
   *
   * This is the default foreground
   * color (non-selected).  By default, it is @p blue.
   */
  void setHighlightedColor(const QColor& highcolor);
  
  /**
   * This is an overloaded version for convenience.
   *
   * @see setHighlightedColor()
   */
  void setHighlightedColor(const QString& highcolor);
  
  /**
   * Set the selected color.
   *
   * This is the color the text will change
   * to when either a mouse passes over it and "glow" mode is on or
   * when it is selected (clicked).  By default, it is @p red.
   */
  void setSelectedColor(const QColor& selcolor);
  
  /**
   * This is an overloaded version for convenience.
   *
   * @see setSelectedColor()
   */
  void setSelectedColor(const QString& selcolor);

  /**
   * Turn the custom cursor feature on or off.
   *
   * When this is on, the cursor will change to a custom cursor
   * (default is a "pointing hand") whenever the cursor passes
   * over the label. By default, it is on.
   *
   * @param on whether a custom cursor should be displayed.
   * @param cursor is the custom cursor. @p 0L indicates the default "hand cursor".
   */
  void setUseCursor (bool on, QCursor* cursor = 0L);

  /**
   * Turn on or off the "glow" feature. 
   *
   * When this is on, the text will switch to the
   * selected color whenever the mouse
   * passes over it. By default, it is @p true.
   */
  void setGlow (bool glow = true);

  /**
   * Turn on or off the "float" feature.
   *
   * This feature is very similar to the "glow" feature in
   * that the color of the label switches to the selected 
   * color when the cursor passes over it. In addition,
   * underlining is turned on for as long as the mouse is overhead.
   * Note that if "glow" and underlining are both already turned
   * on, this feature will have no visible effect.
   * By default, it is @p false.
   */
  void setFloat (bool do_float = true);

  /**
   * Set the "alt" pixmap. 
   *
   * This pixmap will be displayed when the
   * cursor passes over the label.  The effect is similar to the
   * trick done with 'onMouseOver' in javascript.
   *
   * @see altPixmap()
   */
  void setAltPixmap (const QPixmap& altPix);

signals:

  /**
   * The mouse has passed over the label.
   *
   * @param url The URL for this label.
   */ 
  void enteredURL (const QString& url);

  /**
   * The mouse has passed over the label.
   */ 
  void enteredURL ();

  /**
   * The mouse is no longer over the label.
   *
   * @param url The URL for this label.
   */ 
  void leftURL (const QString& url);

  /**
   * The mouse is no longer over the label.
   */ 
  void leftURL ();

  /**
   * The user clicked the left mouse button on this label.
   *
   * @param url The URL for this label.
   */ 
  void leftClickedURL(const QString& url);
  
  /**
   * The user clicked the left mouse button on this label.
   */ 
  void leftClickedURL();
  
  /**
   * The user clicked the right mouse button on this label.
   *
   * @param url The URL for this label.
   */ 
  void rightClickedURL(const QString& url);
  
  /**
   * The user clicked the left mouse button on this label.
   */ 
  void rightClickedURL();
  
  /**
   * The user clicked the middle mouse button on this label.
   *
   * @param url The URL for this label.
   */ 
  void middleClickedURL(const QString& url);
  
  /**
   * The user clicked the left mouse button on this label.
   */ 
  void middleClickedURL();

protected:

  /**
   * Overridden for internal reasons; the API remains unaffected.
   */
  virtual void mousePressEvent (QMouseEvent*);

  /**
   * Overridden for internal reasons; the API remains unaffected.
   */
  virtual void mouseReleaseEvent (QMouseEvent*);

  /**
   * Overridden for internal reasons; the API remains unaffected.
   */
  virtual void enterEvent (QEvent*);

  /**
   * Overridden for internal reasons; the API remains unaffected.
   */
  virtual void leaveEvent (QEvent*);

private slots:
  /**
   * @internal
   * Slot used to reset the link-color to normal (timer-driven).
   */
  void updateColor ();

private:
  class Private;
  Private* d;

  /**
   * @internal
   * A private helper function to set the link-color to @p col.
   */
  void setLinkColor (const QColor& col);
};

#endif // KURLLABEL_H
