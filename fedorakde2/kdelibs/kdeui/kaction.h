/* This file is part of the KDE libraries
    Copyright (C) 1999 Reginald Stadlbauer <reggie@kde.org>
              (C) 1999 Simon Hausmann <hausmann@kde.org>
              (C) 2000 Nicolas Hadacek <haadcek@kde.org>
              (C) 2000 Kurt Granroth <granroth@kde.org>
              (C) 2000 Michael Koch <koch@kde.org>

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
//$Id$

#ifndef __kaction_h__
#define __kaction_h__

#include <qobject.h>
#include <qmap.h>
#include <kaccel.h>

class QMenuBar;
class QPopupMenu;
class QComboBox;
class QPoint;
class QIconSet;
class QString;

class KConfig;
class KURL;
class KInstance;
class KToolBar;
class KActionCollection;
class KPopupMenu;

/**
 * The KAction class (and derived and super classes) provides a way to
 * easily encapsulate a "real" user-selected action or event in your
 * program.
 *
 * For instance, a user may want to @p paste the contents of
 * the clipboard or @p scroll @p down a document or @p quit the
 * application.  These are all @p actions -- events that the
 * user causes to happen.  The KAction class allows the developer to
 * deal with these actions in an easy and intuitive manner.
 *
 * Specifically, the KAction class encapsulated the various attributes
 * to an event/action.  For instance, an action might have an icon
 * that goes along with it (a clipboard for a "paste" action or
 * scissors for a "cut" action).  The action might have some text to
 * describe the action.  It will certainly have a method or function
 * that actually @p executes the action!  All these attributes
 * are contained within the KAction object.
 *
 * The advantage of dealing with Actions is that you can manipulate
 * the Action without regard to the GUI representation of it.  For
 * instance, in the "normal" way of dealing with actions like "cut",
 * you would manually insert a item for Cut into a menu and a button
 * into a toolbar.  If you want to disable the cut action for a moment
 * (maybe nothing is selected), you woud have to hunt down the pointer
 * to the menu item and the toolbar button and disable both
 * individually.  Setting the menu item and toolbar item up uses very
 * similar code - but has to be done twice!
 *
 * With the Action concept, you simply "plug" the Action into whatever
 * GUI element you want.  The KAction class will then take care of
 * correctly defining the menu item (with icons, accelerators, text,
 * etc) or toolbar button.. or whatever.  From then on, if you
 * manipulate the Action at all, the effect will propogate through all
 * GUI representations of it.  Back to the "cut" example: if you want
 * to disable the Cut Action, you would simply do
 * 'cutAction->setEnabled(false)' and the menuitem and button would
 * instantly be disabled!
 *
 * This is the biggest advantage to the Action concept -- there is a
 * one-to-one relationship between the "real" action and @p all
 * GUI representations of it.
 *
 * @sect General Usage:
 *
 * The steps to using actions are roughly as follows
 *
 * @li Decide which attributes you want to associate with a given
 *     action (icons, text, keyboard accelerator, etc)
 * @li Create the action using KAction (or derived or super class).
 * @li "Plug" the Action into whatever GUI element you want.  Typically,
 *      this will be a menu or toolbar.
 *
 * @sect Detailed Example:
 *
 * Here is an example of enabling a "New [document]" action
 * <PRE>
 * KAction *newAct = new KAction(i18n("&New"), "filenew",
 *                               KStdAccel::key(KStdAccel::New), this,
 *                               SLOT(fileNew()), this);
 * </PRE>
 * This line creates our action.  It says that wherever this action is
 * displayed, it will use "&New" as the text, the standard icon, and
 * the standard accelerator.  It further says that whenever this action
 * is invoked, it will use the fileNew() slot to execute it.
 *
 * <PRE>
 * QPopupMenu *file = new QPopupMenu;
 * newAct->plug(file);
 * </PRE>
 * That just inserted the action into the File menu.  The point is, it's not
 * important in which menu it is: all manipulation of the item is
 * done through the newAct object.
 *
 * <PRE>
 * newAct->plug(toolBar());
 * </PRE>
 * And this inserted the Action into the main toolbar as a button.
 *
 * That's it!
 *
 * If you want to disable that action sometime later, you can do so
 * with
 * <PRE>
 * newAct->setEnabled(false)
 * </PRE>
 * and both the menuitem in File and the toolbar button will instantly
 * be disabled.
 *
 * Note: if you are using a "standard" action like "new", "paste",
 * "quit", or any other action described in the KDE UI Standards,
 * please use the methods in the @ref KStdAction class rather then
 * defining your own.
 *
 * @sect Usage Within the XML Framework:
 *
 * If you are using KAction within the context of the XML menu and
 * toolbar building framework, then there are a few tiny changes.  The
 * first is that you must insert your new action into an action
 * collection.  The action collection (a @ref KActionCollection) is,
 * logically enough, a central collection of all of the actions
 * defined in your application.  The XML UI framework code in KXMLGUI
 * classes needs access to this collection in order to build up the
 * GUI (it's how the builder code knows which actions are valid and
 * which aren't).
 *
 * Inserting your action into the collection is very simple.  To use a
 * previous example:
 *
 * <pre>
 * KAction *newAct = new KAction(i18n("&New"), "filenew",
 *                               KStdAccel::key(KStdAccel::New), this,
 *                               SLOT(fileNew()), actionCollection());
 * </pre>
 *
 * The only change is to use 'actionCollection()' as the parent of the
 * action.  That's it!
 *
 * Also, if you use the XML builder framework, then you do not ever
 * have to plug your actions into containers manually.  The framework
 * does that for you.
 *
 * @see KStdAction
 * @short Class to encapsulate user-driven action or event
 */
class KAction : public QObject
{
  friend class KActionCollection;
  Q_OBJECT
  Q_PROPERTY( int containerCount READ containerCount )
  Q_PROPERTY( QPixmap pixmap READ pixmap )
  Q_PROPERTY( QString plainText READ plainText )
  Q_PROPERTY( QString text READ text WRITE setText )
  Q_PROPERTY( int accel READ accel WRITE setAccel )
  Q_PROPERTY( bool enabled READ isEnabled WRITE setEnabled )
  Q_PROPERTY( QString group READ group WRITE setGroup )
  Q_PROPERTY( QString whatsThis READ whatsThis WRITE setWhatsThis )
  Q_PROPERTY( QString toolTip READ toolTip WRITE setToolTip )
  Q_PROPERTY( QString statusText READ statusText WRITE setStatusText )
  Q_PROPERTY( QIconSet iconSet READ iconSet WRITE setIconSet )
  Q_PROPERTY( QString icon READ icon WRITE setIcon )
public:
    /**
     * Construct an action with text and potential keyboard
     * accelerator but nothing else.  Use this only if you really
     * know what you are doing.
     *
     * @param text The text that will be displayed.
     * @param accel The corresponding keyboard accelerator (shortcut).
     * @param parent This action's parent.
     * @param name An internal name for this action.
     */
    KAction( const QString& text, int accel = 0, QObject* parent = 0, const char* name = 0 );

