/*
 * $Id: kdcopwindow.cpp,v 1.13 2001/07/18 21:34:09 firebaugh Exp $
 *
 * Copyright (C) 2000 by Matthias Kalle Dalheimer <kalle@kde.org>
 *
 * Licensed under the Artistic License.
 */

#include "kdcopwindow.h"

#include <dcopclient.h>
#include <klocale.h>
#include <kdatastream.h>
#include <kstdaction.h>
#include <kaction.h>
#include <kapp.h>
#include <kmessagebox.h>
#include <kdialog.h>

#include <qlistview.h>
#include <qwidgetstack.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <qsplitter.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qvalidator.h>
#include <qpushbutton.h>
#include <qkeycode.h>
#include <qdialog.h>

#include <kdebug.h>

#include <stdio.h>

class DCOPBrowserItem : public QListViewItem
{
  public:

    enum DCOPBrowserItemType { Application, Interface, Function };

    DCOPBrowserItem
    (
     DCOPBrowserItemType t,
     DCOPBrowserItem * parent,
     QCString t0 = 0,
     QCString t1 = 0,
     QCString t2 = 0
    )
      : QListViewItem(parent, t0, t1, t2),
        _t(t),
        app(t0),
        obj(t1),
        func(t2)
    {
    }

    DCOPBrowserItem
    (
     DCOPBrowserItemType t,
     QListView * parent,
     QCString t0 = 0,
     QCString t1 = 0,
     QCString t2 = 0
    )
      : QListViewItem(parent, t0, t1, t2),
        _t(t),
        app(t0),
        obj(t1),
        func(t2)
    {
    }

    DCOPBrowserItemType _t;
    QCString app;
    QCString obj;
    QCString func;
};

KDCOPWindow::KDCOPWindow(QWidget *, const char * name)
  : KMainWindow(0, name)
{
  dcop = kapp->dcopClient();
  dcop->attach();

  statusBar()->message(i18n("Welcome to the KDE DCOP browser"));

  QSplitter * splitter = new QSplitter(Vertical, this, "splitter");

  setCentralWidget(splitter);

  lv = new QListView(splitter);

  lv->addColumn(i18n("Application"));
  lv->addColumn(i18n("Interface"));
  lv->addColumn(i18n("Function"));

  lv->setRootIsDecorated(true);

  lv->setAllColumnsShowFocus(true);

  connect
    (
     lv,
     SIGNAL(doubleClicked(QListViewItem *)),
     SLOT(slotCallFunction(QListViewItem *))
    );

  connect
    (
     lv,
     SIGNAL(currentChanged(QListViewItem *)),
     SLOT(slotCurrentChanged(QListViewItem *))
    );

  replyWidgetStack = new QWidgetStack(splitter, "widget stack");

  {
    realReplyWidget = new QWidget(replyWidgetStack, "realReplyWidget");

    l_replyType = new QLabel(realReplyWidget, "l_replyType");
    lb_replyData = new QListBox(realReplyWidget, "lb_replyData");

    QVBoxLayout * replyLayout =
      new QVBoxLayout
      (
       realReplyWidget,
       KDialog::marginHint(),
       KDialog::spacingHint(),
       "replyLayout"
      );

    replyLayout->addWidget(l_replyType);
    replyLayout->addWidget(lb_replyData);
  }

  {
    crapReplyWidget = new QWidget(replyWidgetStack, "crapReplyWidget");

    l_replyStatus = new QLabel(crapReplyWidget);

    QHBoxLayout * crapLayout =
      new QHBoxLayout
      (
       crapReplyWidget,
       KDialog::marginHint(),
       KDialog::spacingHint(),
       "crapLayout"
      );

    crapLayout->addWidget(l_replyStatus);
  }

  replyWidgetStack->addWidget(realReplyWidget, 0);
  replyWidgetStack->addWidget(crapReplyWidget, 1);

  replyWidgetStack->raiseWidget(crapReplyWidget);

  // set up the actions
  KStdAction::quit( this, SLOT( close() ), actionCollection() );

  exeaction =
    new KAction
    (
     i18n("&Execute"),
     CTRL + Key_E,
     this,
     SLOT(slotCallFunction()),
     actionCollection(),
     "execute"
    );

  exeaction->setEnabled(false);

  createGUI();

  fillApplications();

  connect
    (
     dcop,
     SIGNAL(applicationRegistered(const QCString &)),
     SLOT(slotApplicationRegistered(const QCString &))
    );

  connect
    (
     dcop,
     SIGNAL(applicationRemoved(const QCString &)),
     SLOT(slotApplicationUnregistered(const QCString &))
    );

  dcop->setNotifications(true);

  setCaption(i18n("DCOP Browser"));
}


