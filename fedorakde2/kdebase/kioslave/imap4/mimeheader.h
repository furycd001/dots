/***************************************************************************
                          mimeheader.h  -  description
                             -------------------
    begin                : Fri Oct 20 2000
    copyright            : (C) 2000 by Sven Carstens
    email                : s.carstens@gmx.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MIMEHEADER_H
#define MIMEHEADER_H

#include <qlist.h>
#include <qdict.h>

#include "mimehdrline.h"
#include "mimeio.h"
#include "rfcdecoder.h"

/**
  *@author Sven Carstens
  */

class mimeHeader
{
public:
  mimeHeader ();
  virtual ~ mimeHeader ();

  virtual QString internalType ()
  {
    return QString ("mimeHeader");
  };

  virtual void addHdrLine (mimeHdrLine *);
  virtual void outputHeader (mimeIO &);
  virtual void outputPart (mimeIO &);


  QCString outputParameter (QDict < QString > *);

  int parsePart (mimeIO &, QString);
  int parseBody (mimeIO &, QCString &, QString, bool mbox = false);

  // parse a header. returns true if it had a leading 'From ' line
  bool parseHeader (mimeIO &);

  QString getDispositionParm (QCString);
  void setDispositionParm (QCString, QString);
  QDictIterator < QString > getDispositionIterator ();

  QString getTypeParm (QCString);
  void setTypeParm (QCString, QString);
  QDictIterator < QString > getTypeIterator ();

  QCString getType ()
  {
    return contentType;
  };
  void setType (const QCString & _str)
  {
    contentType = _str;
  }

  QCString getDescription ()
  {
    return _contentDescription;
  };
  void setDescription (const QCString & _str)
  {
    _contentDescription = _str;
  }

  QCString getDisposition ()
  {
    return _contentDisposition;
  };
  void setDisposition (const QCString & _str)
  {
    _contentDisposition = _str;
  }

  QCString getEncoding ()
  {
    return contentEncoding;
  };
  void setEncoding (const QCString & _str)
  {
    contentEncoding = _str;
  };

  QCString getMD5 ()
  {
    return contentMD5;
  };
  void setMD5 (const QCString & _str)
  {
    contentMD5 = _str;
  };

  QCString getID ()
  {
    return contentID;
  };
  void setID (const QCString & _str)
  {
    contentID = _str;
  };

  unsigned long getLength ()
  {
    return contentLength;
  };
  void setLength (unsigned long _len)
  {
    contentLength = _len;
  };

  const QString & getPartSpecifier ()
  {
    return partSpecifier;
  };
  void setPartSpecifier (const QString & _str)
  {
    partSpecifier = _str;
  };

  QListIterator < mimeHdrLine > getOriginalIterator ();
  QListIterator < mimeHdrLine > getAdditionalIterator ();
  void setContent (QCString aContent)
  {
    mimeContent = aContent;
  };
  QCString getContent ()
  {
    return mimeContent;
  };

  QCString getBody ()
  {
    return preMultipartBody + postMultipartBody;
  };
  QCString getPreBody ()
  {
    return preMultipartBody;
  };
  void setPreBody (QCString & inBody)
  {
    preMultipartBody = inBody;
  };

  QCString getPostBody ()
  {
    return postMultipartBody;
  };
  void setPostBody (QCString & inBody)
  {
    postMultipartBody = inBody;
    contentLength = inBody.length ();
  };

  mimeHeader *getNestedMessage ()
  {
    return nestedMessage;
  };
  void setNestedMessage (mimeHeader * inPart, bool destroy = true)
  {
    if (nestedMessage && destroy)
      delete nestedMessage;
    nestedMessage = inPart;
  };

//  mimeHeader *getNestedPart() { return nestedPart; };
  void addNestedPart (mimeHeader * inPart)
  {
    nestedParts.append (inPart);
  };
  QListIterator < mimeHeader > getNestedIterator ()
  {
    return QListIterator < mimeHeader > (nestedParts);
  };

  // clears all parts and deletes them from memory
  void clearNestedParts ()
  {
    nestedParts.clear ();
  };

  // clear all parameters to content-type
  void clearTypeParameters ()
  {
    typeList.clear ();
  };

  // clear all parameters to content-disposition
  void clearDispositionParameters ()
  {
    dispositionList.clear ();
  };

  // return the specified body part or NULL
  mimeHeader *bodyPart (const QString &);

#ifdef KMAIL_COMPATIBLE
  ulong msgSize ()
  {
    return contentLength;
  }
  uint numBodyParts ()
  {
    return nestedParts.count ();
  }
  mimeHeader *bodyPart (int which, mimeHeader ** ret = NULL)
  {
    if (ret)
      (*ret) = nestedParts.at (which);
    return nestedParts.at (which);
  }
  void write (const QString &)
  {;
  };
  QString typeStr ()
  {
    return QString (contentType.left (contentType.find ('/')));
  }
  void setTypeStr (const QString & _str)
  {
    contentType = QCString (_str.latin1 ()) + "/" + subtypeStr ().latin1 ();
  }
  QString subtypeStr ()
  {
    return QString (contentType.
                    right (contentType.length () - contentType.find ('/') -
                           1));
  }
  void setSubtypeStr (const QString & _str)
  {
    contentType = QCString (typeStr ().latin1 ()) + "/" + _str.latin1 ();
  }
  QString cteStr ()
  {
    return QString (getEncoding ());
  }
  void setCteStr (const QString & _str)
  {
    setEncoding (_str.latin1 ());
  }
  QString contentDisposition ()
  {
    return QString (_contentDisposition);
  }
  QString body ()
  {
    return QString (postMultipartBody);
  }
  QString charset ()
  {
    return getTypeParm ("charset");
  }
  QString bodyDecoded ();
  void setBodyEncoded (const QByteArray &);
  void setBodyEncodedBinary (const QByteArray &);
  QByteArray bodyDecodedBinary ();
  QString name ()
  {
    return QString (getTypeParm ("name"));
  }
  void setName (const QString & _str)
  {
    setTypeParm ("name", _str);
  }
  QString fileName ()
  {
    return QString (getDispositionParm ("filename"));
  }
  QString contentDescription ()
  {
    return QString (rfcDecoder::decodeRFC2047String (_contentDescription));
  }
  void setContentDescription (const QString & _str)
  {
    _contentDescription = rfcDecoder::encodeRFC2047String (_str).latin1 ();
  }
  QString msgIdMD5 ()
  {
    return QString (contentMD5);
  }
  QString iconName ();
  QString magicSetType (bool aAutoDecode = true);
  QString headerAsString ();
  ulong size ()
  {
    return 0;
  }
  void fromString (const QByteArray &)
  {;
  }
  void setContentDisposition (const QString & _str)
  {
    setDisposition (_str.latin1 ());
  }
#endif

protected:
  static void addParameter (QCString, QDict < QString > *);
  static QString getParameter (QCString, QDict < QString > *);
  static void setParameter (QCString, QString, QDict < QString > *);

  QList < mimeHdrLine > originalHdrLines;

private:
  QList < mimeHdrLine > additionalHdrLines;
  QDict < QString > typeList;
  QDict < QString > dispositionList;
  QCString contentType;
  QCString _contentDisposition;
  QCString contentEncoding;
  QCString _contentDescription;
  QCString contentID;
  QCString contentMD5;
  unsigned long contentLength;
  QCString mimeContent;
  QCString preMultipartBody;
  QCString postMultipartBody;
  mimeHeader *nestedMessage;
  QList < mimeHeader > nestedParts;
  QString partSpecifier;

};

#endif