    /**
     * Construct an action with text, potential keyboard
     * accelerator, and a SLOT to call when this action is invoked by
     * the user.
     *
     * If you do not want or have a keyboard accelerator,
     * set the @p accel param to 0.
     *
     * This is the most common KAction used when you do not have a
     * corresponding icon (note that it won't appear in the current version
     * of the "Edit ToolBar" dialog, because an action needs an icon to be
     * plugged in a toolbar...).
     *
     * @param text The text that will be displayed.
     * @param accel The corresponding keyboard accelerator (shortcut).
     * @param receiver The SLOT's parent.
     * @param slot The SLOT to invoke to execute this action.
     * @param parent This action's parent.
     * @param name An internal name for this action.
     */
    KAction( const QString& text, int accel,
             const QObject* receiver, const char* slot, QObject* parent, const char* name = 0 );

    /**
     * Construct an action with text, icon, and a potential keyboard
     * accelerator.
     *
     * This Action cannot execute any command.  Use this only if you
     * really know what you are doing.
     *
     * @param text The text that will be displayed.
     * @param pix The icons that go with this action.
     * @param accel The corresponding keyboard accelerator (shortcut).
     * @param parent This action's parent.
     * @param name An internal name for this action.
     */
    KAction( const QString& text, const QIconSet& pix, int accel = 0,
             QObject* parent = 0, const char* name = 0 );

    /**
     * Construct an action with text, automatically loaded icon, and a
     * potential keyboard accelerator.
     *
     * This Action cannot execute any command.  Use this only if you
     * really know what you are doing.
     *
     * @param text The text that will be displayed.
     * @param pix The icons that go with this action.
     * @param accel The corresponding keyboard accelerator (shortcut).
     * @param parent This action's parent.
     * @param name An internal name for this action.
     */
    KAction( const QString& text, const QString& pix, int accel = 0,
             QObject* parent = 0, const char* name = 0 );

    /**
     * Construct an action with text, icon, potential keyboard
     * accelerator, and a SLOT to call when this action is invoked by
     * the user.
     *
     * If you do not want or have a keyboard accelerator, set the
     * @p accel param to 0.
     *
     * This is the other common KAction used.  Use it when you
     * @p do have a corresponding icon.
     *
     * @param text The text that will be displayed.
     * @param pix The icon to display.
     * @param accel The corresponding keyboard accelerator (shortcut).
     * @param receiver The SLOT's parent.
     * @param slot The SLOT to invoke to execute this action.
     * @param parent This action's parent.
     * @param name An internal name for this action.
     */
    KAction( const QString& text, const QIconSet& pix, int accel,
             const QObject* receiver, const char* slot, QObject* parent, const char* name = 0 );

    /**
     * Construct an action with text, icon, potential keyboard
     * accelerator, and a SLOT to call when this action is invoked by
     * the user.  The icon is loaded on demand later based on where it
     * is plugged in.
     *
     * If you do not want or have a keyboard accelerator, set the
     * @p accel param to 0.
     *
     * This is the other common KAction used.  Use it when you
     * @p do have a corresponding icon.
     *
     * @param text The text that will be displayed.
     * @param pix The icon to display.
     * @param accel The corresponding keyboard accelerator (shortcut).
     * @param receiver The SLOT's parent.
     * @param slot The SLOT to invoke to execute this action.
     * @param parent This action's parent.
     * @param name An internal name for this action.
     */
    KAction( const QString& text, const QString& pix, int accel,
             const QObject* receiver, const char* slot, QObject* parent,
         const char* name = 0 );

    /**
     * Construct a null action.
     * This is not recommended since all actions should have a text,
     * for the "Edit ToolBar" dialog. So don't forget to call setText later :)
     *
     * @param parent This action's parent.
     * @param name An internal name for this action.
     */
    KAction( QObject* parent = 0, const char* name = 0 );

    /**
     * Standard destructor
     */
    virtual ~KAction();

    /**
     * "Plug" or insert this action into a given widget.
     *
     * This will
     * typically be a menu or a toolbar.  From this point on, you will
     * never need to directly manipulate the item in the menu or
     * toolbar.  You do all enabling/disabling/manipulation directly
     * with your KAction object.
     *
     * @param w The GUI element to display this action
     */
    virtual int plug( QWidget *w, int index = -1 );

    /**
     * "Plug" or insert this action into a given KAccel.
     *
     * It is sometimes useful to use the action paradigm for
     * actions that are not associated with some widget (ie actions
     * that are only activated by keyboard).
     *
     * @param accel The KAccel which activates this action
     * @param configurable If the accelerator is configurable via
     * the KAccel configuration dialog (this is somehow deprecated since
     * there is now a KAction key configuration dialog).
     */
    virtual void plugAccel(KAccel *accel, bool configurable = true);

    /**
     * "Unplug" or remove this action from a given widget.
     *
     * This will typically be a menu or a toolbar.  This is rarely
     * used in "normal" application.  Typically, it would be used if
     * your application has several views or modes, each with a
     * completely different menu structure.  If you simply want to
     * disable an action for a given period, use @ref setEnabled()
     * instead.
     *
     * @param w Remove the action from this GUI element.
     */
    virtual void unplug( QWidget *w );

    /**
     * Disconnect this action from the KAccel.
     */
    virtual void unplugAccel();

    /**
     * returns whether the action is plugged into any container widget or not.
     */
    virtual bool isPlugged() const;

    /**
     * returns whether the action is plugged into the given container with the given, container specific, id (often
     * menu or toolbar id ) .
     */
    virtual bool isPlugged( const QWidget *container, int id ) const;

    /**
     * returns whether the action is plugged into the given container with the given, container specific, representative
     * container widget item.
     */
    virtual bool isPlugged( const QWidget *container, const QWidget *_representative ) const;

