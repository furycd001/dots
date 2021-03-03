/*
 * This file is part of the DOM implementation for KDE.
 *
 * Copyright (C) 1997 Martin Jones (mjones@kde.org)
 *           (C) 1997 Torben Weis (weis@kde.org)
 *           (C) 1998 Waldo Bastian (bastian@kde.org)
 *           (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * $Id$
 */
#ifndef HTML_TABLEIMPL_H
#define HTML_TABLEIMPL_H

#include "html_elementimpl.h"

namespace DOM {

class DOMString;
class HTMLTableElementImpl;
class HTMLTableSectionElementImpl;
class HTMLTableSectionElement;
class HTMLTableRowElementImpl;
class HTMLTableRowElement;
class HTMLTableCellElementImpl;
class HTMLTableCellElement;
class HTMLTableColElementImpl;
class HTMLTableColElement;
class HTMLTableCaptionElementImpl;
class HTMLTableCaptionElement;
class HTMLElement;
class HTMLCollection;

class HTMLTableElementImpl : public HTMLElementImpl
{
public:
    enum Rules {
        None    = 0x00,
        RGroups = 0x01,
        CGroups = 0x02,
        Groups  = 0x03,
        Rows    = 0x05,
        Cols    = 0x0a,
        All     = 0x0f
    };
    enum Frame {
        Void   = 0x00,
        Above  = 0x01,
        Below  = 0x02,
        Lhs    = 0x04,
        Rhs    = 0x08,
        Hsides = 0x03,
        Vsides = 0x0c,
        Box    = 0x0f
    };

    HTMLTableElementImpl(DocumentPtr *doc);
    ~HTMLTableElementImpl();

    virtual const DOMString nodeName() const;
    virtual ushort id() const;

    HTMLTableCaptionElementImpl *caption() const { return tCaption; }
    NodeImpl *setCaption( HTMLTableCaptionElementImpl * );

    HTMLTableSectionElementImpl *tHead() const { return head; }
    NodeImpl *setTHead( HTMLTableSectionElementImpl * );

    HTMLTableSectionElementImpl *tFoot() const { return foot; }
    NodeImpl *setTFoot( HTMLTableSectionElementImpl * );

    HTMLElementImpl *createTHead (  );
    void deleteTHead (  );
    HTMLElementImpl *createTFoot (  );
    void deleteTFoot (  );
    HTMLElementImpl *createCaption (  );
    void deleteCaption (  );
    HTMLElementImpl *insertRow ( long index );
    void deleteRow ( long index );

    // overrides
    virtual NodeImpl *addChild(NodeImpl *child);
    virtual void parseAttribute(AttrImpl *attr);

    virtual void attach();

protected:
    HTMLTableSectionElementImpl *head;
    HTMLTableSectionElementImpl *foot;
    HTMLTableSectionElementImpl *firstBody;
    HTMLTableCaptionElementImpl *tCaption;

    Frame frame;
    Rules rules;

    bool incremental : 1;
    bool m_noBorder  : 1;
    friend class HTMLTableCellElementImpl;
};

// -------------------------------------------------------------------------

class HTMLTablePartElementImpl : public HTMLElementImpl

{
public:
    HTMLTablePartElementImpl(DocumentPtr *doc)
        : HTMLElementImpl(doc)
        { }

    virtual void parseAttribute(AttrImpl *attr);

    void attach();
};

// -------------------------------------------------------------------------

class HTMLTableSectionElementImpl : public HTMLTablePartElementImpl
{
public:
    HTMLTableSectionElementImpl(DocumentPtr *doc, ushort tagid);

    ~HTMLTableSectionElementImpl();

    virtual const DOMString nodeName() const;
    virtual ushort id() const;

    HTMLElementImpl *insertRow ( long index );
    void deleteRow ( long index );

    int numRows() const { return nrows; }

protected:
    ushort _id;
    int nrows;
};

// -------------------------------------------------------------------------

class HTMLTableRowElementImpl : public HTMLTablePartElementImpl
{
public:
    HTMLTableRowElementImpl(DocumentPtr *doc);

    ~HTMLTableRowElementImpl();

    virtual const DOMString nodeName() const;
    virtual ushort id() const;

    long rowIndex() const;
    long sectionRowIndex() const;

    HTMLElementImpl *insertCell ( long index );
    void deleteCell ( long index );

protected:
    int ncols;
};

// -------------------------------------------------------------------------

class HTMLTableCellElementImpl : public HTMLTablePartElementImpl
{
public:
    HTMLTableCellElementImpl(DocumentPtr *doc, int tagId);

    ~HTMLTableCellElementImpl();

    virtual const DOMString nodeName() const;
    virtual ushort id() const { return _id; }

    // ### FIX these two...
    long cellIndex() const { return 0; }

    int col() const { return _col; }
    void setCol(int col) { _col = col; }
    int row() const { return _row; }
    void setRow(int r) { _row = r; }

    // overrides
    virtual void parseAttribute(AttrImpl *attr);
    virtual void attach();

protected:
    int _row;
    int _col;
    int rSpan;
    int cSpan;
    bool nWrap;
    int _id;
    int rowHeight;
};

// -------------------------------------------------------------------------

class HTMLTableColElementImpl : public HTMLElementImpl
{
public:
    HTMLTableColElementImpl(DocumentPtr *doc, ushort i);

    ~HTMLTableColElementImpl();

    virtual const DOMString nodeName() const;
    virtual ushort id() const;

    void setTable(HTMLTableElementImpl *t) { table = t; }

    virtual NodeImpl *addChild(NodeImpl *child);

    // overrides
    virtual void parseAttribute(AttrImpl *attr);

protected:
    // could be ID_COL or ID_COLGROUP ... The DOM is not quite clear on
    // this, but since both elements work quite similar, we use one
    // DOMElement for them...
    ushort _id;
    int _span;
    HTMLTableElementImpl *table;
};

// -------------------------------------------------------------------------

class HTMLTableCaptionElementImpl : public HTMLTablePartElementImpl
{
public:
    HTMLTableCaptionElementImpl(DocumentPtr *doc);

    ~HTMLTableCaptionElementImpl();

    virtual const DOMString nodeName() const;
    virtual ushort id() const;

    virtual void parseAttribute(AttrImpl *attr);
};

}; //namespace

#endif

