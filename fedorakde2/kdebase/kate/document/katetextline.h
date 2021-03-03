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

#ifndef _KWTEXTLINE_H_
#define _KWTEXTLINE_H_

#include <stdlib.h>

#include <qstring.h>
#include <qarray.h>
#include <qvaluelist.h>

#include <ksharedptr.h>

/**
  FastValueList: QValueList, but with a faster at() like QList
  FVPrivate is needed so that "const" functions can change the
  current position
*/
template<class T>
class FVPrivate
{
public:
    int curpos;
    typedef QValueListConstIterator<T> Iterator;
    Iterator curit;

    FVPrivate() { curpos=-1; };
};

template<class T>
class FastValueList : public QValueList<T>
{
public:
    typedef QValueListIterator<T> Iterator;
    typedef QValueListConstIterator<T> ConstIterator;
protected:
    FVPrivate<T> *fvp;

    Iterator fastat( uint i ) {
        uint num=this->count();
        if (i>=num) {return this->end();}
        if (fvp->curpos<0) { fvp->curpos=0; fvp->curit=this->begin(); }
        uint curpos=(uint) fvp->curpos;
        Iterator curit(fvp->curit.node);
        if (curpos==i) return curit;

        int diff=i-curpos;
        bool forward;
        if (diff<0) diff=-diff;
        if (((uint)diff < i) && ((uint)diff < num-i)) { // start from current node
                forward=i > (uint)curpos;
        } else if (i < num - i) { // start from first node
                curit=this->begin(); diff=i; forward=TRUE;
        } else {                  // start from last node
                curit=this->fromLast(); diff=num - i - 1;
                if (diff<0) diff=0;
                forward=FALSE;
        }
        if (forward) {
                while(diff--) curit++;
        } else {
                while(diff--) curit--;
        }
        fvp->curpos=i; fvp->curit=curit;
        return curit;
    }
    ConstIterator fastat( uint i ) const {
        uint num=this->count();
        if (i>=num) {return this->end();}
        if (fvp->curpos<0) { fvp->curpos=0; fvp->curit=this->begin(); }
        uint curpos=(uint) fvp->curpos;
        ConstIterator curit=fvp->curit;
        if (curpos==i) return curit;

        int diff=i-curpos;
        bool forward;
        if (diff<0) diff=-diff;
        if (((uint)diff < i) && ((uint)diff < num-i)) { // start from current node
                forward=i > (uint)curpos;
        } else if (i < num - i) { // start from first node
                curit=this->begin(); diff=i; forward=TRUE;
        } else {                  // start from last node
                curit=this->fromLast(); diff=num - i - 1;
                if (diff<0) diff=0;
                forward=FALSE;
        }
        if (forward) {
                while(diff--) curit++;
        } else {
                while(diff--) curit--;
        }
        fvp->curpos=i; fvp->curit=curit;
        return curit;
    }

public:
    FastValueList() : QValueList<T>()
    { fvp=new FVPrivate<T>(); }
    FastValueList(const FastValueList<T>& l) : QValueList<T>(l)
    { fvp=new FVPrivate<T>(); }
    ~FastValueList() { delete fvp; }

    Iterator insert( Iterator it, const T& x ) {
      fvp->curpos=-1; return QValueList<T>::insert(it, x);
    }

    Iterator append( const T& x ) {
      fvp->curpos=-1; return QValueList<T>::append( x );
    }
    Iterator prepend( const T& x ) {
      fvp->curpos=-1; return QValueList<T>::prepend( x );
    }

    Iterator remove( Iterator it ) {
      fvp->curpos=-1; return QValueList<T>::remove( it );
    }
    void remove( const T& x ) {
      fvp->curpos=-1; QValueList<T>::remove( x );
    }

    T& operator[] ( uint i ) { this->detach(); return fastat(i); }
    const T& operator[] ( uint i ) const { return *fastat(i); }
    Iterator at( uint i ) { this->detach(); return fastat(i); }
    ConstIterator at( uint i ) const { return ConstIterator( fastat(i) ); }
};


/**
  The TextLine represents a line of text. A text line that contains the
  text, an attribute for each character, an attribute for the free space
  behind the last character and a context number for the syntax highlight.
  The attribute stores the index to a table that contains fonts and colors
  and also if a character is selected.
*/
class TextLine : public KShared
{
  friend class KWBuffer;
  friend class KWBufBlock;

public:
    typedef KSharedPtr<TextLine> Ptr;
    typedef FastValueList<Ptr> List;

public:
    /**
      Creates an empty text line with given attribute and syntax highlight
      context
    */
    TextLine(uchar attribute = 0, int context = 0);
    ~TextLine();

