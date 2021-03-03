/* This file is part of the KDE libraries
    Copyright (C) 1999,2000 Carsten Pfeiffer <pfeiffer@kde.org>

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

#ifndef KCOMPLETION_H
#define KCOMPLETION_H

#include <qmap.h>
#include <qlist.h>
#include <qobject.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qguardedptr.h>

#include <kglobalsettings.h>

class KCompTreeNode;
class KCompletionPrivate;
class KCompletionBasePrivate;
class QPopupMenu;

/**
 * This class offers easy use of "auto-completion", "manual-completion" or
 * "shell completion" on QString objects. A common use is completing filenames
 * or URLs (see @ref KURLCompletion()).
 * But it is not limited to URL-completion -- everything should be completable!
 * The user should be able to complete email-addresses, telephone-numbers,
 * commands, SQL queries, ...
 * Everytime your program knows what the user can type into an edit-field, you
 * should offer completion. With KCompletion, this is very easy, and if you are
 * using a LineEdit-widget (@ref KLineEdit()), it is even more easy.
 * Basically, you tell a KCompletion-object what strings should be completable
 * and whenever completion should be invoked, you call @ref makeCompletion().
 * KLineEdit and (an editable) KComboBox even do this automatically for you.
 *
 * KCompletion offers the completed string via the signal @ref match() and
 * all matching strings (when the result is ambiguous) via the method
 * @ref allMatches().
 *
 * Notice: auto-completion, shell completion and manual completion work
 *         slightly differently:
 *
 * @li auto-completion always returns a complete item as match.
 *     When more than one matching items are available, it will deliver just
 *     the first (depending on sorting order) item. Iterating over all matches
 *     is possible via @ref nextMatch() and @ref previousMatch().
 *
 * @li popup-completion works in the same way, the only difference being that
 *     the completed items are not put into the edit-widget, but into a
 *     separate popup-box.
 *
 * @li manual completion works the same way as auto-completion, the
 *     subtle difference is, that it isn't invoked automatically while the user
 *     is typing, but only when the user presses a special key. The difference
 *     of manual and auto-completion is therefore only visible in UI classes,
 *     KCompletion needs to know whether to deliver partial matches
 *     (shell completion) or whole matches (auto/manual completion), therefore
 *     @ref KGlobalSettings::CompletionMan and
 *     @ref KGlobalSettings::CompletionAuto have the exact same effect in
 *     KCompletion.
 *
 * @li shell completion works like how shells complete filenames:
 *     when multiple matches are available, the longest possible string of all
 *     matches is returned (i.e. only a partial item).
 *     Iterating over all matching items (complete, not partial) is possible
 *     via @ref nextMatch() and @ref previousMatch().
 *
 * You don't have to worry much about that though, KCompletion handles
 * that for you, according to the setting @ref setCompletionMode().
 * The default setting is globally configured by the user and read
 * from @ref KGlobalSettings::completionMode().
 *
 * A short example:
 * <pre>
 * KCompletion completion;
 * completion.setOrder( KCompletion::Sorted );
 * completion.addItem( "pfeiffer@kde.org" );
 * completion.addItem( "coolo@kde.org" );
 * completion.addItem( "carpdjih@sp.zrz.tu-berlin.de" );
 * completion.addItem( "carp@cs.tu-berlin.de" );
 *
 * cout << completion.makeCompletion( "ca" ).latin1() << endl;
 * </pre>
 * In shell-completion-mode, this will be "carp"; in auto-completion-
 * mode it will be "carp@cs.tu-berlin.de", as that is alphabetically
 * smaller.
 * If setOrder was set to Insertion, "carpdjih@sp.zrz.tu-berlin.de"
 * would be completed in auto-completion-mode, as that was inserted before
 * "carp@cs.tu-berlin.de".
 *
 * You can dynamically update the completable items by removing and adding them
 * whenever you want.
 * For advanced usage, you could even use multiple KCompletion objects. E.g.
 * imagine an editor like kwrite with multiple open files. You could store
 * items of each file in a different KCompletion object, so that you know (and
 * tell the user) where a completion comes from.
 *
 * Note: KCompletion does not work with strings that contain 0x0 characters
 *       (unicode nul), as this is used internally as a delimiter.
 *
 * You may inherit from KCompletion and override @ref makeCompletion() in
 * special cases (like reading directories/urls and then supplying the
 * contents to KCompletion, as KURLCompletion does), but generally, this is
 * not necessary.
 *
 *
 * @short A generic class for completing QStrings
 * @author Carsten Pfeiffer <pfeiffer@kde.org>
 * @version $Id$ 
 */