void KDCOPWindow::slotCurrentChanged( QListViewItem* i )
{
  DCOPBrowserItem* item = (DCOPBrowserItem*)i;

  if( item->_t == DCOPBrowserItem::Function )
    exeaction->setEnabled( true );
  else
    exeaction->setEnabled( false );
}


void KDCOPWindow::slotCallFunction()
{
  slotCallFunction( lv->currentItem() );
}


void KDCOPWindow::slotCallFunction( QListViewItem* it )
{
  DCOPBrowserItem* item = static_cast<DCOPBrowserItem *>(it);

  if (item->_t != DCOPBrowserItem::Function)
    return;

  QString unNormalisedSignature = QString::fromUtf8(item->func);
  QString normalisedSignature;
  QStringList types;
  QStringList names;

  if (!getParameters(unNormalisedSignature, normalisedSignature, types, names))
  {
    KMessageBox::error
      (this, i18n("No parameters found"), i18n("DCOP Browser Error"));

    return;
  }

  QByteArray data;
  QByteArray replyData;

  QCString replyType;

  QDataStream arg(data, IO_WriteOnly);

  QDialog* mydialog = new QDialog( 0, "KDCOP parameter entry", true );

  mydialog->setCaption( QString( i18n("Call function %1") ).arg( item->func ) );

  QLabel* h1 = new QLabel( i18n( "Name" ), mydialog );
  QLabel* h2 = new QLabel( i18n( "Type" ), mydialog );
  QLabel* h3 = new QLabel( i18n( "Value" ), mydialog );

  QGridLayout* grid = new QGridLayout( mydialog, types.count() + 2, 3, 10 );

  grid->addWidget( h1, 0, 0 );
  grid->addWidget( h2, 0, 1 );
  grid->addWidget( h3, 0, 2 );

  // Build up a dialog for parameter entry if there are any parameters.

  if (types.count())
  {
    int i = 0;

    QList<QWidget> wl;

    for (QStringList::Iterator it = types.begin(); it != types.end(); ++it)
    {
      i++;

      QString type = *it;

      if( type == "int" )
      {
        QLabel* n = new QLabel( names[i-1], mydialog );
        grid->addWidget( n, i, 0 );
        QLabel* l = new QLabel( "int", mydialog );
        grid->addWidget( l, i, 1 );
        QLineEdit* e = new QLineEdit( mydialog );
        grid->addWidget( e, i, 2 );
        wl.append( e );
        e->setValidator( new QIntValidator( e ) );
      }
      else if ( type == "unsigned"  || type == "uint" )
      {
        QLabel* n = new QLabel( names[i-1], mydialog );
        grid->addWidget( n, i, 0 );
        QLabel* l = new QLabel( "unsigned", mydialog );
        grid->addWidget( l, i, 1 );
        QLineEdit* e = new QLineEdit( mydialog );
        grid->addWidget( e, i, 2 );
        wl.append( e );

        QIntValidator* iv = new QIntValidator( e );
        iv->setBottom( 0 );
        e->setValidator( iv );
      }
      else if ( type == "long" || type == "ulong" )
      {
        QLabel* n = new QLabel( names[i-1], mydialog );
        grid->addWidget( n, i, 0 );
        QLabel* l = new QLabel( "long", mydialog );
        grid->addWidget( l, i, 1 );
        QLineEdit* e = new QLineEdit( mydialog );
        grid->addWidget( e, i, 2 );
        wl.append( e );
        e->setValidator( new QIntValidator( e ) );
      }
      else if ( type == "float" )
      {
        QLabel* n = new QLabel( names[i-1], mydialog );
        grid->addWidget( n, i, 0 );
        QLabel* l = new QLabel( "float", mydialog );
        grid->addWidget( l, i, 1 );
        QLineEdit* e = new QLineEdit( mydialog );
        grid->addWidget( e, i, 2 );
        wl.append( e );
        e->setValidator( new QDoubleValidator( e ) );
      }
      else if ( type == "double" )
      {
        QLabel* n = new QLabel( names[i-1], mydialog );
        grid->addWidget( n, i, 0 );
        QLabel* l = new QLabel( "double", mydialog );
        grid->addWidget( l, i, 1 );
        QLineEdit* e = new QLineEdit( mydialog );
        grid->addWidget( e, i, 2 );
        wl.append( e );
        e->setValidator( new QDoubleValidator( e ) );
      }
      else if ( type == "bool" )
      {
        QLabel* n = new QLabel( names[i-1], mydialog );
        grid->addWidget( n, i, 0 );
        QLabel* l = new QLabel( "bool", mydialog );
        grid->addWidget( l, i, 1 );
        QCheckBox* c = new QCheckBox( mydialog );
        grid->addWidget( c, i, 2 );
        wl.append( c );
      }
      else if ( type == "QString" )
      {
        QLabel* n = new QLabel( names[i-1], mydialog );
        grid->addWidget( n, i, 0 );
        QLabel* l = new QLabel( "QString", mydialog );
        grid->addWidget( l, i, 1 );
        QLineEdit* e = new QLineEdit( mydialog );
        grid->addWidget( e, i, 2 );
        wl.append( e );
      }
      else if ( type == "QCString" )
      {
        QLabel* n = new QLabel( names[i-1], mydialog );
        grid->addWidget( n, i, 0 );
        QLabel* l = new QLabel( "QString", mydialog );
        grid->addWidget( l, i, 1 );
        QLineEdit* e = new QLineEdit( mydialog );
        grid->addWidget( e, i, 2 );
        wl.append( e );
      }
      else if ( type == "KURL" )
      {
        QLabel* n = new QLabel( names[i-1], mydialog );
        grid->addWidget( n, i, 0 );
        QLabel* l = new QLabel( "KURL", mydialog );
        grid->addWidget( l, i, 1 );
        QLineEdit* e = new QLineEdit( mydialog );
        grid->addWidget( e, i, 2 );
        wl.append( e );
      }
      else
      {
        KMessageBox::sorry(this, i18n("Cannot handle datatype %1").arg(type));
        return;
      }
    }

    if (!wl.isEmpty())
      wl.at(0)->setFocus();

    i++;

    QPushButton* ok = new QPushButton( i18n( "OK" ), mydialog );
    ok->setDefault( true );


    QPushButton* cancel = new QPushButton( i18n( "Cancel" ), mydialog );

    grid->addWidget( ok, i, 0 );
    grid->addWidget( cancel, i, 1 );

    QObject::connect( ok, SIGNAL( clicked() ), mydialog, SLOT( accept() ) );
    QObject::connect( cancel, SIGNAL( clicked() ), mydialog, SLOT( reject() ) );

    int ret = mydialog->exec();

    if (QDialog::Accepted != ret)
      return;

    // extract the arguments

    i = 0;

    for
      (
       QStringList::ConstIterator it = types.begin();
       it != types.end();
       ++it
      )
    {
      QString type = *it;

      if ( type == "int" )
      {
        QLineEdit* e = (QLineEdit*)wl.at( i );
        arg << e->text().toInt();
      }
      if ( type == "unsigned" || type == "uint" )
      {
        QLineEdit* e = (QLineEdit*)wl.at( i );
        arg << e->text().toUInt();
      }
      else if( type == "long" )
      {
        QLineEdit* e = (QLineEdit*)wl.at( i );
        arg << e->text().toLong();
      }
      else if( type == "ulong" )
      {
        QLineEdit* e = (QLineEdit*)wl.at( i );
        arg << e->text().toULong();
      }
      else if( type == "float" )
      {
        QLineEdit* e = (QLineEdit*)wl.at( i );
        arg << e->text().toFloat();
      }
      else if( type == "double" )
      {
        QLineEdit* e = (QLineEdit*)wl.at( i );
        arg << e->text().toDouble();
      }
      else if( type == "bool" )
      {
        QCheckBox* c = (QCheckBox*)wl.at( i );
        arg << c->isChecked();
      }
      else if( type == "QCString" )
      {
        QLineEdit* e = (QLineEdit*)wl.at( i );
        arg << QCString( e->text().local8Bit() );
      }
      else if( type == "QString" )
      {
        QLineEdit* e = (QLineEdit*)wl.at( i );
        arg << e->text();
      }
      else if( type == "KURL" )
      {
        QLineEdit* e = (QLineEdit*)wl.at( i );
        arg << KURL( e->text() );
      }
      else
      {
        KMessageBox::sorry(this, i18n("Cannot handle datatype %1").arg(type));
        return;
      }

      i++;
    }
  }

  // Now do the DCOP call

  if
    (
     !dcop->call
     (
      item->app,
      item->obj,
      normalisedSignature.utf8(),
      data,
      replyType,
      replyData
     )
    )
  {
    kdDebug()
      << "call failed( " << item->app.data() << ", " << item->obj.data()
      << ", " << normalisedSignature << " )" << endl;

    statusBar()->message(i18n("DCOP call failed"));

    QString msg = i18n("<p>DCOP call failed.</p>%1");

    if (dcop->isApplicationRegistered(item->app))
    {
      msg =
        msg.arg
        (
         i18n
         (
          "<p>Application is still registered with DCOP."
          " I don't know why this call failed.</p>"
         )
        );
    }
    else
    {
      msg =
        msg.arg
        (
         i18n
         (
          "<p>The application appears to have unregistered with DCOP.</p>"
         )
        );
    }

    KMessageBox::information(this, msg);
  }
  else
  {
    QString coolSignature =
      QString::fromUtf8(item->app)
      + "."
      + QString::fromUtf8(item->obj)
      + "."
      + normalisedSignature ;

    statusBar()
      ->message(i18n("DCOP call %1 executed").arg(coolSignature));

    if (replyType != "void" && replyType != "ASYNC" && replyType != "")
    {
      QStringList ret = demarshal(replyType, replyData);

      l_replyType->setText
        (
         i18n("Reply type: <strong>%1</strong>")
         .arg(QString::fromUtf8(replyType))
        );

      lb_replyData->clear();
      lb_replyData->insertStringList(ret);

      replyWidgetStack->raiseWidget(realReplyWidget);
    }
    else
    {
      l_replyStatus->setText(i18n("No returned values"));
      replyWidgetStack->raiseWidget(crapReplyWidget);
    }
  }
}


  void
