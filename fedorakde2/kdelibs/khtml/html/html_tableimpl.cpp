
/**
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
#include "html_tableimpl.h"

#include "dom_exception.h"
#include "html_documentimpl.h"
#include "dom_node.h"

using namespace DOM;

#include "htmlhashes.h"
#include "khtmlview.h"
#include "khtml_part.h"

#include "css/cssstyleselector.h"
#include "css/cssproperties.h"
#include "css/cssvalues.h"
#include "css/csshelper.h"

#include "rendering/render_table.h"
using namespace khtml;

#include <kdebug.h>

HTMLTableElementImpl::HTMLTableElementImpl(DocumentPtr *doc)
  : HTMLElementImpl(doc)
{
    tCaption = 0;
    head = 0;
    foot = 0;
    firstBody = 0;

    rules = None;
    frame = Void;

    incremental = false;
    m_noBorder = true;

    // reset font color and sizes here, if we don't have strict parse mode.
    // this is 90% compatible to ie and mozilla, and the by way easiest solution...
    // only difference to 100% correct is that in strict mode <font> elements are propagated into tables.
    if ( ownerDocument()->parseMode() != DocumentImpl::Strict ) {
        addCSSProperty( CSS_PROP_FONT_SIZE, CSS_VAL_MEDIUM );
        addCSSProperty( CSS_PROP_COLOR, ownerDocument()->textColor() );
        addCSSProperty( CSS_PROP_FONT_FAMILY, "konq_default" );
    }
}

HTMLTableElementImpl::~HTMLTableElementImpl()
{
}

const DOMString HTMLTableElementImpl::nodeName() const
{
    return "TABLE";
}

ushort HTMLTableElementImpl::id() const
{
    return ID_TABLE;
}

NodeImpl* HTMLTableElementImpl::setCaption( HTMLTableCaptionElementImpl *c )
{
    int exceptioncode;
    NodeImpl* r;
    if(tCaption) {
        replaceChild ( c, tCaption, exceptioncode );
        r = c;
    }
    else
        r = insertBefore( c, firstChild(), exceptioncode );
    tCaption = c;
    return r;
}

NodeImpl* HTMLTableElementImpl::setTHead( HTMLTableSectionElementImpl *s )
{
    int exceptioncode;
    NodeImpl* r;
    if(head) {
        replaceChild( s, head, exceptioncode );
        r = s;
    }
    else if( foot )
        r = insertBefore( s, foot, exceptioncode );
    else if( firstBody )
        r = insertBefore( s, firstBody, exceptioncode );
    else
        r = appendChild( s, exceptioncode );

    head = s;
    return r;
}

NodeImpl* HTMLTableElementImpl::setTFoot( HTMLTableSectionElementImpl *s )
{
    int exceptioncode;
    NodeImpl* r;
    if(foot) {
        replaceChild ( s, foot, exceptioncode );
        r = s;
    }
    else if( firstBody )
        r = insertBefore( s, firstBody, exceptioncode );
    else
        r = appendChild( s, exceptioncode );
    foot = s;
    return r;
}

HTMLElementImpl *HTMLTableElementImpl::createTHead(  )
{
    if(!head)
    {
        int exceptioncode;
        head = new HTMLTableSectionElementImpl(docPtr(), ID_THEAD);
        if(foot)
            insertBefore( head, foot, exceptioncode );
        if(firstBody)
            insertBefore( head, firstBody, exceptioncode);
        else
            appendChild(head, exceptioncode);
    }
    return head;
}

void HTMLTableElementImpl::deleteTHead(  )
{
    if(head) {
        int exceptioncode;
        HTMLElementImpl::removeChild(head, exceptioncode);
    }
    head = 0;
}

HTMLElementImpl *HTMLTableElementImpl::createTFoot(  )
{
    if(!foot)
    {
        int exceptioncode;
        foot = new HTMLTableSectionElementImpl(docPtr(), ID_TFOOT);
        if(firstBody)
            insertBefore( foot, firstBody, exceptioncode );
        else
            appendChild(foot, exceptioncode);
    }
    return foot;
}

void HTMLTableElementImpl::deleteTFoot(  )
{
    if(foot) {
        int exceptioncode;
        HTMLElementImpl::removeChild(foot, exceptioncode);
    }
    foot = 0;
}

HTMLElementImpl *HTMLTableElementImpl::createCaption(  )
{
    if(!tCaption)
    {
        int exceptioncode;
        tCaption = new HTMLTableCaptionElementImpl(docPtr());
        insertBefore( tCaption, firstChild(), exceptioncode );
    }
    return tCaption;
}

void HTMLTableElementImpl::deleteCaption(  )
{
    if(tCaption) {
        int exceptioncode;
        HTMLElementImpl::removeChild(tCaption, exceptioncode);
    }
    tCaption = 0;
}

HTMLElementImpl *HTMLTableElementImpl::insertRow( long /*index*/ )
{
    // ###
    return 0;
}