class KCompletion : public QObject
{
    Q_OBJECT

public:
    /**
     * Constants that represent the order in which KCompletion performs
     * completion-lookups.
     */
    enum CompOrder { Sorted, Insertion, Weighted };

    /**
     * Constructor, nothing special here :)
     */
    KCompletion();

    // FIXME: copy constructor, assignment constructor...

    /**
     * Destructor, nothing special here, either.
     */
    virtual ~KCompletion();

    /**
     * Attempts to find an item in the list of available completions,
     * that begins with @p string. Will either return the first matching item
     * (if there is more than one match) or QString::null, if no match was
     * found.
     *
     * In the latter case, a sound will be issued, depending on
     * @ref isSoundsEnabled().
     * If a match was found, it will also be emitted via the signal
     * @ref match().
     *
     * If this is called twice or more often with the same string while no
     * items were added or removed in the meantime, all available completions
     * will be emitted via the signal @ref matches().
     * This happens only in shell-completion-mode.
     *
     * @returns the matching item, or QString::null if there is no matching
     * item.
     * @see #slotMakeCompletion
     */
    virtual QString makeCompletion( const QString& string );

    /**
     * @returns the next item from the matching-items-list.
     * When reaching the beginning, the list is rotated so it will return the
     * last match and a sound is issued (depending on @ref isSoundsEnabled()).
     * When there is no match, QString::null is returned and
     * a sound is be issued.
     * @see #slotPreviousMatch
     */
    QString previousMatch();

    /**
     * @returns the previous item from the matching-items-list
     * When reaching the last item, the list is rotated, so it will return
     * the first match and a sound is issued (depending on
     * @ref isSoundsEnabled()). When there is no match, QString::null is
     * returned and a sound is issued.
     * @see #slotNextMatch
     */
    QString nextMatch();

    /**
     * @returns the last match. Might be useful if you need to check whether
     * a completion is different from the last one.
     * QString::null is returned when there is no last match.
     */
    virtual const QString& lastMatch() const { return myLastMatch; }


    /**
     * Returns a list of all items inserted into KCompletion. This is useful
     * if you need to save the state of a KCompletion object and restore it
     * later.
     *
     * Important note: when @ref order() == Weighted, then every item in the
     * stringlist has its weight appended, delimited by a colon. E.g. an item
     * "www.kde.org" might look like "www.kde.org:4", where 4 is the weight.
     *
     * This is necessary so that you can save the items along with its
     * weighting on disk and load them back with @ref setItems(), restoring its
     * weight as well. If you really don't want the appended weightings, call
     * @ref setOrder( KCompletion::Insertion )
     * before calling items().
     *
     * @returns a list of all items
     * @see #setItems
     */
    QStringList items() const;


    /**
     * Sets the completion mode to Auto/Manual (see @ref KCompletion
     * documentation), Shell or None.
     * If you don't set the mode explicitly, the global default value
     * KGlobalSettings::completionMode() is used.
     * @ref KGlobalSettings::CompletionNone disables completion.
     * @see #completionMode
     * @see #KGlobalSettings::completionMode
     */
    void setCompletionMode( KGlobalSettings::Completion mode );

    /**
     * @returns the current completion mode.
     * May be different from @ref KGlobalSettings::completionMode(), if you
     * explicitly called @ref setCompletionMode().
     * @see #setCompletionMode
     */
    KGlobalSettings::Completion completionMode() const {
	return myCompletionMode;
    }

    /**
     * KCompletion offers three different ways in which it offers its items:
     * @li in the order of insertion
     * @li sorted alphabetically
     * @li weighted
     *
     * Choosing weighted makes KCompletion perform an implicit weighting based
     * on how often an item is inserted. Imagine a webbrowser with a location
     * bar, where the user enters URLs. The more often a URL is entered, the
     * higher priority it gets.
     *
     * Note: Setting the order to sorted only affects new inserted items,
     * already existing items will stay in the current order. So you probably
     * want to call setOrder( Sorted ) before inserting items, when you want
     * everything sorted.
     *
     * Default is insertion order
     * @see #order
     */
    void setOrder( CompOrder order ) { myOrder = order; }

