/*
   Copyright (C) 1998, 1999 Jochen Wilhelmy
                            digisnap@cs.tu-berlin.de

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#ifndef _HIGHLIGHT_H_
#define _HIGHLIGHT_H_

#include <qlist.h>
#include <qdialog.h>

#include <kcolorbtn.h>
#include <qstrvec.h>
#include <qdict.h>
#include <qregexp.h>
#include "../qt3back/qregexp3.h"
#include <kdebug.h>

class SyntaxDocument;
struct syntaxModeListItem;
struct syntaxContextData;

class QCheckBox;
class QComboBox;
class QLineEdit;

class TextLine;
class Attribute;

class HlItem {
  public:
    HlItem(int attribute, int context);
    virtual ~HlItem();
    virtual bool startEnable(QChar);
    virtual const QChar *checkHgl(const QChar *, int len, bool) = 0;
    QList<HlItem> *subItems;
    int attr;
    int ctx;
};

class HlCharDetect : public HlItem {
  public:
    HlCharDetect(int attribute, int context, QChar);
    virtual const QChar *checkHgl(const QChar *, int len, bool);
  protected:
    QChar sChar;
};

class Hl2CharDetect : public HlItem {
  public:
    Hl2CharDetect(int attribute, int context,  QChar ch1, QChar ch2);
   	Hl2CharDetect(int attribute, int context, const QChar *ch);

    virtual const QChar *checkHgl(const QChar *, int len, bool);
  protected:
    QChar sChar1;
    QChar sChar2;
};

class HlStringDetect : public HlItem {
  public:
    HlStringDetect(int attribute, int context, const QString &, bool inSensitive=false);
    virtual ~HlStringDetect();
    virtual const QChar *checkHgl(const QChar *, int len, bool);
  protected:
    const QString str;
    bool _inSensitive;
};

class HlRangeDetect : public HlItem {
  public:
    HlRangeDetect(int attribute, int context, QChar ch1, QChar ch2);
    virtual const QChar *checkHgl(const QChar *, int len, bool);
  protected:
    QChar sChar1;
    QChar sChar2;
};

class HlKeyword : public HlItem
{
  public:
    HlKeyword(int attribute, int context,bool casesensitive, const QChar *deliminator, uint deliLen);
    virtual ~HlKeyword();

    virtual void addWord(const QString &);
    virtual void addList(const QStringList &);
    virtual const QChar *checkHgl(const QChar *, int len, bool);
    QStringList getList() { return words;};
    virtual bool startEnable(QChar c);

  protected:
    QStringList words;
    QDict<bool> dict;
    bool _caseSensitive;
    const QChar *deliminatorChars;
    uint deliminatorLen;
};

class HlPHex : public HlItem {
  public:
    HlPHex(int attribute,int context);
    virtual const QChar *checkHgl(const QChar *, int len, bool);
};
class HlInt : public HlItem {
  public:
    HlInt(int attribute, int context);
    virtual const QChar *checkHgl(const QChar *, int len, bool);
};

class HlFloat : public HlItem {
  public:
    HlFloat(int attribute, int context);
    virtual const QChar *checkHgl(const QChar *, int len, bool);
};

class HlCInt : public HlInt {
  public:
    HlCInt(int attribute, int context);
    virtual const QChar *checkHgl(const QChar *, int len, bool);
};

class HlCOct : public HlItem {
  public:
    HlCOct(int attribute, int context);
    virtual const QChar *checkHgl(const QChar *, int len, bool);
};

class HlCHex : public HlItem {
  public:
    HlCHex(int attribute, int context);
    virtual const QChar *checkHgl(const QChar *, int len, bool);
};

class HlCFloat : public HlFloat {
  public:
    HlCFloat(int attribute, int context);
    virtual const QChar *checkHgl(const QChar *, int len, bool);
};

class HlLineContinue : public HlItem {
  public:
    HlLineContinue(int attribute, int context);
    virtual bool endEnable(QChar c) {return c == '\0';}
    virtual const QChar *checkHgl(const QChar *, int len, bool);
};

class HlCStringChar : public HlItem {
  public:
    HlCStringChar(int attribute, int context);
    virtual const QChar *checkHgl(const QChar *, int len, bool);
};

class HlCChar : public HlItem {
  public:
    HlCChar(int attribute, int context);
    virtual const QChar *checkHgl(const QChar *, int len, bool);
};

class HlAnyChar : public HlItem {
  public:
    HlAnyChar(int attribute, int context, const QChar* charList, uint len);
    virtual const QChar *checkHgl(const QChar *, int len, bool);
    const QChar* _charList;
    uint _charListLen;
};

class HlRegExpr : public HlItem {
  public:
  HlRegExpr(int attribute, int context,QString expr);
  ~HlRegExpr(){delete Expr;};
  virtual const QChar *checkHgl(const QChar *, int len, bool);
  QRegExp3 *Expr;
  bool handlesLinestart;
};

//--------


//Item Style: color, selected color, bold, italic
class ItemStyle {
  public:
    ItemStyle();
//    ItemStyle(const ItemStyle &);
    ItemStyle(const QColor &, const QColor &, bool bold, bool italic);
    ItemStyle(ItemStyle *its){col=its->col;selCol=its->selCol; bold=its->bold; italic=its->italic;}
//    void setData(const ItemStyle &);
    QColor col;
    QColor selCol;
    int bold;   //boolean value
    int italic; //boolean value
};

typedef QList<ItemStyle> ItemStyleList;

//Item Properties: name, Item Style, Item Font
class ItemData : public ItemStyle {
  public:
    ItemData(const QString  name, int defStyleNum);
    ItemData(const QString  name, int defStyleNum,
      const QColor&, const QColor&, bool bold, bool italic);
    ItemData(ItemData
*itd):ItemStyle((ItemStyle*)itd),name(itd->name),defStyleNum(itd->defStyleNum),defStyle(itd->defStyle){;}
    const QString name;
    int defStyleNum;
    int defStyle; //boolean value
};

typedef QList<ItemData> ItemDataList;

class HlData {
  public:
    HlData(const QString &wildcards, const QString &mimetypes,const QString &identifier);
    ItemDataList itemDataList;
    QString wildcards;
    QString mimetypes;
    QString identifier;
};

typedef QList<HlData> HlDataList;

class HlManager;
class KConfig;

//context
class HlContext {
  public:
    HlContext(int attribute, int lineEndContext,int _lineBeginContext);
    QList<HlItem> items;
    int attr;
    int ctx;
    int lineBeginContext;
};

class Highlight
{
  friend class HlManager;

  public:
    Highlight(syntaxModeListItem *def);
    ~Highlight();

    int doHighlight(int ctxNum, TextLine *);

    KConfig *getKConfig();
    QString getWildcards();
    QString getMimetypes();
    HlData *getData();
    void setData(HlData *);
    void getItemDataList(ItemDataList &);
    void getItemDataList(ItemDataList &, KConfig *);
    void setItemDataList(ItemDataList &, KConfig *);
    QString name() {return iName;}
    QString section() {return iSection;}
    void use();
    void release();
    bool isInWord(QChar c);

    QString getCommentStart() {return cmlStart;};
    QString getCommentEnd()  {return cmlEnd;};
    QString getCommentSingleLineStart() { return cslStart;};

  protected:
    void init();
    void done();
    void makeContextList ();
    void createItemData (ItemDataList &list);
    void readGlobalKeywordConfig();
    void readCommentConfig();
    HlItem *createHlItem(struct syntaxContextData *data, ItemDataList &iDl);
    int lookupAttrName(const QString& name, ItemDataList &iDl);
    ItemDataList internalIDList;
    static const int nContexts = 32;
    HlContext *contextList[nContexts];

    bool noHl;
    bool casesensitive;
    QString weakDeliminator;
    QString deliminator;
    const QChar *deliminatorChars;
    uint deliminatorLen;
    QString cmlStart;
    QString cmlEnd;
    QString cslStart;
    QString iName;
    QString iSection;
    QString iWildcards;
    QString iMimetypes;
    QString identifier;
    int refCount;
};

class HlManager : public QObject {
    Q_OBJECT
  public:
    HlManager();
    ~HlManager();

    static HlManager *self();

    Highlight *getHl(int n);
    int defaultHl();
    int nameFind(const QString &name);

    int wildcardFind(const QString &fileName);
    int mimeFind(const QByteArray &contents, const QString &fname);
    int findHl(Highlight *h) {return hlList.find(h);}

    int makeAttribs(Highlight *, Attribute *, int maxAttribs);

    int defaultStyles();
    QString defaultStyleName(int n);
    void getDefaults(ItemStyleList &);
    void setDefaults(ItemStyleList &);

    int highlights();
    QString hlName(int n);
    QString hlSection(int n);
    void getHlDataList(HlDataList &);
    void setHlDataList(HlDataList &);

    SyntaxDocument *syntax;

  signals:
    void changed();
  protected:
    QList<Highlight> hlList;
    static HlManager *s_pSelf;
};




#endif //_HIGHLIGHT_H_