void HTMLTableElementImpl::deleteRow( long /*index*/ )
{
    // ###
}

NodeImpl *HTMLTableElementImpl::addChild(NodeImpl *child)
{
#ifdef DEBUG_LAYOUT
    kdDebug( 6030 ) << nodeName().string() << "(Table)::addChild( " << child->nodeName().string() << " )" << endl;
#endif

    switch(child->id())
    {
    case ID_CAPTION:
        return setCaption(static_cast<HTMLTableCaptionElementImpl *>(child));
        break;
    case ID_COL:
    case ID_COLGROUP:
        {
        // these have to come before the table definition!
        if(head || foot || firstBody)
            return 0;
        HTMLElementImpl::addChild(child);
        // ###
        }
        return child;
    case ID_THEAD:
        //      if(incremental && !columnPos[totalCols]);// calcColWidth();
        return setTHead(static_cast<HTMLTableSectionElementImpl *>(child));
        break;
    case ID_TFOOT:
        //if(incremental && !columnPos[totalCols]);// calcColWidth();
        return setTFoot(static_cast<HTMLTableSectionElementImpl *>(child));
        break;
    case ID_TBODY:
        //if(incremental && !columnPos[totalCols]);// calcColWidth();
        if(!firstBody)
            firstBody = static_cast<HTMLTableSectionElementImpl *>(child);
        return HTMLElementImpl::addChild( child );
        break;
    }
    return 0;
}