KDCOPWindow::fillFunctions
(
 DCOPBrowserItem * item,
 const char * _app,
 const char * _obj
)
{
  QCString app(_app);
  QCString obj(_obj);

  if (app == "kicker" && obj == "appletArea")
  {
    // Why ?
    return;
  }

  bool ok = false;

  QCStringList funcs = dcop->remoteFunctions( app, obj, &ok );

  if (!ok)
    return;

  for (QCStringList::Iterator it = funcs.begin(); it != funcs.end(); ++it)
  {
    if ((*it) == "QCStringList functions()")
      continue;

    new DCOPBrowserItem
      (DCOPBrowserItem::Function, item, app, obj, (*it).data());
  }
}

  void
KDCOPWindow::fillObjects(DCOPBrowserItem * item, const char * app)
{
  if (!qstrncmp(app, "klauncher", 9))
    return;

  bool ok = false;

  QCStringList objs = dcop->remoteObjects(app, &ok);

  for (QCStringList::ConstIterator it = objs.begin(); it != objs.end(); ++it)
  {
    DCOPBrowserItem * oitem;

    if ((*it) == "default" && ++it != objs.end())
    {
      oitem =
        new DCOPBrowserItem
        (
         DCOPBrowserItem::Interface,
         item,
         app,
         i18n("%1 (default)").arg((*it).data()).local8Bit()
        );
    }
    else
    {
      oitem =
        new DCOPBrowserItem
        (DCOPBrowserItem::Interface, item, app, (*it).data());
    }

    fillFunctions( oitem, app, (*it).data() );
  }
}

