#ifndef _K_LDAP_H_
#define _K_LDAP_H_


#include <qstring.h>
#include <qstrlist.h>

#include <lber.h>
#include <ldap.h>

namespace KLDAP
{

  /**
   * The base class of all LDAP objects.
   *
   * LDAPBase encapsulates the LDAP connection information.
   * It provides some helper functions for error checking and
   * error reporting.
   *
   * Objects of class LDAPBase should never be used directly.
   * All functionality is inside the derived classes.
   *
   * @short Base class of all LDAP objects.
   * @author Matthias Hoelzer-Kluepfel <hoelzer@kde.org>
   * @version $Id: kldap.h,v 1.7 2001/03/06 15:43:55 matz Exp $
   */
  class LDAPBase
  {
  public:

    /**
     * Constructor. Initialises an empty object.
     *
     */
    LDAPBase();

    /**
     * Return the LDAP handle.
     *
     * The LDAP handle is used to keep the state of
     * the client/server connection.
     *
     * Derived classes need the handle to perform
     * any LDAP operation.
     *
     * You can use the handle to perform low-level
     * operations not provided by this library.
     */
    LDAP *handle() { return _handle; };

    /**
     * Return the last result code.
     *
     * Returns the result of the last LDAP operation.
     *
     * The possible values are defined in ldap.h. The most
     * prominent result code is LDAP_SUCCESS.
     */
    int result() { return res; };

    /**
     * Returns a description of the last result.
     *
     * Returns a textual description of the last result code.
     * This text can be used in error messages.
     *
     * @see LDAPBase#result
     */
    QString error();

  protected:

    /**
     * Checks a LDAP result code.
     *
     * Checks a result code provided by a LDAP operations.
     * Stores the result code of the last operation.
     *
     * @param r result code
     * @return TRUE, if result == LDAP_SUCCESS, else FALSE
     */
    bool check(int r);

    int res;          // the last result code checked
    LDAP *_handle;    // the ldap handle

  };

  /**
   * A LDAP connection.
   *
   * This class represents a connection to a LDAP server.
   * The first thing in any LDAP activity is to connect
   * to a server.
   *
   * @short A LDAP connection.
   * @author Matthias Hoelzer-Kluepfel <hoelzer@kde.org>
   * @version $Id: kldap.h,v 1.7 2001/03/06 15:43:55 matz Exp $
   */
  class Connection : public LDAPBase
  {
    friend class Request;

  public:

    /**
     * Constructor. Creates a LDAP connection.
     *
     * The constructor creates a new LDAP connection.
     * Note that the constructor does NOT connect to the
     * server immediately. This must be done via the
     * connect() method.
     *
     * param s the server to connect, defaults to "localhost"
     * param p the port on the server, defaults to 389.
     *
     */
    Connection(const char *s="localhost", int p=LDAP_PORT);

    /**
     * Destructor. Destroys the connection.
     *
     * The destructor disconnects from the server, is
     * the connection was still active.
     *
     */
    ~Connection();

    /**
     * Connects to the LDAP server.
     *
     * This method actually connects to the server and
     * activates the connection.
     *
     * @return TRUE, if connections was established, else FALSE
     */
    bool connect();

    /**
     * Disconnects from the server.
     *
     * This method closes an active connection with a LDAP
     * server.
     *
     * @return TRUE, if the connection was closed, else FALSE
     *
     */
    bool disconnect();

    /**
     * Returns the state of the connection.
     *
     * This method tells if the connection to the server is
     * currently active.
     *
     * @return TRUE, if connected to the server, else FALSE
     *
     */
    bool isConnected() { return handle() != 0; };

    /**
     * Authenticates to the server.
     *
     * This method is used to authenticate to the server.
     *
     * Some servers require the user to authenticate before
     * operations are allowed. Currently, there are two authentication
     * methods:
     *
     * Simple authentication using a password.
     * Kerberos authentication using a ticket.
     *
     * Please note that LDAP does not provide a high level of
     * security. Especially simple authentication over
     * unencrypted lines is dangerous.
     *
     * @param dn Distinguished name of the entity to authorize as.
     * @param cred The credentials (e.g. the password)
     * @param method The authentication method.
     * @return TRUE, if authentication was successfull, else FALSE.
     *
     */
    bool authenticate(const char *dn=0, const char *cred=0, int method=LDAP_AUTH_SIMPLE);

    /// Returns the host to connect to.
    QString host() { return _server; };

    /// Sets the host to connect to.
    void setHost(const QString &host) { _server=host; };

    /// Returns the port for the connection.
    int port() { return _port; }

    /// Sets the port for the connection.
    void setPort(int port) { _port=port; };

  private:

    QString _server;
    int     _port;

  };

  /**
   * A LDAP attribute.
   *
   * This class encapsulates a LDAP attribute.
   * Attributes are returned as results of queries.
   *
   * Note that an attribute can have more than one value.
   * In fact, most attributes have several values.
   *
   * @short A LDAP attribute.
   * @author Matthias Hoelzer-Kluepfel <hoelzer@kde.org>
   * @version $Id: kldap.h,v 1.7 2001/03/06 15:43:55 matz Exp $
   *
   */
  class Attribute : public LDAPBase
  {
  public:

