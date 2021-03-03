#ifndef CONTACTLISTVIEW_H
#define CONTACTLISTVIEW_H

#include <qcolor.h>
#include <qpixmap.h>
#include <qtooltip.h>
#include <qstring.h>
#include <qlistview.h>

class KAddressBookView;
class ContactListView;
class ContactEntry;

class DynamicTip : public QToolTip
{
public:
    DynamicTip( ContactListView * parent );

protected:
    void maybeTip( const QPoint & );
};

class ContactListViewItem : public QListViewItem
{

public:
  ContactListViewItem( QString entryKey, ContactListView* parent, QStringList* field );
  QString entryKey();
  ContactEntry *getEntry(); // will change name back to entry some time
  virtual void refresh();
  virtual ContactListView* parent();
  virtual QString key ( int, bool ) const;
  virtual void paintCell ( QPainter * p, const QColorGroup & cg, int column, int width, int align );

private:
  QString entryKey_;
  QStringList *field;
  ContactListView *parentListView;
};

class ContactListView : public QListView
{
  Q_OBJECT

friend class ContactListViewItem;
friend class KAddressBookView;

public:
  ContactListView( KAddressBookView *parent, const char *name = 0L );
  virtual ~ContactListView() {}
  void resort();
  bool tooltips();
  KAddressBookView *getKAddressBookView();
  ContactListViewItem *getItem( QString entryKey );

protected:
  virtual void paintEmptyArea( QPainter * p, const QRect & rect );
  virtual void backgroundColorChange( const QColor &color );
  virtual void contentsMousePressEvent(QMouseEvent*);
  void contentsMouseMoveEvent( QMouseEvent *e );
  void contentsDragEnterEvent( QDragEnterEvent *e );
  void contentsDropEvent( QDropEvent *e );
  virtual void keyPressEvent( QKeyEvent * );

public slots:
  void incSearch( const QString &value );
  void setSorting( int column, bool ascending );
  void setSorting( int column );
  void loadBackground();
  void readConfig();
  void saveConfig();

private:
  KAddressBookView *pabWidget;
  int oldColumn;
  QIconSet *up, *down;
  int column;
  bool ascending;

  bool backPixmapOn;
  QString backPixmap;
  QPixmap background;
  QPixmap iBackground;
  bool underline;
  bool autUnderline;
  QColor cUnderline;
  bool tooltips_;
  QPoint presspos;
};


#endif
