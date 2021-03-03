/***************************************************************************
                          kateviewspace.h  -  description
                             -------------------
    begin                : Mon Jan 15 2001
    copyright            : (C) 2001 by Anders Lund
    email                : anders@alweb.dk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef __kate_view_space_h__
#define __kate_view_space_h__

#include "../main/katemain.h"
#include "kateview.h"

#include <qlist.h>
#include <qwidget.h>
#include <qvbox.h>

class KSimpleConfig;
class KateVSStatusBar : public QWidget
{
  Q_OBJECT

   public:
      KateVSStatusBar ( KateViewSpace *parent = 0L, const char *name = 0L );
      virtual ~KateVSStatusBar ();

      void showActiveViewIndicator ( bool b );

   public slots:
      void slotDisplayStatusText (const QString& text);
      void slotClear ();

   signals:
      void clicked();

   protected:
      virtual bool eventFilter (QObject*,QEvent *);
      virtual void showMenu ();

      virtual void paintEvent (QPaintEvent *e);
      KateViewSpace* viewspace;
      QLabel *m_pStatusLabel;
      int m_yOffset;
      bool m_showLed;
};

class KateViewSpace : public QVBox
{
  Q_OBJECT

  public:
    KateViewSpace(QWidget* parent=0, const char* name=0);
    ~KateViewSpace();
    bool isActiveSpace();
    void setActive(bool b, bool showled=false);
    QWidgetStack* stack;
    void addView(KateView* v, bool show=true);
    void removeView(KateView* v);
    bool showView(KateView* v);
    bool showView(uint docID);
    KateView* currentView();
    int viewCount() { return mViewList.count(); }
    /** Saves the list of documents represented in this viewspace.
     * Documents with an invalid URL is discarded.
     * myIndex is used as identifyer for a config group.
     */
    void saveFileList(KSimpleConfig* config, int myIndex);
  protected:
    bool eventFilter(QObject* o, QEvent* e);
  private:
    bool mIsActiveSpace;
    KateVSStatusBar* mStatusBar;
    QLabel* l;
    QPixmap i_active;
    QPixmap i_empty;
    QList<KateView> mViewList;
    int mViewCount;

  private slots:
    void slotStatusChanged (KateView *view, int r, int c, int ovr, int mod, QString msg);
};

#endif