    /**
     * @returns the current completion order.
     * @see #setOrder
     */
    CompOrder order() const { return myOrder; }

    /**
     * Setting this to true makes KCompletion behave case insensitively.
     * E.g. makeCompletion( "CA" ); might return "carp@cs.tu-berlin.de".
     * Default is false (case sensitive).
     * @see #ignoreCase
     */
    //  void setIgnoreCase( bool ignoreCase ) { myIgnoreCase = ignoreCase; }

    /**
     * @returns whether KCompletion acts case insensitively or not.
     * Default is false (case sensitive).
     * @see #setIgnoreCase
     */
    //  bool ignoreCase() const { return myIgnoreCase; }

    /**
     * @returns a list of all items matching the last completed string.
     * Might take some time, when you have LOTS of items.
     */
    QStringList allMatches();

    /**
     * @returns a list of all items matching @p string.
     */
    QStringList allMatches( const QString& string );

    /**
     * Enables playing a sound when
     * @li @ref makeCompletion() can't find a match
     * @li there is a partial completion (= multiple matches in
     *     Shell-completion mode)
     * @li @ref nextMatch() or @ref previousMatch() hit the last possible
     *     match -> rotation
     *
     * For playing the sounds, @ref KNotifyClient() is used.
     *
     * @see #disableSounds
     * @see #isSoundsEnabled
     */
    void enableSounds() { myBeep = true; }

    /**
     * Disables playing sounds.
     * Default is enabled
     * @see #enableSounds
     * @see #isSoundsEnabled
     */
    void disableSounds() { myBeep = false; }

    /**
     * Tells you whether KCompletion will play sounds on certain occasions.
     * Default is enabled
     * @see #enableSounds
     * @see #disableSounds
     */
    bool isSoundsEnabled() const { return myBeep; }

    /**
     * @returns true when more than one match is found
     * @see #multipleMatches
     */
    bool hasMultipleMatches() const { return myHasMultipleMatches; }

public slots:
    /**
     * Attempts to complete "string" and emits the completion via @ref match().
     * Same as @ref makeCompletion() (just as a slot).
     * @see #makeCompletion
     */
    void slotMakeCompletion( const QString& string ) {
	(void) makeCompletion( string );
    }

    /**
     * Searches the previous matching item and emits it via @ref match()
     * Same as @ref previousMatch() (just as a slot).
     * @see #previousMatch
     */
    void slotPreviousMatch() {
	(void) previousMatch();
    }

    /**
     * Searches the next matching item and emits it via @ref match()
     * Same as @ref nextMatch() (just as a slot).
     * @see #nextMatch
     */
    void slotNextMatch() {
	(void) nextMatch();
    }

    /**
     * Inserts @p items into the list of possible completions.
     * Does the same as @ref setItems(), but does not call @ref clear() before.
     */
    void insertItems( const QStringList& items );

    /**
     * Sets the list of items available for completion. Removes all previous
     * items.
     *
     * Notice: when order() == Weighted, then the weighting is looked up for
     * every item in the stringlist. Every item should have ":number" appended,
     * where number is an unsigned integer, specifying the weighting.
     *
     * If you don't like this, call
     * setOrder( KCompletion::Insertion )
     * before calling setItems().
     *
     * @see #items
     */
    void setItems( const QStringList& );

    /**
     * Adds an item to the list of available completions.
     * Resets the current item-state (@ref previousMatch() and @ref nextMatch()
     * won't work anymore).
     */
    void addItem( const QString& );

    /**
     * Adds an item to the list of available completions.
     * Resets the current item-state (@ref previousMatch() and @ref nextMatch()
     * won't work anymore).
     *
     * Sets the weighting of the item to @p weight or adds it to the current
     * weighting if the item is already available. The weight has to be greater
     * than 1 to take effect (default weight is 1).
     */
    void addItem( const QString&, uint weight );


    /**
     * Removes an item from the list of available completions.
     * Resets the current item-state (@ref previousMatch() and @ref nextMatch()
     * won't work anymore).
     */
    void removeItem( const QString& );