    /**
     * Constructor. Initializes an attribute.
     *
     * The constructor sets up the data structures
     * necessary to retrieve an attributes values.
     *
     * @param h The LDAP handle.
     * @param msg The LDAP message record.
     * @param n The name of the attribute.
     *
     */
    Attribute(LDAP *h, LDAPMessage *msg, const char *n);

    /// Returns the name of the attribute.
    QString name() { return _name; };

    /**
     * Returns the attributes values.
     *
     * Returns a list of strings containing the values
     * of the attributes.
     *
     * Note that this method only works with non-binary,
     * string value attributes.
     *
     * @param list The list that will store the values.
     *
     */
    void getValues(QStrList &list);


    /**
     * Returns the attributes binary values.
     *
     * Returns an array of binary attribute values.
     * The caller is responsible to free these values
     * via the freeBinaryValues method.
     *
     * @return array of values.
     *
     */
    struct berval **getBinaryValues();

    /**
     * Frees value array.
     *
     * This method must be called to free the binary
     * values returnd by getBinaryValues.
     *
     * @param vals The value array.
     *
     */
    void freeBinaryValues(struct berval **vals);

  private:

    LDAPMessage *message;
    char        *_name;

  };

  /**
   * A LDAP entry.
   *
   * This class encapsulates an entry in the result of a LDAP
   * query. Each entry can have a number of attributes.
   *
   * @short A LDAP entry.
   * @author Matthias Hoelzer-Kluepfel <hoelzer@kde.org>
   * @version $Id: kldap.h,v 1.7 2001/03/06 15:43:55 matz Exp $
   *
   */
  class Entry : public LDAPBase
  {
  public:

    /**
     * Constructor. Creates a LDAP entry.
     *
     * The constructor initializes the data structures
     * necessary to retrieve an entries attributes.
     *
     * @param h The LDAP handle.
     * @param msg The LDAP message record.
     *
     */
    Entry(LDAP *h, LDAPMessage *msg);

    /**
     * Returns the distinguished name of the entry.
     *
     * The distinguished name of the entry is a unique
     * identifier of an entry.
     *
     * @return The distinguished name.
     *
     */
    QString dn();

    /**
     * Returns the names of the entries attributes.
     *
     * Returns a list of strings containing the attributes
     * of the entry.
     *
     * @param list The list that will store the names.
     *
     */
    void getAttributes(QStrList &list);

    /**
     * Returns the attribute with the given name.
     *
     * Returns the entries attribute with the given name.
     * If the entry does not contain an attribute with
     * this name, an empty attribute is returned (i.e. an
     * attribute without values)
     *
     * @param name The name of the attribute.
     * @return The attribute object.
     *
     */
    Attribute getAttribute(const char *name);

  private:

    LDAPMessage *message;

  };

  /**
   * The base class for LDAP requests.
   *
   * This class is the base of all classes encapsulating
   * LDAP requests. It contains the methods and datastructures
   * to send a request to the servern and retrieve the results.
   *
   * @short Base class for LDAP requests.
   * @author Matthias Hoelzer-Kluepfel <hoelzer@kde.org>
   * @version $Id: kldap.h,v 1.7 2001/03/06 15:43:55 matz Exp $
   *
   */
  class Request : public LDAPBase
  {
  public:

    /**
     * The mode to run the request.
     *
     * LDAP operations can be done in two ways: Synchronous requests
     * block the calling process until the result is available.
     * Asynchronous request return immediately and require the
     * programm to retrieve the result later.
     *
     * As LDAP operations connect to a (remote) server, nonblocking
     * operation is probably the method of choice.
     *
     */
    enum RunMode { Synchronous, Asynchronous };

    /**
     * Constructor. Initializes an empty request.
     *
     * Note that the default mode is asynchronous.
     *
     */
    Request(Connection &c, RunMode m=Asynchronous);

    /// Destructor.
    virtual ~Request();

    /**
     * Execute the request.
     *
     * This method executes the request, i.e. it sends the request
     * to the server. If the run mode is "Synchronous", this method
     * will block until the result is available, or the timeout
     * has been exceeded.
     *
     * @see Request#setTimeout
     *
     * @return TRUE, if the request was initiated, else FALSE.
     *
     */
    virtual bool execute();

    /**
     * Finish the request.
     *
     * This method retrieves the result of the request.
     * If the mode is "Asynchronous", this will block until the
     * result is available, or the timeout has been exceeded.
     *
     * @see Request#setTimeout
     *
     * @return TRUE, if the result is available, else FALSE.
     *
     */
    virtual bool finish();

    /**
     * Abandon a running request.
     *
     * This method cancels a running asynchronous operation.
     *
     * @return TRUE, if the operation was cancelled, else FALSE.
     *
     */
    virtual bool abandon();