    QWidget* container( int index ) const;
    int itemId( int index ) const;
    QWidget* representative( int index ) const;
    int containerCount() const;

    virtual QPixmap pixmap() const;

    virtual bool hasIconSet() const;
    virtual QString plainText() const;

    /**
     * Get the text associated with this action.
     */
    virtual QString text() const;

    /**
     * Get the keyboard accelerator associated with this action.
     */
    virtual int accel() const;

    /** Returns true if this action is enabled. */
    virtual bool isEnabled() const;

    virtual QString group() const;

    /**
     * Get the What's this text for the action.
     */
    virtual QString whatsThis() const;

    /**
     * Get the tooltip text for the action.
     */
    virtual QString toolTip() const;

    virtual QString statusText() const;

    /**
     * Get the QIconSet from which the icons used to display this action will
     * be chosen.
     */
    virtual QIconSet iconSet() const;

    virtual QString icon() const;

    KActionCollection *parentCollection() const;

    /**
     * @internal
     * Generate a toolbar button id. Made public for reimplementations.
     */
    static int getToolButtonID();


    void unplugAll();

public slots:
    /**
     * Set the text associated with this action. The text is used for menu
     * and toolbar labels etc.
     */
    virtual void setText(const QString &text);

    /**
     * Set the keyboard accelerator associated with this action.
     */
    virtual void setAccel(int a);

    virtual void setGroup( const QString& );

    /**
     * Set the What's this text for the action. This text will be displayed when
     * a widget that has been created by plugging this action into a container
     * is clicked on in What's this mode.
     *
     * The What's this text can include QML markup as well as raw text.
     */
    virtual void setWhatsThis( const QString& text );

    /**
     * Set the tooltip text for the action.
     * This will be used as a tooltip for a toolbar button, as a
     * statusbar help-text for a menu item, and it also appears
     * in the toolbar editor, to describe the action.
     */
    virtual void setToolTip( const QString& );

    /**
     * Set the QIconSet from which the icons used to display this action will
     * be chosen.
     */
    virtual void setIconSet( const QIconSet &iconSet );

    virtual void setIcon( const QString& icon );

    /**
     * @deprecated. Use setToolTip instead (they do the same thing now).
     */
    virtual void setStatusText( const QString &text );

    /**
     * Enables or disables this action. All uses of this action (eg. in menus
     * or toolbars) will be updated to reflect the state of the action.
     */
    virtual void setEnabled(bool enable);

    /**
     * Emulate user's interaction programmatically, by activating the action.
     * The implementation simply emits activated().
     */
    virtual void activate();

protected slots:
    virtual void slotDestroyed();
    virtual void slotKeycodeChanged();
    virtual void slotActivated();

protected:
    KToolBar* toolBar( int index ) const;
    QPopupMenu* popupMenu( int index ) const;
    void removeContainer( int index );
    int findContainer( const QWidget* widget ) const;
    void plugMainWindowAccel( QWidget *w );

    void addContainer( QWidget* parent, int id );
    void addContainer( QWidget* parent, QWidget* representative );

    virtual void setAccel( int id, int accel );
    virtual void setGroup( int id, const QString& grp );
    virtual void setText(int i, const QString &text);
    virtual void setEnabled(int i, bool enable);
    virtual void setIconSet(int i, const QIconSet &iconSet);
    virtual void setIcon( int i, const QString& icon );
    virtual void setToolTip( int id, const QString& tt );
    /// To be removed in KDE 3.0
    virtual void setStatusText( int id, const QString &text );
    virtual void setWhatsThis( int id, const QString& text );

    /**
       * for backwards compatibility. deprecated!
       */
    int menuId( int i ) { return itemId( i ); }

    KActionCollection *m_parentCollection;

signals:
    void activated();
    void enabled( bool );

private:
    class KActionPrivate;
    KActionPrivate *d;
};

/**
 *  Checkbox like action.
 *
 *  This action provides two states: checked or not.
 *
 *  @short Checkbox like action.
 */
class KToggleAction : public KAction
{
    Q_OBJECT
    Q_PROPERTY( bool checked READ isChecked WRITE setChecked )
    Q_PROPERTY( QString exclusiveGroup READ exclusiveGroup WRITE setExclusiveGroup )
public:

    /**
     * Construct a toggle action with text and potential keyboard
     * accelerator but nothing else. Use this only if you really
     * know what you are doing.
     *
     * @param text The text that will be displayed.
     * @param accel The corresponding keyboard accelerator (shortcut).
     * @param parent This action's parent.
     * @param name An internal name for this action.
     */
    KToggleAction( const QString& text, int accel = 0, QObject* parent = 0, const char* name = 0 );

    /**
     *  @param text The text that will be displayed.
     *  @param accel The corresponding keyboard accelerator (shortcut).
     *  @param receiver The SLOT's parent.
     *  @param slot The SLOT to invoke to execute this action.
     *  @param parent This action's parent.
     *  @param name An internal name for this action.
     */
    KToggleAction( const QString& text, int accel,
                   const QObject* receiver, const char* slot, QObject* parent, const char* name = 0 );

    /**
     *  @param text The text that will be displayed.
     *  @param pix The icons that go with this action.
     *  @param accel The corresponding keyboard accelerator (shortcut).
     *  @param parent This action's parent.
     *  @param name An internal name for this action.
     */
    KToggleAction( const QString& text, const QIconSet& pix, int accel = 0,
             QObject* parent = 0, const char* name = 0 );

    /**
     *  @param text The text that will be displayed.
     *  @param pix The dynamically loaded icon that goes with this action.
     *  @param accel The corresponding keyboard accelerator (shortcut).
     *  @param parent This action's parent.
     *  @param name An internal name for this action.
     */
    KToggleAction( const QString& text, const QString& pix, int accel = 0,
                   QObject* parent = 0, const char* name = 0 );

    /**
     *  @param text The text that will be displayed.
     *  @param pix The icons that go with this action.
     *  @param accel The corresponding keyboard accelerator (shortcut).
     *  @param receiver The SLOT's parent.
     *  @param slot The SLOT to invoke to execute this action.
     *  @param parent This action's parent.
     *  @param name An internal name for this action.
     */
    KToggleAction( const QString& text, const QIconSet& pix, int accel,
                   const QObject* receiver, const char* slot, QObject* parent, const char* name = 0 );