    /**
     * Removes all inserted items.
     */
    void clear();


signals:
    /**
     * The matching item. Will be emitted by @ref makeCompletion(),
     * @ref previousMatch() or @ref nextMatch(). May be QString::null if there
     * is no matching item.
     */
    void match( const QString& );

    /**
     * All matching items. Will be emitted by @ref makeCompletion() in shell-
     * completion-mode, when the same string is passed to makeCompletion twice
     * or more often.
     */
    void matches( const QStringList& );

    /**
     * This signal is emitted, when calling @ref makeCompletion() and more than
     * one matching item is found.
     * @see #hasMultipleMatches
     */
    void multipleMatches();

protected:
    /**
     * This method is called after a completion is found and before the
     * matching string is emitted. You can override this method to modify the
     * string that will be emitted.
     * This is necessary e.g. in @ref KURLCompletion(), where files with spaces
     * in their names are shown escaped ("filename\ with\ spaces"), but stored
     * unescaped inside KCompletion.
     * Never delete that pointer!
     *
     * Default implementation does nothing.
     * @see #postProcessMatches
     */
    virtual void postProcessMatch( QString * /*match*/ ) {}

    /**
     * This method is called before a list of all available completions is
     * emitted via @ref matches. You can override this method to modify the
     * found items before @ref match() or @ref matches() are emitted.
     * Never delete that pointer!
     *
     * Default implementation does nothing.
     * @see #postProcessMatch
     */
    virtual void postProcessMatches( QStringList * /*matches*/ ) {}

private:
    void 		addWeightedItem( const QString& );
    QString 		findCompletion( const QString& string );
    const QStringList& 	findAllCompletions( const QString& );
    void 		extractStringsFromNode( const KCompTreeNode *,
						const QString& beginning,
						QStringList *matches,
						bool addWeight = false ) const;

    enum 		BeepMode { NoMatch, PartialMatch, Rotation };
    void 		doBeep( BeepMode );

    QStringList         myMatches;
    KGlobalSettings::Completion myCompletionMode;

    CompOrder 		myOrder;
    QString             myLastString;
    QString 		myLastMatch;
    QString 		myCurrentMatch;
    KCompTreeNode *     myTreeRoot;
    QStringList 	myRotations;
    bool                myBeep;
    bool 		myBackwards;
    bool 		myIgnoreCase;
    bool 		myHasMultipleMatches;
    uint 		myRotationIndex;


    KCompletionPrivate *d;
};


/**
 * An abstract base class for adding a completion feature
 * into widgets.
 *
 * This is a convienence class that provides the basic functions
 * needed to add text completion support into widgets.  All that
 * is required is an implementation for the pure virtual function
 * @ref setCompletionText.  Refer to @ref KLineEdit or @ref KComboBox
 * to see how easily such support can be added using this as a base
 * class.
 *
 * @short An abstract class for adding text completion support to widgets.
 * @author Dawit Alemayehu <adawit@kde.org>
 */
class KCompletionBase
{

public:

    /**
     * Constants that represent the items whose short-cut
     * key-binding is programmable.  The default key-bindings
     * for these items are defined in @ref KStdAccel().
     */
    enum KeyBindingType {
       TextCompletion,
       PrevCompletionMatch,
       NextCompletionMatch
    };


    // Map for the key binding types mentioned above.
    typedef QMap<KeyBindingType, int> KeyBindingMap;

    /**
    * Default constructor.
    */
    KCompletionBase();

    /**
    * Destructor.
    */
    virtual ~KCompletionBase();

    /**
    * Returns a pointer to the current completion object.
    *
    * If the object does not exist, it is automatically
    * created.  Note that the completion object created
    * here is used by default to handle the signals
    * internally.  It is also deleted when this object's
    * destructor is invoked.  If you do not want these
    * default settings, use @ref setAutoDeleteCompletionObject
    * and @ref setHandleSignals to change the behavior.
    * Alternatively, you can set the boolean parameter to
    * false to disable the automatic handling of the signals
    * by this object.  Note that the boolean argument will be
    * ignored if there already exists a completion object since
    * no new object needs to be created. You need to use either
    * @ref setHandleSignals or @ref setCompletionObject for
    * such cases depending on your requirement.
    *
    * @param hsig if true, handles signals internally.
    * @return a pointer the completion object.
    */
    KCompletion* completionObject( bool hsig = true );

