/* -------------------------------------------------------------

   urlgrabber.h (part of Klipper - Cut & paste history for KDE)

   $Id: urlgrabber.h,v 1.10 2001/01/30 21:46:15 pfeiffer Exp $

   (C) 2000 by Carsten Pfeiffer <pfeiffer@kde.org>

   Licensed under the Artistic License

 ------------------------------------------------------------- */

#ifndef URLGRABBER_H
#define URLGRABBER_H

#include <qlist.h>
#include <qintdict.h>
#include <qregexp.h>
#include <qstring.h>
#include <qstringlist.h>

#include <kprocess.h>


class QTimer;

class KConfig;
class KPopupMenu;

class ClipAction;
struct ClipCommand;
typedef QList<ClipAction> ActionList;
typedef QListIterator<ClipAction> ActionListIterator;

class URLGrabber : public QObject
{
  Q_OBJECT

public:
  URLGrabber();
  ~URLGrabber();

  /**
   * Checks a given string whether it matches any of the user-defined criteria.
   * If it does, the configured action will be executed.
   * @returns false if the string should be put into the popupmenu or not,
   * otherwise true.
   */
  bool checkNewData( const QString& clipData );
  void invokeAction( const QString& clip = QString::null );

  const ActionList * actionList() const { return myActions; }
  void setActionList( ActionList * );
  void readConfiguration( KConfig * );
  void writeConfiguration( KConfig * );

  int popupTimeout() const { return myPopupKillTimeout; }
  void setPopupTimeout( int timeout ) { myPopupKillTimeout = timeout; }

  const QStringList& avoidWindows() const { return myAvoidWindows; }
  void setAvoidWindows( const QStringList& list ) { myAvoidWindows = list; }

private:	
  const ActionList& matchingActions( const QString& );
  void execute( const struct ClipCommand *command ) const;
  void startProcess( const QString& cmdLine ) const;
  void editData();
  bool isAvoidedWindow() const;
  void actionMenu( bool wm_class_check );

  ActionList *myActions;
  ActionList myMatches;
  QStringList myAvoidWindows;
  QString myClipData;
  ClipAction *myCurrentAction;
  QIntDict<ClipCommand> myCommandMapper;
  KPopupMenu *myMenu;
  QTimer *myPopupKillTimer;
  int myPopupKillTimeout;

private slots:
  void slotActionMenu() { actionMenu( true ); }
  void slotItemSelected( int );
  void slotKillPopupMenu();


signals:
    void sigPopup( QPopupMenu * );

};


struct ClipCommand
{
    ClipCommand( const QString &, const QString &, bool = true );
    QString command;
    QString description;
    bool isEnabled;
    QString pixmap;
    //  int id; // the index reflecting the position in the list of commands
};

/**
 * Represents one configured action. An action consists of one regular
 * expression, an (optional) description and a list of ClipCommands
 * (a command to be executed, a description and an enabled/disabled flag).
 */
class ClipAction
{
public:
  ClipAction( const QString& regExp, const QString& description );
  ClipAction( const ClipAction& );
  ClipAction( KConfig *kc );
  ~ClipAction();

  void  setRegExp( const QString& r) 	      { myRegExp = r; }
  QString regExp() 			const { return myRegExp.pattern(); }
  inline bool matches( const QString& string ) const {
      return ( myRegExp.match( string ) != -1 );
  }

  void 	setDescription( const QString& d)     { myDescription = d; }
  const QString& description() 		const { return myDescription; }

  /**
   * Removes all ClipCommands associated with this ClipAction.
   */
  void clearCommands() { myCommands.clear(); }

  void  addCommand( const QString& command, const QString& description, bool );
  const QList<ClipCommand>& commands() 	const { return myCommands; }

  /**
   * Saves this action to a a given KConfig object
   */
  void save( KConfig * ) const;


private:
  void startProcess( const QString& cmdLine ) const;

  QRegExp 		myRegExp;
  QString 		myDescription;
  QList<ClipCommand> 	myCommands;

};


#endif // URLGRABBER_H