bool KDCOPWindow::fillApplications()
{
  QCStringList apps = dcop->registeredApplications();

  for (QCStringList::ConstIterator it = apps.begin(); it != apps.end(); ++it)
  {
    if ((*it) != dcop->appId() && (*it).left(9) != "anonymous")
    {
      DCOPBrowserItem * item =
        new DCOPBrowserItem
        (DCOPBrowserItem::Application, lv, (*it).data());

      fillObjects(item, (*it).data());
    }
  }

  return dcop->isAttached();
}

  QStringList
KDCOPWindow::demarshal
(
 const QCString & replyType,
 QByteArray       replyData
)
{
  QStringList ret;

  QDataStream reply(replyData, IO_ReadOnly);

  if ( replyType == "int" )
  {
    int i;
    reply >> i;
    ret << QString::number(i);
  }
  else if ( replyType == "uint" )
  {
    uint i;
    reply >> i;
    ret << QString::number(i);
  }
  else if ( replyType == "long" )
  {
    long l;
    reply >> l;
    ret << QString::number(l);
  }
  else if ( replyType == "ulong" )
  {
    ulong l;
    reply >> l;
    ret << QString::number(l);
  }
  else if ( replyType == "float" )
  {
    float f;
    reply >> f;
    ret << QString::number(f);
  }
  else if ( replyType == "double" )
  {
    double d;
    reply >> d;
    ret << QString::number(d);
  }
  else if (replyType == "bool")
  {
    bool b;
    reply >> b;
    ret << (b ? QString::fromUtf8("true") : QString::fromUtf8("false"));
  }
  else if (replyType == "QString")
  {
    QString s;
    reply >> s;
    ret << s;
  }
  else if (replyType == "QStringList")
  {
    reply >> ret;
  }
  else if (replyType == "QCString")
  {
    QCString r;
    reply >> r;
    ret << QString::fromUtf8(r);
  }
  else if (replyType == "QCStringList")
  {
    QCStringList lst;
    reply >> lst;

    for (QCStringList::ConstIterator it(lst.begin()); it != lst.end(); ++it)
      ret << *it;
  }
  else if (replyType == "KURL")
  {
    KURL r;
    reply >> r;
    ret << r.prettyURL();
  }
  else
  {
    ret <<
      i18n("Don't know how to demarshal %1").arg(QString::fromUtf8(replyType));
  }

  return ret;
}

  void