    /**
     * Set timeout usage.
     *
     * Decides if a timeout is used in retrieving the result.
     *
     * The timeout is only used if you "setUseTimeout(TRUE);". This
     * is to allow a timeout of zero for polling the result.
     *
     * @see Request#setTimeout
     *
     * @param use TRUE, if timeout should be used, else FALSE.
     */
    void setUseTimeout(bool use) { use_timeout=use; };

    /// Returns timeout usage.
    bool useTimeout() { return use_timeout; };

    /**
     * Set timeout.
     *
     * Allows to set the time to wait for a request to finish.
     *
     * You can use a timeout of 0 seconds, 0 microseconds to
     * implement a polling.
     *
     * Note that the timeout is ignored until you call
     * setUseTimeout(TRUE).
     *
     * @see Request#setUseTimeout
     *
     * @param sec seconds to wait
     * @param usec microseconds to wait
     *
     */
    void setTimeout(int sec, int usec=0) { to_sec=sec; to_usec=usec; };

    /**
     * Returns the timeout.
     *
     * Returns the value of the currently set timeout.
     *
     * @param sec Variable to store the seconds.
     * @param usec Variable to store the microseconds.
     *
     */
    void timeout(int &sec, int &usec) { sec=to_sec; usec=to_usec; };

  protected:

    int     expected;
    RunMode mode;
    bool    running;

    int id;
    int all;

    LDAPMessage    *req_result;
    int to_sec;
    int to_usec;
    bool           use_timeout;

  };

  /**
   * A class for search requests.
   *
   * This class enapsulates LDAP search requests, the most common
   * form of LDAP requests.
   *
   */
  class SearchRequest : public Request
  {
  public:

    /**
     * Constructor. Initializes a search request.
     *
     * Creates a new search request object.
     *
     * @param c The connection to use.
     * @param m The mode for the request.
     *
     */
    SearchRequest(Connection &c, RunMode m=Asynchronous);

    /**
     * Constructor. Initializes from a LDAP url.
     *
     * Takes a LDAP url and creates a matching search request.
     *
     * LDAP urls are defined in RFC 2255.
     *
     * @param c The connection to use.
     * @param url The LDAP url.
     * @param m The mode for the request.
     *
     */
    SearchRequest(Connection &c, QString _url, RunMode m=Asynchronous);

    /// Derived method from Request.
    virtual bool execute();

    /**
     * Performs a search.
     *
     * This method is a just a shortcut for:
     *
     * setBase(base); setFilter(filter); execute();
     *
     * @param base The base DN to start the search.
     * @param filter The filter to use.
     *
     * @return TRUE, if the search was started, else FALSE.
     *
     */
    bool search(QString base, QString filter="(objectClass=*)");

    /**
     * Set the base DN for the search.
     *
     * Sets the distinguished name from which the search will be started.
     *
     * @param base the base dn for the search.
     *
     */
    void setBase(QString base) { _base=base; };

    /**
     * Returns the base DN for the search.
     *
     * @return the base DN for the search.
     *
     */
    QString base() { return _base; };

    /**
     * Set the filter for the search.
     *
     * This method sets a filter expression for the
     * search. The search will only return entries matching
     * the filter.
     *
     * The default filter is "(objectClass=*)".
     *
     * @param filter The filter to use.
     *
     */
    void setFilter(QString filter) { _filter=filter; };

    /// Returns the filter for the search.
    QString filter() { return _filter; };

    /**
     * Set the attributes to return.
     *
     * This method sets the list of attributes to return
     * for each entry found in the search.
     *
     * @param list The list of attributes to return.
     *
     */
    void setAttributes(const QStrList &list) { _attributes=list; };

    /// Returns the list of attributes to return for each entry.
    QStrList &attributes() { return _attributes; };

    /**
     * Set the scope of the search.
     *
     * There are three possibele values for the scope of the search:
     *
     * LDAP_SCOPE_BASE: Only search in the base DN.
     * LDAP_SCOPE_ONELEVEL: Search in the children of the base DN.
     * LDAP_SCOPE_SUBTREE: Search the tree originating in the base DN.
     *
     * The default is LDAP_SCOPE_SUBTREE.
     *
     * @param s The scope to use.
     *
     */
    void setScope(int s) { _scope=s; };

    /// Returns the scope of the search.
    int scope() { return _scope; };

    /**
     * Returns the first entry found.
     *
     * After finishing the search, this method returns the first
     * entry found.
     *
     * @return The first entry found.
     */
    Entry first();

    /**
     * Returns the next entry found.
     *
     * @return The next entry found.
     *
     */
    Entry next();

    /**
     * Indicate if more entries are available.
     *
     * @return TRUE, if there are more entries, else FALSE.
     *
     */
    bool end();

    /**
     * Return the search result as an LDIF string.
     *
     * @return the search result as LDIF string.
     */
    QString asLDIF();

  private:

    QString  _base, _filter;
    QStrList _attributes;
    int      _scope, _attrsonly;

    LDAPMessage *entry;

  };

};


#endif
