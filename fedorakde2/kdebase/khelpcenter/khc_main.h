
#ifndef __khc_main_h__
#define __khc_main_h__

#include <kmainwindow.h>
#include <kio/job.h>
#include <kparts/browserextension.h>
#include "khc_navigator.h"
#include <kurl.h>
#include <qlist.h>

class KHTMLPart;
class QSplitter;
class KToolBarPopupAction;
class KHCView;

struct HistoryEntry
{
  KURL url;
  QString title;
  QByteArray buffer;
};

class KHMainWindow : public KMainWindow
{
    Q_OBJECT

public:
    KHMainWindow(const KURL &url);
    ~KHMainWindow();

public slots:
    void slotStarted(KIO::Job *job);
    void slotInfoMessage(KIO::Job *, const QString &);
    void openURL(const QString &url);
    void slotGlossSelected(const khcNavigatorWidget::GlossaryEntry &entry);                              
    void slotOpenURLRequest( const KURL &url,
                             const KParts::URLArgs &args);
    void slotBack();
    void slotBackActivated( int id );
    void slotForward();
    void slotForwardActivated( int id );
    void slotGoHistoryActivated( int steps );
    void slotGoHistoryDelayed();
    void documentCompleted();
    void fillBackMenu();
    void fillForwardMenu();
    void fillGoMenu();
    void goMenuActivated( int id );

private:
    void createHistoryEntry();
    void updateHistoryEntry();
    void goHistory( int steps );
    void openURL( const KURL &url );
    void updateHistoryActions();
    void stop();
    void fillHistoryPopup( QPopupMenu *popup, bool onlyBack, bool onlyForward,
                           bool checkCurrentItem, uint startPos = 0 );

    inline bool canGoBack() const { return m_lstHistory.at() > 0; }
    inline bool canGoForward() const { return m_lstHistory.at() != (int)m_lstHistory.count() - 1; }

    KHCView *doc;
    QSplitter *splitter;
    khcNavigator *nav;
    KToolBarPopupAction *back, *forward;
    int m_goBuffer;
    QList<HistoryEntry> m_lstHistory;
    int m_goMenuIndex;
    int m_goMenuHistoryStartPos;
    int m_goMenuHistoryCurrentPos;
};

#endif