    /**
     *  @param text The text that will be displayed.
     *  @param pix The dynamically loaded icon that goes with this action.
     *  @param accel The corresponding keyboard accelerator (shortcut).
     *  @param receiver The SLOT's parent.
     *  @param slot The SLOT to invoke to execute this action.
     *  @param parent This action's parent.
     *  @param name An internal name for this action.
     */
    KToggleAction( const QString& text, const QString& pix, int accel,
                   const QObject* receiver, const char* slot,
                   QObject* parent, const char* name = 0 );

    /**
     *  @param parent This action's parent.
     *  @param name An internal name for this action.
     */
    KToggleAction( QObject* parent = 0, const char* name = 0 );

    /**
     * Destructor
     */
    virtual ~KToggleAction();

    /**
     *  "Plug" or insert this action into a given widget.
     *
     *  This will typically be a menu or a toolbar.  From this point
     *  on, you will never need to directly manipulate the item in the
     *  menu or toolbar.  You do all enabling/disabling/manipulation
     *  directly with your KToggleAction object.
     *
     *  @param widget The GUI element to display this action.
     *  @param index  The index of the item.
     */
    virtual int plug( QWidget*, int index = -1 );

    /**
     *  Retrieves the actual state of the action.
     */
    bool isChecked() const;

    /**
     * @return which "exclusive group" this action is part of.
     * @see setExclusiveGroup
     */
    virtual QString exclusiveGroup() const;

public slots:

    /**
     * Defines which "exclusive group" this action is part of.
     * In a given exclusive group, only one toggle action can be checked
     * at a any moment. Checking an action unchecks the other actions
     * of the group.
     */
    virtual void setExclusiveGroup( const QString& name );
    // why virtual? why slot? (change in KDE 3?)

    /**
     *  Sets the state of the action.
     */
    virtual void setChecked( bool );

protected slots:
    virtual void slotActivated();

protected:
    virtual void setChecked( int id, bool checked );

signals:
    void toggled( bool );

private:
    class KToggleActionPrivate;
    KToggleActionPrivate *d;
};

/**
 * An action that operates like a radio button. At any given time
 * only a single action from the group will be active.
 */
class KRadioAction : public KToggleAction
{
  Q_OBJECT
public:
    /**
     * Construct a radio action with text and potential keyboard
     * accelerator but nothing else. Use this only if you really
     * know what you are doing.
     *
     * @param text The text that will be displayed.
     * @param accel The corresponding keyboard accelerator (shortcut).
     * @param parent This action's parent.
     * @param name An internal name for this action.
     */
    KRadioAction( const QString& text, int accel = 0, QObject* parent = 0, const char* name = 0 );

    /**
     *  @param text The text that will be displayed.
     *  @param accel The corresponding keyboard accelerator (shortcut).
     *  @param receiver The SLOT's parent.
     *  @param slot The SLOT to invoke to execute this action.
     *  @param parent This action's parent.
     *  @param name An internal name for this action.
     */
    KRadioAction( const QString& text, int accel,
                  const QObject* receiver, const char* slot, QObject* parent, const char* name = 0 );

    /**
     *  @param text The text that will be displayed.
     *  @param pix The icons that go with this action.
     *  @param accel The corresponding keyboard accelerator (shortcut).
     *  @param parent This action's parent.
     *  @param name An internal name for this action.
     */
    KRadioAction( const QString& text, const QIconSet& pix, int accel = 0,
                  QObject* parent = 0, const char* name = 0 );

    /**
     *  @param text The text that will be displayed.
     *  @param pix The dynamically loaded icon that goes with this action.
     *  @param accel The corresponding keyboard accelerator (shortcut).
     *  @param parent This action's parent.
     *  @param name An internal name for this action.
     */
    KRadioAction( const QString& text, const QString& pix, int accel = 0,
                  QObject* parent = 0, const char* name = 0 );

    /**
     *  @param text The text that will be displayed.
     *  @param pix The icons that go with this action.
     *  @param accel The corresponding keyboard accelerator (shortcut).
     *  @param receiver The SLOT's parent.
     *  @param slot The SLOT to invoke to execute this action.
     *  @param parent This action's parent.
     *  @param name An internal name for this action.
     */
    KRadioAction( const QString& text, const QIconSet& pix, int accel,
                  const QObject* receiver, const char* slot, QObject* parent, const char* name = 0 );

    /**
     *  @param text The text that will be displayed.
     *  @param pix The dynamically loaded icon that goes with this action.
     *  @param accel The corresponding keyboard accelerator (shortcut).
     *  @param receiver The SLOT's parent.
     *  @param slot The SLOT to invoke to execute this action.
     *  @param parent This action's parent.
     *  @param name An internal name for this action.
     */
    KRadioAction( const QString& text, const QString& pix, int accel,
                  const QObject* receiver, const char* slot,
                  QObject* parent, const char* name = 0 );

    /**
     *  @param parent This action's parent.
     *  @param name An internal name for this action.
     */
    KRadioAction( QObject* parent = 0, const char* name = 0 );

protected:
    virtual void slotActivated();

private:
    class KRadioActionPrivate;
    KRadioActionPrivate *d;
};

/**
 *  Action for selecting one of several items.
 *
 *  This action shows up a submenu with a list of items.
 *  One of them can be checked. If the user clicks on an item
 *  this item will automatically be checked,
 *  the formerly checked item becomes unchecked.
 *  There can be only one item checked at a time.
 *
 *  @short Action for selecting one of several items
 */
class KSelectAction : public KAction
{
    Q_OBJECT
    Q_PROPERTY( int currentItem READ currentItem WRITE setCurrentItem )
    Q_PROPERTY( QStringList items READ items WRITE setItems )
    Q_PROPERTY( bool editable READ isEditable WRITE setEditable )
    Q_PROPERTY( int comboWidth READ comboWidth WRITE setComboWidth )
    Q_PROPERTY( QString currentText READ currentText )
public:

    /**
     * Construct a select action with text and potential keyboard
     * accelerator but nothing else. Use this only if you really
     * know what you are doing.
     *
     * @param text The text that will be displayed.
     * @param accel The corresponding keyboard accelerator (shortcut).
     * @param parent This action's parent.
     * @param name An internal name for this action.
     */
    KSelectAction( const QString& text, int accel = 0, QObject* parent = 0, const char* name = 0 );

