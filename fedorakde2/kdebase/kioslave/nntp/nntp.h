/* This file is part of KDE
   Copyright (C) 2000 by Wolfram Diestel <wolfram@steloj.de>

   This is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.
*/

#ifndef _NNTP_H
#define _NNTP_H "$Id: nntp.h,v 1.13 2000/11/21 13:07:27 wolfram Exp $"

#include <qobject.h>
#include <qstring.h>
#include <kio/global.h>
#include <kio/slavebase.h>

/* TODO:
  - test special post command
  - progress information in get, and maybe post
  - remove unnecessary debug stuff
*/

class TCPWrapper: public QObject {

  Q_OBJECT

  public:
    TCPWrapper();
    virtual ~TCPWrapper();

    bool connect(const QString &host, short unsigned int port); // connect to host
    bool connected() { return tcpSocket >= 0; }; // socket exist
    bool disconnect();                           // close socket

    int  read(QByteArray &data, int max_chars);  // read from buffer
    bool readLine(QCString &line);               // read next line
    bool write(const QByteArray &data)   { return writeData(data); };  // write to socket
    bool writeLine(const QCString &line) { return writeData(line + "\r\n"); }; // write to socket

    void setTimeOut(int tm_out);                 // sets a new timeout value,

  protected:
    bool readData();                             // read data from socket into buffer
    bool writeData(const QByteArray &data);      // write data to socket

  signals:
    void error(KIO::Error errnum, const QString &errinfo);

  private:
    int timeOut;          // socket timeout in sec
    int tcpSocket;        // socket handle
    char* thisLine; // line (unread data) position in the buffer
    char* data_end; // end of data in the buffer
    char* buffer;   // buffer for accessing by readLine

    bool readyForReading(); // waits until socket is ready for reading or error
    bool readyForWriting(); // waits until socket is ready for writing or error
};

class NNTPProtocol : public QObject, public KIO::SlaveBase
{

 Q_OBJECT

 public:
  NNTPProtocol (const QCString &pool, const QCString &app );
  virtual ~NNTPProtocol();

  virtual void get(const KURL& url );
  virtual void stat(const KURL& url );
  virtual void listDir(const KURL& url );
  virtual void slave_status();
  virtual void setHost(const QString& host, int port,
        const QString& user, const QString& pass);

  /**
    *  Special command: 1 = post article
    *  it takes no other args, the article data are
    *  requested by dataReq() and should be valid
    *  as in RFC850. It's not checked for correctness here.
    */
  virtual void special(const QByteArray& data);

 protected:

  /**
    *  Send a command to the server. Returns the response code and
    *  the response line
    */
  int send_cmd (const QString &cmd);

  /**
    *  Attempt to properly shut down the NNTP connection by sending
    *  "QUIT\r\n" before closing the socket.
    */
  void nntp_close ();

  /**
    * Attempt to initiate a NNTP connection via a TCP socket.  If no port
    * is passed, port 119 is assumed.
    */
  void nntp_open();

  /**
    * Post article. Invoked by special()
    */
  bool post_article();

 protected slots:
   void socketError(KIO::Error errnum, const QString &errinfo);

 private:
   // connection info
   QString host, pass, user;
   short unsigned int port;
   QString resp_line;
   bool postingAllowed;

   TCPWrapper socket;   // handles the socket stuff
   int eval_resp();     // get server response and check it for general errors

   void fetchGroups();  // fetch all availabel news groups
   bool fetchGroup(QString& group); // fetch all messages from one news group
   void fillUDSEntry(KIO::UDSEntry& entry, const QString& name, int size, bool posting_allowed,
      bool is_article); // makes an UDSEntry with file informations,
                        // used in stat and listDir
   // QString& errorStr(int resp_code); // gives the NNTP error message for a server response code
   void unexpected_response(int res_code, const QString& command); // error handling for unexpected responses
};


#endif










