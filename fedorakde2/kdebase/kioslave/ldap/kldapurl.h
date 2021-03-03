#ifndef _K_LDAPURL_H_
#define _K_LDAPURL_H_

#include <kurl.h>
#include <qstring.h>
#include <qstrlist.h>


namespace KLDAP
{

  class Url : public KURL
  {
  public:

    Url(QString _url);
  
    QString dn() { return _dn; };
    void setDn(QString dn) { _dn = dn; update(); };

    QStrList &attributes();
    QStrList &attributesEncoded() { return _attributes; };
    void setAttributes(const QStrList &attributes) { _attributes=attributes; update(); };

    int scope() { return _scope; };
    void setScope(int scope) { _scope=scope; update(); };

    QString filter() { QString item=decode_string(_filter); return item; };
    void setFilter(QString filter) { _filter=filter; update(); } ;

  protected:

    void splitString(QString q, char c, QStrList &list);

    void parseLDAP();
    void update();

  private:

    QString  _dn;
    QStrList _attributes, _attr_decoded;
    int      _scope;
    QString  _filter;
    QString  _extensions;

  };

};


#endif