    /**
     *  @param text The text that will be displayed.
     *  @param accel The corresponding keyboard accelerator (shortcut).
     *  @param receiver The SLOT's parent.
     *  @param slot The SLOT to invoke to execute this action.
     *  @param parent This action's parent.
     *  @param name An internal name for this action.
     */
    KSelectAction( const QString& text, int accel,
                   const QObject* receiver, const char* slot, QObject* parent, const char* name = 0 );

    /**
     *  @param text The text that will be displayed.
     *  @param pix The icons that go with this action.
     *  @param accel The corresponding keyboard accelerator (shortcut).
     *  @param parent This action's parent.
     *  @param name An internal name for this action.
     */
    KSelectAction( const QString& text, const QIconSet& pix, int accel = 0,
             QObject* parent = 0, const char* name = 0 );

    /**
     *  @param text The text that will be displayed.
     *  @param pix The dynamically loaded icon that goes with this action.
     *  @param accel The corresponding keyboard accelerator (shortcut).
     *  @param parent This action's parent.
     *  @param name An internal name for this action.
     */
    KSelectAction( const QString& text, const QString& pix, int accel = 0,
                   QObject* parent = 0, const char* name = 0 );

    /**
     *  @param text The text that will be displayed.
     *  @param pix The icons that go with this action.
     *  @param accel The corresponding keyboard accelerator (shortcut).
     *  @param receiver The SLOT's parent.
     *  @param slot The SLOT to invoke to execute this action.
     *  @param parent This action's parent.
     *  @param name An internal name for this action.
     */
    KSelectAction( const QString& text, const QIconSet& pix, int accel,
                   const QObject* receiver, const char* slot, QObject* parent, const char* name = 0 );

    /**
     *  @param text The text that will be displayed.
     *  @param pix The dynamically loaded icon that goes with this action.
     *  @param accel The corresponding keyboard accelerator (shortcut).
     *  @param receiver The SLOT's parent.
     *  @param slot The SLOT to invoke to execute this action.
     *  @param parent This action's parent.
     *  @param name An internal name for this action.
     */
    KSelectAction( const QString& text, const QString& pix, int accel,
                   const QObject* receiver, const char* slot,
                   QObject* parent, const char* name = 0 );

    /**
     *  @param parent This action's parent.
     *  @param name An internal name for this action.
     */
    KSelectAction( QObject* parent = 0, const char* name = 0 );

    /**
     * Destructor
     */
    virtual ~KSelectAction();

    /**
     *  "Plug" or insert this action into a given widget.
     *
     *  This will typically be a menu or a toolbar.
     *  From this point on, you will never need to directly
     *  manipulate the item in the menu or toolbar.
     *  You do all enabling/disabling/manipulation directly with your KSelectAction object.
     *
     *  @param widget The GUI element to display this action.
     *  @param index  The index of the item.
     */
    virtual int plug( QWidget* widget, int index = -1 );

    /**
     * When this action is plugged into a toolbar, it creates a combobox.
     * @return true if the combo editable.
     */
    virtual bool isEditable() const;

    /**
     * @return the items that can be selected with this action.
     * Use setItems to set them.
     */
    virtual QStringList items() const;

    virtual void changeItem( int index, const QString& text );

    virtual QString currentText() const;

    virtual int currentItem() const;

    /**
     * When this action is plugged into a toolbar, it creates a combobox.
     * This returns the maximum width set by setComboWidth
     */
    virtual int comboWidth() const;

    QPopupMenu* popupMenu();

public slots:
    /**
     *  Sets the currently checked item.
     *
     *  @param index Index of the item (remember the first item is zero).
     */
    virtual void setCurrentItem( int index );

    /**
     * Sets the items to be displayed in this action
     * You need to call this.
     */
    virtual void setItems( const QStringList &lst );

    /**
     * Clears up all the items in this action
     */
    virtual void clear();

    /**
     * When this action is plugged into a toolbar, it creates a combobox.
     * This makes the combo editable or read-only.
     */
    virtual void setEditable( bool );

    /**
     * When this action is plugged into a toolbar, it creates a combobox.
     * This gives a _maximum_ size to the combobox.
     * The minimum size is automatically given by the contents (the items).
     */
    virtual void setComboWidth( int width );

protected:
    virtual void changeItem( int id, int index, const QString& text );

protected slots:
    virtual void slotActivated( int id );
    virtual void slotActivated( const QString &text );
    virtual void slotActivated() { KAction::slotActivated(); }

signals:
    void activated( int index );
    void activated( const QString& text );

protected:
    virtual void setCurrentItem( int id, int index );

    virtual void setComboWidth( int id, int width );

    virtual void setItems( int id, const QStringList &lst );

    virtual void clear( int id );

private:
    class KSelectActionPrivate;
    KSelectActionPrivate *d;

};

class KListAction : public KSelectAction
{
    Q_OBJECT
public:
    /**
     * Construct a list action with text and potential keyboard
     * accelerator but nothing else. Use this only if you really
     * know what you are doing.
     *
     * @param text The text that will be displayed.
     * @param accel The corresponding keyboard accelerator (shortcut).
     * @param parent This action's parent.
     * @param name An internal name for this action.
     */
    KListAction( const QString& text, int accel = 0, QObject* parent = 0,
                  const char* name = 0 );

    /**
     *  @param text The text that will be displayed.
     *  @param accel The corresponding keyboard accelerator (shortcut).
     *  @param receiver The SLOT's parent.
     *  @param slot The SLOT to invoke to execute this action.
     *  @param parent This action's parent.
     *  @param name An internal name for this action.
     */
    KListAction( const QString& text, int accel, const QObject* receiver,
                  const char* slot, QObject* parent, const char* name = 0 );

    /**
     *  @param text The text that will be displayed.
     *  @param pix The icons that go with this action.
     *  @param accel The corresponding keyboard accelerator (shortcut).
     *  @param parent This action's parent.
     *  @param name An internal name for this action.
     */
    KListAction( const QString& text, const QIconSet& pix, int accel = 0,
                      QObject* parent = 0, const char* name = 0 );

    /**
     *  @param text The text that will be displayed.
     *  @param pix The dynamically loaded icon that goes with this action.
     *  @param accel The corresponding keyboard accelerator (shortcut).
     *  @param parent This action's parent.
     *  @param name An internal name for this action.
     */
    KListAction( const QString& text, const QString& pix, int accel = 0,
                      QObject* parent = 0, const char* name = 0 );

