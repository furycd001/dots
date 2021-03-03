#ifndef __CONNECTIONLIST_H__
#define __CONNECTIONLIST_H__

#include <qvaluelist.h>

#include <kdb/opendatabasedlg.h>

class KConfig;

class ConnectionList;
typedef QValueListIterator<KDB::ConnectionData> ConnectionListIterator;

class ConnectionList : public QValueList<KDB::ConnectionData>
{
public:
  ConnectionList();
  ~ConnectionList();

  void load();
  void save();

public:
  KConfig* m_config;
};

#endif
