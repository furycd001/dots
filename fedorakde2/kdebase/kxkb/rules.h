#ifndef __RULES_H__
#define __RULES_H__


#include <qstring.h>
#include <qdict.h>

class KeyRules
{
public:

  KeyRules(QString rule="xfree86", QString path = QString::null);

  const QDict<char> &models() { return _models; };
  const QDict<char> &layouts() { return _layouts; };
  const QDict<char> &encodings() { return _encodings; };
    const QDict<const unsigned int> &group() { return _group; }
    
  static QStringList rules(QString path = QString::null);


protected:

  void loadRules(QString filename);
    void loadEncodings(QString filename);

private:

  QDict<char> _models;
  QDict<char> _layouts;
    QDict<char> _encodings;
    QDict<const unsigned int> _group;
};


#endif