void HTMLTableElementImpl::parseAttribute(AttrImpl *attr)
{
    // ### to CSS!!
    switch(attr->attrId)
    {
    case ATTR_WIDTH:
        if (!attr->value().isEmpty())
            addCSSLength(CSS_PROP_WIDTH, attr->value());
        else
            removeCSSProperty(CSS_PROP_WIDTH);
        break;
    case ATTR_HEIGHT:
        if (!attr->value().isEmpty())
            addCSSLength(CSS_PROP_HEIGHT, attr->value());
        else
            removeCSSProperty(CSS_PROP_HEIGHT);
        break;
    case ATTR_BORDER:
    {
        int border;
        // ### this needs more work, as the border value is not only
        //     the border of the box, but also between the cells
        if(!attr->val())
            border = 0;
        else if(attr->val()->l == 0)
            border = 1;
        else
            border = attr->val()->toInt();
#ifdef DEBUG_DRAW_BORDER
        border=1;
#endif
        m_noBorder = !border;
        addCSSLength(CSS_PROP_BORDER_WIDTH, DOMString( QString::number( border) ) );
#if 0
        // wanted by HTML4 specs
        if(!border)
            frame = Void, rules = None;
        else
            frame = Box, rules = All;
#endif
        break;
    }
    case ATTR_BGCOLOR:
        if (!attr->value().isEmpty())
            addCSSProperty(CSS_PROP_BACKGROUND_COLOR, attr->value());
        else
            removeCSSProperty(CSS_PROP_BACKGROUND_COLOR);
        break;
    case ATTR_BORDERCOLOR:
        if(!attr->value().isEmpty()) {
            addCSSProperty(CSS_PROP_BORDER_COLOR, attr->value());
            addCSSProperty(CSS_PROP_BORDER_TOP_STYLE, CSS_VAL_SOLID);
            addCSSProperty(CSS_PROP_BORDER_LEFT_STYLE, CSS_VAL_SOLID);
            addCSSProperty(CSS_PROP_BORDER_BOTTOM_STYLE, CSS_VAL_SOLID);
            addCSSProperty(CSS_PROP_BORDER_RIGHT_STYLE, CSS_VAL_SOLID);
        }
        break;
    case ATTR_BACKGROUND:
    {
        if (!attr->value().isEmpty()) {
            HTMLDocumentImpl *doc = static_cast<HTMLDocumentImpl *>(ownerDocument());
            QString url = khtml::parseURL( attr->value() ).string();
            if ( doc->view() )
                url = doc->view()->part()->completeURL( url ).url();
            addCSSProperty(CSS_PROP_BACKGROUND_IMAGE, "url('"+url+"')" );
        }
        else
            removeCSSProperty(CSS_PROP_BACKGROUND_IMAGE);
        break;
    }
    case ATTR_FRAME:
#if 0
        if ( strcasecmp( attr->value(), "void" ) == 0 )
            frame = Void;
        else if ( strcasecmp( attr->value(), "border" ) == 0 )
            frame = Box;
        else if ( strcasecmp( attr->value(), "box" ) == 0 )
            frame = Box;
        else if ( strcasecmp( attr->value(), "hsides" ) == 0 )
            frame = Hsides;
        else if ( strcasecmp( attr->value(), "vsides" ) == 0 )
            frame = Vsides;
        else if ( strcasecmp( attr->value(), "above" ) == 0 )
            frame = Above;
        else if ( strcasecmp( attr->value(), "below" ) == 0 )
            frame = Below;
        else if ( strcasecmp( attr->value(), "lhs" ) == 0 )
            frame = Lhs;
        else if ( strcasecmp( attr->value(), "rhs" ) == 0 )
            frame = Rhs;
#endif
        break;
    case ATTR_RULES:
#if 0
        if ( strcasecmp( attr->value(), "none" ) == 0 )
            rules = None;
        else if ( strcasecmp( attr->value(), "groups" ) == 0 )
            rules = Groups;
        else if ( strcasecmp( attr->value(), "rows" ) == 0 )
            rules = Rows;
        else if ( strcasecmp( attr->value(), "cols" ) == 0 )
            rules = Cols;
        else if ( strcasecmp( attr->value(), "all" ) == 0 )
            rules = All;
#endif
        break;
   case ATTR_CELLSPACING:
        if (!attr->value().isEmpty())
            addCSSLength(CSS_PROP_BORDER_SPACING, attr->value());
        else
            removeCSSProperty(CSS_PROP_BORDER_SPACING);
        break;
    case ATTR_CELLPADDING:
        if (!attr->value().isEmpty()) {
            addCSSLength(CSS_PROP_PADDING_TOP, attr->value());
            addCSSLength(CSS_PROP_PADDING_LEFT, attr->value());
            addCSSLength(CSS_PROP_PADDING_BOTTOM, attr->value());
            addCSSLength(CSS_PROP_PADDING_RIGHT, attr->value());
        }
        else {
            removeCSSProperty(CSS_PROP_PADDING_TOP);
            removeCSSProperty(CSS_PROP_PADDING_LEFT);
            removeCSSProperty(CSS_PROP_PADDING_BOTTOM);
            removeCSSProperty(CSS_PROP_PADDING_RIGHT);
        }
        break;
    case ATTR_COLS:
    {
        // ###
#if 0
        int c;
        c = attr->val()->toInt();
        addColumns(c-totalCols);
        break;
#endif
    }
    case ATTR_ALIGN:
        if (!attr->value().isEmpty())
            addCSSProperty(CSS_PROP_FLOAT, attr->value());
        else
            removeCSSProperty(CSS_PROP_FLOAT);
        break;
    case ATTR_VALIGN:
        if (!attr->value().isEmpty())
            addCSSProperty(CSS_PROP_VERTICAL_ALIGN, attr->value());
        else
            removeCSSProperty(CSS_PROP_VERTICAL_ALIGN);
        break;
    case ATTR_NOSAVE:
	break;	
    default:
        HTMLElementImpl::parseAttribute(attr);
    }
}

void HTMLTableElementImpl::attach()
{
    HTMLElementImpl::attach();
}

// --------------------------------------------------------------------------