    /**
     *  @param text The text that will be displayed.
     *  @param pix The icons that go with this action.
     *  @param accel The corresponding keyboard accelerator (shortcut).
     *  @param receiver The SLOT's parent.
     *  @param slot The SLOT to invoke to execute this action.
     *  @param parent This action's parent.
     *  @param name An internal name for this action.
     */
    KListAction( const QString& text, const QIconSet& pix, int accel,
                          const QObject* receiver, const char* slot, QObject* parent,
                  const char* name = 0 );

    /**
     *  @param text The text that will be displayed.
     *  @param pix The dynamically loaded icon that goes with this action.
     *  @param accel The corresponding keyboard accelerator (shortcut).
     *  @param receiver The SLOT's parent.
     *  @param slot The SLOT to invoke to execute this action.
     *  @param parent This action's parent.
     *  @param name An internal name for this action.
     */
    KListAction( const QString& text, const QString& pix, int accel,
                 const QObject* receiver, const char* slot, QObject* parent,
                 const char* name = 0 );

    /**
     *  @param parent This action's parent.
     *  @param name An internal name for this action.
     */
    KListAction( QObject* parent = 0, const char* name = 0 );

    /**
     * Destructor
     */
    virtual ~KListAction();


    virtual QString currentText() const;
    virtual int currentItem() const;


public slots:
    /**
     *  Sets the currently checked item.
     *
     *  @param index Index of the item (remember the first item is zero).
     */
    virtual void setCurrentItem( int index );

    /// @internal
    virtual void setCurrentItem(int a, int b) { KSelectAction::setCurrentItem(a, b); }

private:

    class KListActionPrivate;
    KListActionPrivate *d;
};

/**
 *  This class is an action to handle a recent files submenu.
 *  The best way to create the action is to use KStdAction::openRecent.
 *  Then you simply need to call @ref loadEntries on startup, @ref saveEntries
 *  on shutdown, @ref addURL when your application loads/saves a file.
 *
 *  @author Michael Koch
 *  @short Recent files action
 */
class KRecentFilesAction : public KListAction
{
  Q_OBJECT
  Q_PROPERTY( uint maxItems READ maxItems WRITE setMaxItems )
public:
  /**
   *  @param text The text that will be displayed.
   *  @param accel The corresponding keyboard accelerator (shortcut).
   *  @param parent This action's parent.
   *  @param name An internal name for this action.
   *  @param maxItems The maximum number of files to display
   */
  KRecentFilesAction( const QString& text, int accel,
                      QObject* parent, const char* name = 0,
                      uint maxItems = 10 );

  /**
   *  @param text The text that will be displayed.
   *  @param accel The corresponding keyboard accelerator (shortcut).
   *  @param receiver The SLOT's parent.
   *  @param slot The SLOT to invoke when a URL is selected.
   *  Its signature is of the form slotURLSelected( const KURL & ).
   *  @param parent This action's parent.
   *  @param name An internal name for this action.
   *  @param maxItems The maximum number of files to display
   */
  KRecentFilesAction( const QString& text, int accel,
                      const QObject* receiver, const char* slot,
                      QObject* parent, const char* name = 0,
                      uint maxItems = 10 );

  /**
   *  @param text The text that will be displayed.
   *  @param pix The icons that go with this action.
   *  @param accel The corresponding keyboard accelerator (shortcut).
   *  @param parent This action's parent.
   *  @param name An internal name for this action.
   *  @param maxItems The maximum number of files to display
   */
  KRecentFilesAction( const QString& text, const QIconSet& pix, int accel,
                      QObject* parent, const char* name = 0,
                      uint maxItems = 10 );

  /**
   *  @param text The text that will be displayed.
   *  @param pix The dynamically loaded icon that goes with this action.
   *  @param accel The corresponding keyboard accelerator (shortcut).
   *  @param parent This action's parent.
   *  @param name An internal name for this action.
   *  @param maxItems The maximum number of files to display
   */
  KRecentFilesAction( const QString& text, const QString& pix, int accel,
                      QObject* parent, const char* name = 0,
                      uint maxItems = 10 );

  /**
   *  @param text The text that will be displayed.
   *  @param pix The icons that go with this action.
   *  @param accel The corresponding keyboard accelerator (shortcut).
   *  @param receiver The SLOT's parent.
   *  @param slot The SLOT to invoke when a URL is selected.
   *  Its signature is of the form slotURLSelected( const KURL & ).
   *  @param parent This action's parent.
   *  @param name An internal name for this action.
   *  @param maxItems The maximum number of files to display
   */
  KRecentFilesAction( const QString& text, const QIconSet& pix, int accel,
                      const QObject* receiver, const char* slot,
                      QObject* parent, const char* name = 0,
                      uint maxItems = 10 );

  /**
   *  @param text The text that will be displayed.
   *  @param pix The dynamically loaded icon that goes with this action.
   *  @param accel The corresponding keyboard accelerator (shortcut).
   *  @param receiver The SLOT's parent.
   *  @param slot The SLOT to invoke when a URL is selected.
   *  Its signature is of the form slotURLSelected( const KURL & ).
   *  @param parent This action's parent.
   *  @param name An internal name for this action.
   *  @param maxItems The maximum number of files to display
   */
  KRecentFilesAction( const QString& text, const QString& pix, int accel,
                      const QObject* receiver, const char* slot,
                      QObject* parent, const char* name = 0,
                      uint maxItems = 10 );

  /**
   *  @param parent This action's parent.
   *  @param name An internal name for this action.
   *  @param maxItems The maximum number of files to display
   */
  KRecentFilesAction( QObject* parent = 0, const char* name = 0,
                      uint maxItems = 10 );

  /**
   *  Destructor.
   */
  virtual ~KRecentFilesAction();

public slots:
  /**
   *  Retrieves the maximum of items in the recent files list.
   */
  uint maxItems() const;

  /**
   *  Sets the maximum of items in the recent files list.
   *  The default for this value is 10 set in the constructor.
   *
   *  If this value is lesser than the number of items currently
   *  in the recent files list the last items are deleted until
   *  the number of items are equal to the new maximum.
   */
  void setMaxItems( uint maxItems );

  /**
   *  Loads the recent files entries from a given KConfig object.
   *  You can provide the name of the group used to load the entries.
   *  If the groupname is empty, entries are load from a group called 'RecentFiles'
   *
   *  This method does not effect the active group of KConfig.
   */
  void loadEntries( KConfig* config, QString groupname=QString::null );