KDCOPWindow::slotApplicationRegistered(const QCString & appName)
{
  QListViewItemIterator it(lv);

  for (; it.current(); ++it)
  {
    DCOPBrowserItem * item = static_cast<DCOPBrowserItem *>(it.current());

    if (item->app == appName)
      return;
  }

  if (appName != dcop->appId() && appName.left(9) != "anonymous")
  {
    DCOPBrowserItem * item =
      new DCOPBrowserItem
      (DCOPBrowserItem::Application, lv, appName.data());

    fillObjects(item, appName.data());
  }
}

  void
KDCOPWindow::slotApplicationUnregistered(const QCString & appName)
{
  QListViewItemIterator it(lv);

  for (; it.current(); ++it)
  {
    DCOPBrowserItem * item = static_cast<DCOPBrowserItem *>(it.current());

    if (item->app == appName)
    {
      delete item;
      return;
    }
  }
}

  bool
KDCOPWindow::getParameters
(
 const QString  & _unNormalisedSignature,
 QString        & normalisedSignature,
 QStringList    & types,
 QStringList    & names
)
{
  QString unNormalisedSignature(_unNormalisedSignature);

  int s = unNormalisedSignature.find(' ');

  if ( s < 0 )
    s = 0;
  else
    s++;

  unNormalisedSignature = unNormalisedSignature.mid(s);

  int left  = unNormalisedSignature.find('(');
  int right = unNormalisedSignature.findRev(')');

  if (-1 == left)
  {
    // Fucked up function signature.
    return false;
  }

  if (left > 0 && left + 1 < right - 1)
  {
    types =
      QStringList::split
      (',', unNormalisedSignature.mid(left + 1, right - left - 1));

    for (QStringList::Iterator it = types.begin(); it != types.end(); ++it)
    {
      (*it) = (*it).stripWhiteSpace();

      int s = (*it).find(' ');

      if (-1 != s)
      {
        names.append((*it).mid(s + 1));

        (*it) = (*it).left(s);
      }
    }
  }

  normalisedSignature =
    unNormalisedSignature.left(left) + "(" + types.join(",") + ")";

  return true;
}


#include "kdcopwindow.moc"