void HTMLTablePartElementImpl::parseAttribute(AttrImpl *attr)
{
    switch(attr->attrId)
    {
    case ATTR_BGCOLOR:
        if (attr->val())
            addCSSProperty(CSS_PROP_BACKGROUND_COLOR, attr->value() );
        else
            removeCSSProperty(CSS_PROP_BACKGROUND_COLOR);
        break;
    case ATTR_BACKGROUND:
    {
        if (attr->val()) {
            HTMLDocumentImpl *doc = static_cast<HTMLDocumentImpl *>(ownerDocument());
            QString url = khtml::parseURL( attr->value() ).string();
            if ( doc->view() )
                url = doc->view()->part()->completeURL( url ).url();
            addCSSProperty(CSS_PROP_BACKGROUND_IMAGE, "url('"+url+"')" );
        }
        else
            removeCSSProperty(CSS_PROP_BACKGROUND_IMAGE);
        break;
    }
    case ATTR_BORDERCOLOR:
    {
        if(!attr->value().isEmpty()) {
            addCSSProperty(CSS_PROP_BORDER_COLOR, attr->value());
            addCSSProperty(CSS_PROP_BORDER_TOP_STYLE, CSS_VAL_SOLID);
            addCSSProperty(CSS_PROP_BORDER_LEFT_STYLE, CSS_VAL_SOLID);
            addCSSProperty(CSS_PROP_BORDER_BOTTOM_STYLE, CSS_VAL_SOLID);
            addCSSProperty(CSS_PROP_BORDER_RIGHT_STYLE, CSS_VAL_SOLID);
        }
        break;
    }
    case ATTR_VALIGN:
    {
        if (!attr->value().isEmpty())
            addCSSProperty(CSS_PROP_VERTICAL_ALIGN, attr->value());
        else
            removeCSSProperty(CSS_PROP_VERTICAL_ALIGN);
        break;
    }
    case ATTR_NOSAVE:
	break;	
    default:
        HTMLElementImpl::parseAttribute(attr);
    }
}

void HTMLTablePartElementImpl::attach()
{
    HTMLElementImpl::attach();
}

// -------------------------------------------------------------------------

HTMLTableSectionElementImpl::HTMLTableSectionElementImpl(DocumentPtr *doc,
                                                         ushort tagid)
    : HTMLTablePartElementImpl(doc)
{
    _id = tagid;
}

HTMLTableSectionElementImpl::~HTMLTableSectionElementImpl()
{
    nrows = 0;
}

const DOMString HTMLTableSectionElementImpl::nodeName() const
{
    return getTagName(_id);
}

ushort HTMLTableSectionElementImpl::id() const
{
    return _id;
}


// these functions are rather slow, since we need to get the row at
// the index... but they aren't used during usual HTML parsing anyway
HTMLElementImpl *HTMLTableSectionElementImpl::insertRow( long index )
{
    nrows++;

    HTMLTableRowElementImpl *r = new HTMLTableRowElementImpl(docPtr());

    NodeListImpl *children = childNodes();
    int exceptioncode;
    if(!children || (int)children->length() <= index)
        appendChild(r, exceptioncode);
    else {
        NodeImpl *n;
        if(index < 1)
            n = firstChild();
        else
            n = children->item(index);
        insertBefore(r, n, exceptioncode );
    }
    if(children) delete children;
    return r;
}

void HTMLTableSectionElementImpl::deleteRow( long index )
{
    if(index < 0) return;
    NodeListImpl *children = childNodes();
    if(children && (int)children->length() > index)
    {
        nrows--;
        int exceptioncode;
        HTMLElementImpl::removeChild(children->item(index), exceptioncode);
    }
    if(children) delete children;
}

// -------------------------------------------------------------------------

HTMLTableRowElementImpl::HTMLTableRowElementImpl(DocumentPtr *doc)
  : HTMLTablePartElementImpl(doc)
{
}

HTMLTableRowElementImpl::~HTMLTableRowElementImpl()
{
}

const DOMString HTMLTableRowElementImpl::nodeName() const
{
    return "TR";
}

ushort HTMLTableRowElementImpl::id() const
{
    return ID_TR;
}

