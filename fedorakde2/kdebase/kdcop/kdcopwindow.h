/*
 * $Id: kdcopwindow.h,v 1.3 2001/06/26 22:59:30 rikkus Exp $
 *
 * Copyright (C) 2000 by Matthias Kalle Dalheimer <kalle@kde.org>
 *
 * Licensed under the Artistic License.
 */

#ifndef __KDCOPWINDOW_H__
#define __KDCOPWINDOW_H__

class DCOPClient;
class QListView;
class QListViewItem;
class DCOPBrowserItem;
class KAction;
class QWidgetStack;
class QListBox;
class QLabel;

#include <kmainwindow.h>

class KDCOPWindow : public KMainWindow
{
  Q_OBJECT

  public:

    KDCOPWindow( QWidget* parent = 0, const char* name = 0 );

  protected slots:

    void slotCurrentChanged( QListViewItem* item );
    void slotCallFunction();
    void slotCallFunction( QListViewItem* item );
    void slotApplicationRegistered(const QCString &);
    void slotApplicationUnregistered(const QCString &);

  private:

    bool fillApplications();
    void fillObjects( DCOPBrowserItem*, const char* app );
    void fillFunctions( DCOPBrowserItem*, const char* app, const char* obj );

    bool getParameters
      (
       const QString  & unNormalisedSignature,
       QString        & normalisedSignature,
       QStringList    & types,
       QStringList    & names
      );

    QStringList demarshal(const QCString & replyType, QByteArray replyData);

    DCOPClient    * dcop;
    QListView     * lv;
    KAction       * exeaction;
    QWidgetStack  * replyWidgetStack;
    QLabel        * l_replyType;
    QListBox      * lb_replyData;
    QWidget       * realReplyWidget;
    QWidget       * crapReplyWidget;
    QLabel        * l_replyStatus;
};

#endif