  /**
   *  Saves the current recent files entries to a given KConfig object.
   *  You can provide the name of the group used to load the entries.
   *  If the groupname is empty, entries are saved to a group called 'RecentFiles'
   *
   *  This method does not effect the active group of KConfig.
   */
  void saveEntries( KConfig* config, QString groupname=QString::null );

public slots:
  /**
   *  Add URL to recent files list.
   *
   *  @param url The URL of the file
   */
  void addURL( const KURL& url );

  /**
   *  Remove an URL from the recent files list.
   *
   *  @param url The URL of the file
   */
  void removeURL( const KURL& url );

  /**
   *  Removes all entries from the recent files list.
   */
  void clearURLList();

signals:

  /**
   *  This signal gets emited when the user selects an URL.
   *
   *  @param url The URL thats the user selected.
   */
  void urlSelected( const KURL& url );

protected slots:
  /**
   *
   */
  void itemSelected( const QString& string );

private:
  class KRecentFilesActionPrivate;
  KRecentFilesActionPrivate *d;
};

class KFontAction : public KSelectAction
{
    Q_OBJECT
    Q_PROPERTY( QString font READ font WRITE setFont )
public:
    KFontAction( const QString& text, int accel = 0, QObject* parent = 0,
                 const char* name = 0 );
    KFontAction( const QString& text, int accel,
                 const QObject* receiver, const char* slot, QObject* parent,
                 const char* name = 0 );
    KFontAction( const QString& text, const QIconSet& pix, int accel = 0,
                 QObject* parent = 0, const char* name = 0 );
    KFontAction( const QString& text, const QString& pix, int accel = 0,
                 QObject* parent = 0, const char* name = 0 );
    KFontAction( const QString& text, const QIconSet& pix, int accel,
                 const QObject* receiver, const char* slot, QObject* parent,
                 const char* name = 0 );
    KFontAction( const QString& text, const QString& pix, int accel,
                 const QObject* receiver, const char* slot, QObject* parent,
                 const char* name = 0 );

    KFontAction( QObject* parent = 0, const char* name = 0 );
    ~KFontAction();

    QString font() const {
        return currentText();
    }

    int plug( QWidget*, int index = -1 );

public slots:
    void setFont( const QString &family );

private:
    class KFontActionPrivate;
    KFontActionPrivate *d;
};

class KFontSizeAction : public KSelectAction
{
    Q_OBJECT
    Q_PROPERTY( int fontSize READ fontSize WRITE setFontSize )
public:
    KFontSizeAction( const QString& text, int accel = 0, QObject* parent = 0,
                     const char* name = 0 );
    KFontSizeAction( const QString& text, int accel, const QObject* receiver,
                     const char* slot, QObject* parent, const char* name = 0 );
    KFontSizeAction( const QString& text, const QIconSet& pix, int accel = 0,
                     QObject* parent = 0, const char* name = 0 );
    KFontSizeAction( const QString& text, const QString& pix, int accel = 0,
                     QObject* parent = 0, const char* name = 0 );
    KFontSizeAction( const QString& text, const QIconSet& pix, int accel,
                     const QObject* receiver, const char* slot,
                     QObject* parent, const char* name = 0 );
    KFontSizeAction( const QString& text, const QString& pix, int accel,
                     const QObject* receiver, const char* slot,
                     QObject* parent, const char* name = 0 );
    KFontSizeAction( QObject* parent = 0, const char* name = 0 );

    virtual ~KFontSizeAction();

    virtual int fontSize() const;

public slots:
    virtual void setFontSize( int size );

protected slots:
    virtual void slotActivated( int );
    virtual void slotActivated( const QString& );
    virtual void slotActivated() { KAction::slotActivated(); }

signals:
    void fontSizeChanged( int );

private:
    void init();


    class KFontSizeActionPrivate;
    KFontSizeActionPrivate *d;
};

/**
 * A KActionMenu is an action that holds a sub-menu of other actions.
 * insert() and remove() allow to insert and remove actions into this action-menu.
 * Plugged in a popupmenu, it will create a submenu.
 * Plugged in a toolbar, it will create a button with a popup menu.
 *
 * This is the action used by the XMLGUI since it holds other actions.
 * If you want a submenu for selecting one tool among many (without icons), see KSelectAction.
 * See also setDelayed about the main action.
 */
class KActionMenu : public KAction
{
  Q_OBJECT
  Q_PROPERTY( bool delayed READ delayed WRITE setDelayed )
  Q_PROPERTY( bool stickyMenu READ stickyMenu WRITE setStickyMenu )

public:
    KActionMenu( const QString& text, QObject* parent = 0,
                 const char* name = 0 );
    KActionMenu( const QString& text, const QIconSet& icon,
                 QObject* parent = 0, const char* name = 0 );
    KActionMenu( const QString& text, const QString& icon,
                 QObject* parent = 0, const char* name = 0 );
    KActionMenu( QObject* parent = 0, const char* name = 0 );
    virtual ~KActionMenu();

    virtual void insert( KAction*, int index = -1 );
    virtual void remove( KAction* );

    KPopupMenu* popupMenu();
    void popup( const QPoint& global );

    /**
       Returns true if this action creates a delayed popup menu
       when plugged in a KToolbar.
     */
    bool delayed() const;
    /**
       If set to true, this action will create a delayed popup menu
       when plugged in a KToolbar. Otherwise it creates a normal popup.
       Default: delayed

       Remember that if the "main" action (the toolbar button itself)
       cannot be clicked, then you should call setDelayed(false).

       On the opposite, if the main action can be clicked, it can only happen
       in a toolbar: in a menu, the parent of a submenu can't be activated.
       To get a "normal" menu item when plugged a menu (and no submenu)
       use KToolBarPopupAction.
    */
    void setDelayed(bool _delayed);

    /**
     * Returns true if this action creates a sticky popup menu.
     * See @ref setStickyMenu.
     */
    bool stickyMenu() const;
    /**
     * If set to true, this action will create a sticky popup menu
     * when plugged in a KToolbar.
     * "Sticky", means it's visible until a selection is made or the mouse is
     * clicked elsewhere. This feature allows you to make a selection without
     * having to press and hold down the mouse while making a selection.
     * Default: sticky.
     */
    void setStickyMenu(bool sticky);