long HTMLTableRowElementImpl::rowIndex() const
{
    // some complex traversal stuff here to take into account that some rows may be in different sections
    int rIndex = 0;
    const NodeImpl *n = this;
    do {
        while (!n->previousSibling() && !(n->isElementNode() && n->id() == ID_TABLE))
            n = n->parentNode();
        if (n->isElementNode() && n->id() == ID_TABLE)
            n = 0;
        if (n) {
            n = n->previousSibling();
            while (!(n->isElementNode() && n->id() == ID_TR) && n->lastChild())
                n = n->lastChild();
        }

        if (n && n->isElementNode() && n->id() == ID_TR)
            rIndex++;
    }
    while (n && n->isElementNode() && n->id() == ID_TR);

    return rIndex;
}

long HTMLTableRowElementImpl::sectionRowIndex() const
{
    int rIndex = 0;
    const NodeImpl *n = this;
    do {
        n = n->previousSibling();
        if (n && n->isElementNode() && n->id() == ID_TR)
            rIndex++;
    }
    while (n);

    return rIndex;
}

HTMLElementImpl *HTMLTableRowElementImpl::insertCell( long index )
{
    HTMLTableCellElementImpl *c = new HTMLTableCellElementImpl(docPtr(), ID_TD);

    NodeListImpl *children = childNodes();
    int exceptioncode;
    if(!children || (int)children->length() <= index)
        appendChild(c, exceptioncode);
    else {
        NodeImpl *n;
        if(index < 1)
            n = firstChild();
        else
            n = children->item(index);
        insertBefore(c, n, exceptioncode);
    }
    if(children) delete children;
    return c;
}

void HTMLTableRowElementImpl::deleteCell( long index )
{
    if(index < 0) return;
    NodeListImpl *children = childNodes();
    if(children && (int)children->length() > index) {
        int exceptioncode;
        HTMLElementImpl::removeChild(children->item(index), exceptioncode);
    }
    if(children) delete children;
}

// -------------------------------------------------------------------------

HTMLTableCellElementImpl::HTMLTableCellElementImpl(DocumentPtr *doc, int tag)
  : HTMLTablePartElementImpl(doc)
{
  _col = -1;
  _row = -1;
  cSpan = rSpan = 1;
  nWrap = false;
  _id = tag;
  rowHeight = 0;
}

HTMLTableCellElementImpl::~HTMLTableCellElementImpl()
{
}

const DOMString HTMLTableCellElementImpl::nodeName() const
{
    return getTagName(_id);
}

void HTMLTableCellElementImpl::parseAttribute(AttrImpl *attr)
{
    switch(attr->attrId)
    {
    case ATTR_BORDER:
        addCSSLength(CSS_PROP_BORDER_TOP_WIDTH, attr->value());
        addCSSLength(CSS_PROP_BORDER_RIGHT_WIDTH, attr->value());
        addCSSLength(CSS_PROP_BORDER_BOTTOM_WIDTH, attr->value());
        addCSSLength(CSS_PROP_BORDER_LEFT_WIDTH, attr->value());
        break;
    case ATTR_ROWSPAN:
        // ###
        rSpan = attr->val() ? attr->val()->toInt() : 1;
        // limit this to something not causing an overflow with short int
        if(rSpan < 1 || rSpan > 1024) rSpan = 1;
        break;
    case ATTR_COLSPAN:
        // ###
        cSpan = attr->val() ? attr->val()->toInt() : 1;
        // limit this to something not causing an overflow with short int
        if(cSpan < 1 || cSpan > 1024) cSpan = 1;
        break;
    case ATTR_NOWRAP:
        nWrap = (attr->val() != 0);
        break;
    case ATTR_WIDTH:
        if (!attr->value().isEmpty())
            addCSSLength(CSS_PROP_WIDTH, attr->value());
        else
            removeCSSProperty(CSS_PROP_WIDTH);
        break;
    case ATTR_HEIGHT:
        if (!attr->value().isEmpty())
            addCSSLength(CSS_PROP_HEIGHT, attr->value());
        else
            removeCSSProperty(CSS_PROP_HEIGHT);
        break;
    case ATTR_NOSAVE:
	break;	
    default:
        HTMLTablePartElementImpl::parseAttribute(attr);
    }
}