    /**
      Returns the length
    */
    uint length() const {return text.length();}
    /**
      Universal text manipulation method. It can be used to insert, delete
      or replace text.
    */
    void replace(uint pos, uint delLen, const QChar *insText, uint insLen, uchar *insAttribs = 0L);

    /**
      Appends a string of length l to the textline
    */
    void append(const QChar *s, uint l) {replace(text.length(), 0, s, l);}
    /**
      Wraps the text from the given position to the end to the next line
    */
    void wrap(TextLine::Ptr nextLine, uint pos);
    /**
      Wraps the text of given length from the beginning of the next line to
      this line at the given position
    */
    void unWrap(uint pos, TextLine::Ptr nextLine, uint len);
    /**
      Truncates the textline to the new length
    */
    void truncate(uint newLen) { text.truncate(newLen); attributes.resize(text.length()); }
    /**
      Returns the position of the first character which is not a white space
    */
    int firstChar() const;
    /**
      Returns the position of the last character which is not a white space
    */
    int lastChar() const;
    /**
      Removes trailing spaces
    */
    void removeSpaces();
    /**
      Gets the char at the given position
    */
    QChar getChar(uint pos) const;
    /**
      Gets the text. WARNING: it is not null terminated
    */
    const QChar *getText() const {return text.unicode();};
    /**
      Gets a C-like null terminated string
    */
    const QString getString() { return text; };

    /*
      Gets a null terminated pointer to first non space char
    */
    const QChar *firstNonSpace();
    /**
      Returns the x position of the cursor at the given position, which
      depends on the number of tab characters
    */
    int cursorX(uint pos, uint tabChars) const;
    /**
      Is the line starting with the given string
    */
    bool startingWith(QString& match);
    /**
      Is the line ending with the given string
    */
    bool endingWith(QString& match);

    /**
      Sets the attributes from start to end -1
    */
    void setAttribs(uchar attribute, uint start, uint end);
    /**
      Sets the attribute for the free space behind the last character
    */
    void setAttr(uchar attribute);
    /**
      Gets the attribute at the given position
    */
    uchar getAttr(uint pos) const;
    /**
      Gets the attribute for the free space behind the last character
    */
    uchar getAttr() const;
    /**
      Gets the attribute, including the select state, at the given position
    */
    uchar getRawAttr(uint pos) const;
    /**
      Gets the attribute, including the select state, for the free space
      behind the last character
    */
    uchar getRawAttr() const;

    /**
      Sets the syntax highlight context number
    */
    void setContext(int context);
    /**
      Gets the syntax highlight context number
    */
    int getContext() const;

    /**
      Sets the select state from start to end -1
    */
    void select(bool sel, uint start, uint end);
    /**
      Sets the select state from the given position to the end, including
      the free space behind the last character
    */
    void selectEol(bool sel, uint pos);
    /**
      Toggles the select state from start to end -1
    */
    void toggleSelect(uint start, uint end);
    /**
      Toggles the select state from the given position to the end, including
      the free space behind the last character
    */
    void toggleSelectEol(uint pos);
    /**
      Returns the number of selected characters
    */
    int numSelected() const;
    /**
      Returns if the character at the given position is selected
    */
    bool isSelected(uint pos) const;
    /**
      Returns true if the free space behind the last character is selected
    */
    bool isSelected() const;
    /**
      Finds the next selected character, starting at the given position
    */
    int findSelected(uint pos) const;
    /**
      Finds the next unselected character, starting at the given position
    */
    int findUnselected(uint pos) const;
    /**
      Finds the previous selected character, starting at the given position
    */
    int findRevSelected(uint pos) const;
    /**
      Finds the previous unselected character, starting at the given position
    */
    int findRevUnselected(uint pos) const;

    void clearMark () { myMark = 0; };
    void addMark ( uint m );
    void delMark ( uint m );
    uint mark() { return myMark; };

    uchar *getAttribs() { return attributes.data(); }

  protected:
    /**
      The text
    */
    QString text;
    /**
      The attributes
    */
    QArray<uchar> attributes;
    /**
      The attribute of the free space behind the end
    */
    uchar attr;
    /**
      The syntax highlight context
    */
    int ctx;
    /**
      The marks of the current line
    */
    uint myMark;
};

//text attribute constants
const int taSelected = 0x40;
const int taAttrMask = ~taSelected & 0xFF;

#endif //KWTEXTLINE_H