    // ### merge these two methods
    /**
     * Overloaded method to specify whether a completion-object should be
     * created or not. If @p create is set to false and there is no
     * completion-object yet, 0L will be returned.
     */
    KCompletion* completionObject( bool create, bool hsig );

    /**
    * Sets up the completion object to be used.
    *
    * This method assigns the completion object and sets it
    * up to automatically handle the completion and rotation
    * signals internally.  You should use this function if
    * you want to share one completion object among you widgets
    * or need to use a customized completion object.
    *
    * The object assigned through this method is not deleted
    * when this object's destructor is invoked unless you
    * explicitly call @ref setAutoDeleteCompletionObject after
    * calling this method.  Also if you do not want the signals
    * to be handled by an internal implementation, be sure to
    * set the bool argument to false.
    *
    * This method is also called when a completion-object is created
    * automatically, when completionObject() is called the first time.
    * 
    * @param compObj a @ref KCompletion() or a derived child object.
    * @param hsig if true, handles signals internally.
    */
    virtual void setCompletionObject( KCompletion* /*compObj*/, bool hsig = true );

    /**
    * Enables this object to handle completion and rotation
    * events internally.
    *
    * This function simply assigns a boolean value that
    * indicates whether it should handle rotation and
    * completion events or not.  Note that this does not
    * stop the object from emitting signals when these
    * events occur.
    *
    * @param handle if true, handle completion & rotation internally.
    */
    virtual void setHandleSignals( bool /*handle*/ );

    /**
    * Returns true if the completion object is deleted
    * upon this widget's destruction.
    *
    * See @ref setCompletionObject() and @ref enableCompletion()
    * for details.
    *
    * @return true if the completion object
    */
    bool isCompletionObjectAutoDeleted() const { return m_bAutoDelCompObj; }

    /**
    * Sets the completion object when this widget's destructor
    * is called.
    *
    * If the argument is set to true, the completion object
    * is deleted when this widget's destructor is called.
    *
    * @param autoDelete if true, delete completion object on destruction.
    */
    void setAutoDeleteCompletionObject( bool autoDelete ) {
        m_bAutoDelCompObj = autoDelete;
    }

    /**
    * Sets the widget's ability to emit text completion and
    * rotation signals.
    *
    * Invoking this function with @p enable set to @p false will
    * cause the completion & rotation signals not to be emitted.
    * However, unlike setting the completion object to @p NULL
    * using @ref setCompletionObject, disabling the emition of
    * the signals through this method does not affect the current
    * completion object.
    *
    * There is no need to invoke this function by default.  When a
    * completion object is created through @ref completionObject or
    * @ref setCompletionObject, these signals are set to emit
    * automatically.  Also note that disabling this signals will not
    * necessarily interfere with the objects ability to handle these
    * events internally.  See @ref setHandleSignals.
    *
    * @param enable if false, disables the emittion of completion & rotation signals.
    */
    void setEnableSignals( bool enable ) { m_bEmitSignals = enable; }

    /**
    * Returns true if the object handles the signals
    *
    * @return true if this signals are handled internally.
    */
    bool handleSignals() const { return m_bHandleSignals; }

    /**
    * Returns true if the object emits the signals
    *
    * @return true if signals are emitted
    */
    bool emitSignals() const { return m_bEmitSignals; }

    /**
    * Sets the type of completion to be used.
    *
    * The completion modes supported are those defined in
    * @ref KGlobalSettings().  See below.
    *
    * @param mode Completion type:
    *   @li CompletionNone:  Disables completion feature.
    *   @li CompletionAuto:  Attempts to find a match &
    *                        fills-in the remaining text.
    *   @li CompletionMan:   Acts the same as the above
    *                        except the action has to be
    *                        manually triggered through
    *                        pre-defined completion key.
    *   @li CompletionShell: Mimics the completion feature
    *                        found in typcial *nix shell
    *                        enviornments.
    *   @li CompletionPopup: Shows all available completions at once,
    *                        in a listbox popping up.
    */
    virtual void setCompletionMode( KGlobalSettings::Completion mode );

