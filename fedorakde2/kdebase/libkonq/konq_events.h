#ifndef __konq_events_h__
#define __konq_events_h__

//$Id: konq_events.h,v 1.8 2001/02/08 14:03:25 mueller Exp $

#include <kparts/event.h>
#include <qlist.h>

namespace KParts
{
  class ReadOnlyPart;
};

class KFileItem;
typedef QList<KFileItem> KFileItemList;

class KonqFileSelectionEvent : public KParts::Event
{
public:
  KonqFileSelectionEvent( const KFileItemList &selection, KParts::ReadOnlyPart *part ) : KParts::Event( s_fileItemSelectionEventName ), m_selection( selection ), m_part( part ) {}

  KFileItemList selection() const { return m_selection; }
  KParts::ReadOnlyPart *part() const { return m_part; }

  static bool test( const QEvent *event ) { return KParts::Event::test( event, s_fileItemSelectionEventName ); }

private:
  static const char *s_fileItemSelectionEventName;

  KFileItemList m_selection;
  KParts::ReadOnlyPart *m_part;
};

#endif