    virtual int plug( QWidget* widget, int index = -1 );
    virtual void unplug( QWidget* widget );

public slots:
    virtual void setEnabled( bool b );
protected:
    virtual void setEnabled( int i, bool enable );
    virtual void setText( int id, const QString& text );
    virtual void setIconSet( int id, const QIconSet& iconSet );

    /// @internal
    virtual void setIconSet(const QIconSet &i) { KAction::setIconSet(i); }
    /// @internal
    virtual void setText(const QString& s) { KAction::setText(s); }
private:

    class KActionMenuPrivate;
    KActionMenuPrivate *d;
};

/**
 * This action is a normal action everywhere, except in a toolbar
 * where it also has a delayed popupmenu. This action is designed
 * for history actions (back/forward, undo/redo) and for any other action
 * that has more detail in a toolbar than in a menu (e.g. tool chooser
 * with "Other" leading to a dialog...).
 */
class KToolBarPopupAction : public KAction
{
  Q_OBJECT
  Q_PROPERTY( bool delayed READ delayed WRITE setDelayed )
  Q_PROPERTY( bool stickyMenu READ stickyMenu WRITE setStickyMenu )

public:
    //Not all constructors - because we need an icon, since this action only makes
    // sense in a toolbar (as well as menubar)
    /**
     * Create a KToolBarPopupAction, with a text, an icon, an optionnal accelerator,
     * parent and name.
     *
     * @param text The text that will be displayed.
     * @param icon The icon to display.
     * @param accel The corresponding keyboard accelerator (shortcut).
     * @param parent This action's parent.
     * @param name An internal name for this action.
     */
    KToolBarPopupAction( const QString& text, const QString& icon, int accel = 0,
                         QObject* parent = 0, const char* name = 0 );

    /**
     * Create a KToolBarPopupAction, with a text, an icon, an accelerator,
     * a slot connected to the action, parent and name.
     *
     * If you do not want or have a keyboard accelerator, set the
     * @p accel param to 0.
     *
     * @param text The text that will be displayed.
     * @param icon The icon to display.
     * @param accel The corresponding keyboard accelerator (shortcut).
     * @param receiver The SLOT's owner.
     * @param slot The SLOT to invoke to execute this action.
     * @param parent This action's parent.
     * @param name An internal name for this action.
     */
    KToolBarPopupAction( const QString& text, const QString& icon, int accel,
                         const QObject* receiver, const char* slot,
                         QObject* parent = 0, const char* name = 0 );

    virtual ~KToolBarPopupAction();

    virtual int plug( QWidget *widget, int index = -1 );
    virtual void unplug( QWidget *widget );

    /**
     * The popup menu that is shown when clicking (some time) on the toolbar
     * button. You may want to plug items into it on creation, or connect to
     * aboutToShow for a more dynamic menu.
     */
    KPopupMenu *popupMenu();

    /**
     * Returns true if this action creates a delayed popup menu
     * when plugged in a KToolbar.
     */
    bool delayed() const;
    /**
     * If set to true, this action will create a delayed popup menu
     * when plugged in a KToolbar. Otherwise it creates a normal popup.
     * Default: delayed.
     */
    void setDelayed(bool delayed);
    /**
     * Returns true if this action creates a sticky popup menu.
     * See @ref setStickyMenu.
     */
    bool stickyMenu() const;
    /**
     * If set to true, this action will create a sticky popup menu
     * when plugged in a KToolbar.
     * "Sticky", means it's visible until a selection is made or the mouse is
     * clicked elsewhere. This feature allows you to make a selection without
     * having to press and hold down the mouse while making a selection.
     * Only available if delayed() is true.
     * Default: sticky.
     */
    void setStickyMenu(bool sticky);

private:
    KPopupMenu *m_popup;
    bool m_delayed:1;
    bool m_stickyMenu:1;
    class KToolBarPopupActionPrivate;
    KToolBarPopupActionPrivate *d;
};

class KActionSeparator : public KAction
{
    Q_OBJECT
public:
    KActionSeparator( QObject* parent = 0, const char* name = 0 );
    virtual ~KActionSeparator();

    virtual int plug( QWidget*, int index = -1 );
    virtual void unplug( QWidget* );

private:
    class KActionSeparatorPrivate;
    KActionSeparatorPrivate *d;
};

class KActionCollection : public QObject
{
  friend class KAction;

  Q_OBJECT
public:
  KActionCollection( QObject *parent = 0, const char *name = 0, KInstance *instance = 0 );
  KActionCollection( const KActionCollection &copy );
  virtual ~KActionCollection();

  virtual void insert( KAction* );
  virtual void remove( KAction* );
  virtual KAction* take( KAction* );

  virtual KAction* action( int index ) const;
  virtual uint count() const;
  bool isEmpty() const { return count() == 0; }
  virtual KAction* action( const char* name, const char* classname = 0 ) const;

  virtual QStringList groups() const;
  virtual QValueList<KAction*> actions( const QString& group ) const;
  virtual QValueList<KAction*> actions() const;

  KActionCollection operator+ (const KActionCollection& ) const;
  KActionCollection& operator= (const KActionCollection& );
  KActionCollection& operator+= (const KActionCollection& );

  virtual void setKeyMap( const KKeyEntryMap& map );
  virtual KKeyEntryMap& keyMap();

  void setInstance( KInstance *instance );
  KInstance *instance() const;

  void setHighlightingEnabled( bool enable );
  bool highlightingEnabled() const;

  void connectHighlight( QWidget *container, KAction *action );
  void disconnectHighlight( QWidget *container, KAction *action );

public slots:
  /**
   * Clears the entire actionCollection, deleting all actions.
   * @see #remove
   */
  void clear();

signals:
  void inserted( KAction* );
  void removed( KAction* );

  void actionHighlighted( KAction *action );
  void actionHighlighted( KAction *action, bool highlight );

  void actionStatusText( const QString &text );
  void clearStatusText();

protected:
   virtual void childEvent( QChildEvent * ) {} // ### KDE 3.0: removeme

private slots:
   void slotMenuItemHighlighted( int id );
   void slotToolBarButtonHighlighted( int id, bool highlight );
   void slotMenuAboutToHide();
   void slotDestroyed();

private:
   KAction *findAction( QWidget *container, int id );
   class KActionCollectionPrivate;
   KActionCollectionPrivate *d;
};

#endif