    /**
    * Retrieves the current completion mode.
    *
    * The return values are of type @ref KGlobalSettings::Completion.
    * See @ref setCompletionMode() for details.
    *
    * @return the completion mode.
    */
    KGlobalSettings::Completion completionMode() const {
        return m_iCompletionMode;
    }

    /**
    * Sets the key-binding to be used for manual text
    * completion, text rotation in a history list as
    * well as a completion list.
    *
	*
    * When the keys set by this function are pressed, a
    * signal defined by the inherting widget will be activated.
    * If the default value or 0 is specified by the second
    * parameter, then the key-binding as defined in the global
    * setting should be used.  This method returns false value
    * for @p key is negative or the supplied key-binding conflicts
    * with the ones set for one of the other features.
    *
    * NOTE: To use a modifier key (Shift, Ctrl, Alt) as part of
    * the key-binding simply simply @p sum up the values of the
    * modifier and the actual key.  For example, to use CTRL+E as
    * a key binding for one of the items, you would simply supply
    * @p "Qt::CtrlButton + Qt::Key_E" as the second argument to this
    * function.
    *
    * @param item the feature whose key-binding needs to be set:
    *
    * @li TextCompletion	the manual completion key-binding.
    * @li PrevCompletionMatch	the previous match key for mutiple completion.
    * @li NextCompletionMatch	the next match key for for multiple completion.
    *
    * @param key key-binding used to rotate down in a list.
    *
    * @return  true if key-binding can successfully be set.
    */
    bool setKeyBinding( KeyBindingType /*item*/ , int key = 0 );

    /**
    * Returns the key-binding used for the specified item.
    *
    * This methods returns the key-binding used to activate
    * the feature feature given by @p item.  If the binding
    * contains modifier key(s), the SUM of the modifier key
    * and the actual key code are returned.
    *
    * @return the key-binding used for the feature given by @p item.
    */
    int getKeyBinding( KeyBindingType item ) const { return m_keyMap[ item ]; }

    /**
    * Sets this object to use global values for key-bindings.
    *
    * This method changes the values of the key bindings for
    * rotation and completion features to the default values
    * provided in KGlobalSettings.
    *
    * NOTE: By default inheriting widgets should uses the
    * global key-bindings so that there will be no need to
    * call this method.
    */
    void useGlobalKeyBindings();

    /*
    * A pure virtual function that must be implemented by
    * all inheriting classes.
    *
    * This function is intended to allow external completion
    * implementions to set completed text appropriately.  It
    * is mostly relevant when the completion mode is set to
    * CompletionAuto and CompletionManual modes. See
    * @ref KCompletionBase::setCompletedText.
    * Does nothing in CompletionPopup mode, as all available
    * matches will be shown in the popup.
    *
    * @param text the completed text to be set in the widget.
    */
    virtual void setCompletedText( const QString& /* text */ ) = 0;

protected:

     /**
     * Returns an instance of the completion object.
     *
     * This method is only different from @ref completionObject()
     * in that it does not create a new KCompletion object evenif
     * the internal pointer is @p NULL. Use this method to get the
     * pointer to a completion object when inheriting so that you
     * won't inadvertantly create it!!
     *	
     * @returns the completion object or NULL if one does not exist.
     */
     KCompletion* compObj() { return m_pCompObj; }

     /**
     * Returns a key-binding maps
     *
     * This method is the same as @ref getKeyBinding() except it
     * returns the whole keymap containing the key-bindings.
     *
     * @return the key-binding used for the feature given by @p item.
     */
     KeyBindingMap getKeyBindings() const { return m_keyMap; }	

private:

    // This method simply sets the autodelete boolen for
    // the completion object, the emit signals and handle
    // signals internally flags to the provided values.
    void setup( bool, bool, bool );

    // Flag that determined whether the completion object
    // should be deleted when this object is destroyed.
    bool m_bAutoDelCompObj;
    // Determines whether this widget handles completion signals
    // internally or not
    bool m_bHandleSignals;
    // Determines whether this widget fires rotation signals
    bool m_bEmitSignals;
    // Stores the completion mode locally.
    KGlobalSettings::Completion m_iCompletionMode;
    // Pointer to Completion object.
    QGuardedPtr<KCompletion> m_pCompObj;
    // Keybindings
    KeyBindingMap m_keyMap;
	
    // BCI
    KCompletionBasePrivate *d;	
};

#endif // KCOMPLETION_H