void HTMLTableCellElementImpl::attach()
{
    HTMLElementImpl* p = static_cast<HTMLElementImpl*>(_parent);
    while(p && p->id() != ID_TABLE)
        p = static_cast<HTMLElementImpl*>(p->parentNode());

    if(p) {
        HTMLTableElementImpl* table = static_cast<HTMLTableElementImpl*>(p);
	//
        if(table->m_noBorder && getAttribute(ATTR_BORDER).isNull()) {
            addCSSProperty(CSS_PROP_BORDER_WIDTH, "0");
	}
        if(!table->getAttribute(ATTR_BORDERCOLOR).isNull()) {
            addCSSProperty(CSS_PROP_BORDER_TOP_STYLE, CSS_VAL_SOLID);
            addCSSProperty(CSS_PROP_BORDER_RIGHT_STYLE, CSS_VAL_SOLID);
            addCSSProperty(CSS_PROP_BORDER_BOTTOM_STYLE, CSS_VAL_SOLID);
            addCSSProperty(CSS_PROP_BORDER_LEFT_STYLE, CSS_VAL_SOLID);
	}
    }

    setStyle(ownerDocument()->styleSelector()->styleForElement(this));
    khtml::RenderObject *r = _parent->renderer();
    if(r)
    {
        m_render = khtml::RenderObject::createObject(this);
        if(m_render && m_render->style()->display() == TABLE_CELL)
        {
            RenderTableCell *cell = static_cast<RenderTableCell *>(m_render);
            cell->setRowSpan(rSpan);
            cell->setColSpan(cSpan);
            cell->setNoWrap(nWrap);
        }
        if(m_render) r->addChild(m_render, nextRenderer());
    }

    HTMLElementImpl::attach();
}



// -------------------------------------------------------------------------

HTMLTableColElementImpl::HTMLTableColElementImpl(DocumentPtr *doc, ushort i)
    : HTMLElementImpl(doc)
{
    _id = i;
    _span = 1;
}

HTMLTableColElementImpl::~HTMLTableColElementImpl()
{
}

const DOMString HTMLTableColElementImpl::nodeName() const
{
    return getTagName(_id);
}

ushort HTMLTableColElementImpl::id() const
{
    return _id;
}


NodeImpl *HTMLTableColElementImpl::addChild(NodeImpl *child)
{
#ifdef DEBUG_LAYOUT
    kdDebug( 6030 ) << nodeName().string() << "(Table)::addChild( " << child->nodeName().string() << " )" << endl;
#endif

    switch(child->id())
    {
    case ID_COL:
    {
        // these have to come before the table definition!
        HTMLElementImpl::addChild(child);
        return child;
    }
    default:
        return 0;
        break;
        // ####
    }
    return child;

}

void HTMLTableColElementImpl::parseAttribute(AttrImpl *attr)
{
    switch(attr->attrId)
    {
    case ATTR_SPAN:
        _span = attr->val() ? attr->val()->toInt() : 1;
        break;
    case ATTR_WIDTH:
        if (!attr->value().isEmpty())
            addCSSLength(CSS_PROP_WIDTH, attr->value());
        else
            removeCSSProperty(CSS_PROP_WIDTH);
        break;
    case ATTR_VALIGN:
        if (!attr->value().isEmpty())
            addCSSProperty(CSS_PROP_VERTICAL_ALIGN, attr->value());
        else
            removeCSSProperty(CSS_PROP_VERTICAL_ALIGN);
        break;
    default:
        HTMLElementImpl::parseAttribute(attr);
    }

}

// -------------------------------------------------------------------------

HTMLTableCaptionElementImpl::HTMLTableCaptionElementImpl(DocumentPtr *doc)
  : HTMLTablePartElementImpl(doc)
{
}

HTMLTableCaptionElementImpl::~HTMLTableCaptionElementImpl()
{
}

const DOMString HTMLTableCaptionElementImpl::nodeName() const
{
    return "CAPTION";
}

ushort HTMLTableCaptionElementImpl::id() const
{
    return ID_CAPTION;
}


void HTMLTableCaptionElementImpl::parseAttribute(AttrImpl *attr)
{
    switch(attr->attrId)
    {
    case ATTR_ALIGN:
        if (!attr->value().isEmpty())
            addCSSProperty(CSS_PROP_CAPTION_SIDE, attr->value());
        else
            removeCSSProperty(CSS_PROP_CAPTION_SIDE);
        break;
    default:
        HTMLElementImpl::parseAttribute(attr);
    }

}
