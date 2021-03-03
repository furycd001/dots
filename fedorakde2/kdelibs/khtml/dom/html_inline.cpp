/**
 * This file is part of the DOM implementation for KDE.
 *
 * (C) 1999 Lars Knoll (knoll@kde.org)
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
// --------------------------------------------------------------------------

#include "dom_string.h"
#include "html_inline.h"
#include "html_inlineimpl.h"
#include "html_baseimpl.h"
#include "dom_docimpl.h"
using namespace DOM;

#include "htmlhashes.h"

HTMLAnchorElement::HTMLAnchorElement() : HTMLElement()
{
}

HTMLAnchorElement::HTMLAnchorElement(const HTMLAnchorElement &other) : HTMLElement(other)
{
}

HTMLAnchorElement::HTMLAnchorElement(HTMLAnchorElementImpl *impl) : HTMLElement(impl)
{
}

HTMLAnchorElement &HTMLAnchorElement::operator = (const Node &other)
{
    if(other.elementId() != ID_A)
    {
	impl = 0;
	return *this;
    }
    Node::operator = (other);
    return *this;
}

HTMLAnchorElement &HTMLAnchorElement::operator = (const HTMLAnchorElement &other)
{
    HTMLElement::operator = (other);
    return *this;
}

HTMLAnchorElement::~HTMLAnchorElement()
{
}

DOMString HTMLAnchorElement::accessKey() const
{
    if(!impl) return DOMString();
    return ((ElementImpl *)impl)->getAttribute(ATTR_ACCESSKEY);
}

void HTMLAnchorElement::setAccessKey( const DOMString &value )
{
    if(impl) ((ElementImpl *)impl)->setAttribute(ATTR_ACCESSKEY, value);
}

DOMString HTMLAnchorElement::charset() const
{
    if(!impl) return DOMString();
    return ((ElementImpl *)impl)->getAttribute(ATTR_CHARSET);
}

void HTMLAnchorElement::setCharset( const DOMString &value )
{
    if(impl) ((ElementImpl *)impl)->setAttribute(ATTR_CHARSET, value);
}

DOMString HTMLAnchorElement::coords() const
{
    if(!impl) return DOMString();
    return ((ElementImpl *)impl)->getAttribute(ATTR_COORDS);
}

void HTMLAnchorElement::setCoords( const DOMString &value )
{
    if(impl) ((ElementImpl *)impl)->setAttribute(ATTR_COORDS, value);
}

DOMString HTMLAnchorElement::href() const
{
    if(!impl) return DOMString();
    return ((ElementImpl *)impl)->getAttribute(ATTR_HREF);
}

void HTMLAnchorElement::setHref( const DOMString &value )
{
    if(impl) ((ElementImpl *)impl)->setAttribute(ATTR_HREF, value);
}

DOMString HTMLAnchorElement::hreflang() const
{
    if(!impl) return DOMString();
    return ((ElementImpl *)impl)->getAttribute(ATTR_HREFLANG);
}

void HTMLAnchorElement::setHreflang( const DOMString &value )
{
    if(impl) ((ElementImpl *)impl)->setAttribute(ATTR_HREFLANG, value);
}

DOMString HTMLAnchorElement::name() const
{
    if(!impl) return DOMString();
    return ((ElementImpl *)impl)->getAttribute(ATTR_NAME);
}

void HTMLAnchorElement::setName( const DOMString &value )
{
    if(impl) ((ElementImpl *)impl)->setAttribute(ATTR_NAME, value);
}

DOMString HTMLAnchorElement::rel() const
{
    if(!impl) return DOMString();
    return ((ElementImpl *)impl)->getAttribute(ATTR_REL);
}

void HTMLAnchorElement::setRel( const DOMString &value )
{
    if(impl) ((ElementImpl *)impl)->setAttribute(ATTR_REL, value);
}

DOMString HTMLAnchorElement::rev() const
{
    if(!impl) return DOMString();
    return ((ElementImpl *)impl)->getAttribute(ATTR_REV);
}

void HTMLAnchorElement::setRev( const DOMString &value )
{
    if(impl) ((ElementImpl *)impl)->setAttribute(ATTR_REV, value);
}

DOMString HTMLAnchorElement::shape() const
{
    if(!impl) return DOMString();
    return ((ElementImpl *)impl)->getAttribute(ATTR_SHAPE);
}

void HTMLAnchorElement::setShape( const DOMString &value )
{
    if(impl) ((ElementImpl *)impl)->setAttribute(ATTR_SHAPE, value);
}

long HTMLAnchorElement::tabIndex() const
{
    if(!impl) return 0;
    return ((ElementImpl *)impl)->getAttribute(ATTR_TABINDEX).toInt();
}

void HTMLAnchorElement::setTabIndex( long _tabIndex )
{
    if(impl) {
	DOMString value(QString::number(_tabIndex));
        ((ElementImpl *)impl)->setAttribute(ATTR_TABINDEX,value);
    }
}

DOMString HTMLAnchorElement::target() const
{
    if(!impl) return DOMString();
    return ((ElementImpl *)impl)->getAttribute(ATTR_TARGET);
}

void HTMLAnchorElement::setTarget( const DOMString &value )
{
    if(impl) ((ElementImpl *)impl)->setAttribute(ATTR_TARGET, value);
}

DOMString HTMLAnchorElement::type() const
{
    if(!impl) return DOMString();
    return ((ElementImpl *)impl)->getAttribute(ATTR_TYPE);
}

void HTMLAnchorElement::setType( const DOMString &value )
{
    if(impl) ((ElementImpl *)impl)->setAttribute(ATTR_TYPE, value);
}

void HTMLAnchorElement::blur(  )
{
    if(impl && impl->ownerDocument() && impl->ownerDocument()->focusNode()==impl)
        impl->ownerDocument()->setFocusNode(0);
}

void HTMLAnchorElement::focus(  )
{
    if(impl && impl->ownerDocument())
        impl->ownerDocument()->setFocusNode(static_cast<ElementImpl*>(impl));
}

// --------------------------------------------------------------------------

HTMLBRElement::HTMLBRElement() : HTMLElement()
{
}

HTMLBRElement::HTMLBRElement(const HTMLBRElement &other) : HTMLElement(other)
{
}

HTMLBRElement::HTMLBRElement(HTMLBRElementImpl *impl) : HTMLElement(impl)
{
}

HTMLBRElement &HTMLBRElement::operator = (const Node &other)
{
    if(other.elementId() != ID_BR)
    {
	impl = 0;
	return *this;
    }
    Node::operator = (other);
    return *this;
}

HTMLBRElement &HTMLBRElement::operator = (const HTMLBRElement &other)
{
    HTMLElement::operator = (other);
    return *this;
}

HTMLBRElement::~HTMLBRElement()
{
}

DOMString HTMLBRElement::clear() const
{
    if(!impl) return DOMString();
    return ((ElementImpl *)impl)->getAttribute(ATTR_CLEAR);
}

void HTMLBRElement::setClear( const DOMString &value )
{
    if(impl) ((ElementImpl *)impl)->setAttribute(ATTR_CLEAR, value);
}

// --------------------------------------------------------------------------

HTMLFontElement::HTMLFontElement() : HTMLElement()
{
}

HTMLFontElement::HTMLFontElement(const HTMLFontElement &other) : HTMLElement(other)
{
}

HTMLFontElement::HTMLFontElement(HTMLFontElementImpl *impl) : HTMLElement(impl)
{
}

HTMLFontElement &HTMLFontElement::operator = (const Node &other)
{
    if(other.elementId() != ID_FONT)
    {
	impl = 0;
	return *this;
    }
    Node::operator = (other);
    return *this;
}

HTMLFontElement &HTMLFontElement::operator = (const HTMLFontElement &other)
{
    HTMLElement::operator = (other);
    return *this;
}

HTMLFontElement::~HTMLFontElement()
{
}

DOMString HTMLFontElement::color() const
{
    if(!impl) return DOMString();
    return ((ElementImpl *)impl)->getAttribute(ATTR_COLOR);
}

void HTMLFontElement::setColor( const DOMString &value )
{
    if(impl) ((ElementImpl *)impl)->setAttribute(ATTR_COLOR, value);
}

DOMString HTMLFontElement::face() const
{
    if(!impl) return DOMString();
    return ((ElementImpl *)impl)->getAttribute(ATTR_FACE);
}

void HTMLFontElement::setFace( const DOMString &value )
{
    if(impl) ((ElementImpl *)impl)->setAttribute(ATTR_FACE, value);
}

DOMString HTMLFontElement::size() const
{
    if(!impl) return DOMString();
    return ((ElementImpl *)impl)->getAttribute(ATTR_SIZE);
}

void HTMLFontElement::setSize( const DOMString &value )
{
    if(impl) ((ElementImpl *)impl)->setAttribute(ATTR_SIZE, value);
}

// --------------------------------------------------------------------------

HTMLIFrameElement::HTMLIFrameElement() : HTMLElement()
{
}

HTMLIFrameElement::HTMLIFrameElement(const HTMLIFrameElement &other) : HTMLElement(other)
{
}

HTMLIFrameElement::HTMLIFrameElement(HTMLIFrameElementImpl *impl) : HTMLElement(impl)
{
}

HTMLIFrameElement &HTMLIFrameElement::operator = (const Node &other)
{
    if(other.elementId() != ID_IFRAME)
    {
	impl = 0;
	return *this;
    }
    Node::operator = (other);
    return *this;
}

HTMLIFrameElement &HTMLIFrameElement::operator = (const HTMLIFrameElement &other)
{
    HTMLElement::operator = (other);
    return *this;
}

HTMLIFrameElement::~HTMLIFrameElement()
{
}

DOMString HTMLIFrameElement::align() const
{
    if(!impl) return DOMString();
    return ((ElementImpl *)impl)->getAttribute(ATTR_ALIGN);
}

void HTMLIFrameElement::setAlign( const DOMString &value )
{
    if(impl) ((ElementImpl *)impl)->setAttribute(ATTR_ALIGN, value);
}

DOMString HTMLIFrameElement::frameBorder() const
{
    if(!impl) return DOMString();
    return ((ElementImpl *)impl)->getAttribute(ATTR_FRAMEBORDER);
}

void HTMLIFrameElement::setFrameBorder( const DOMString &value )
{
    if(impl) ((ElementImpl *)impl)->setAttribute(ATTR_FRAMEBORDER, value);
}

DOMString HTMLIFrameElement::height() const
{
    if(!impl) return DOMString();
    return ((ElementImpl *)impl)->getAttribute(ATTR_HEIGHT);
}

void HTMLIFrameElement::setHeight( const DOMString &value )
{
    if(impl) ((ElementImpl *)impl)->setAttribute(ATTR_HEIGHT, value);
}

DOMString HTMLIFrameElement::longDesc() const
{
    if(!impl) return DOMString();
    return ((ElementImpl *)impl)->getAttribute(ATTR_LONGDESC);
}

void HTMLIFrameElement::setLongDesc( const DOMString &value )
{
    if(impl) ((ElementImpl *)impl)->setAttribute(ATTR_LONGDESC, value);
}

DOMString HTMLIFrameElement::marginHeight() const
{
    if(!impl) return DOMString();
    return ((ElementImpl *)impl)->getAttribute(ATTR_MARGINHEIGHT);
}

void HTMLIFrameElement::setMarginHeight( const DOMString &value )
{
    if(impl) ((ElementImpl *)impl)->setAttribute(ATTR_MARGINHEIGHT, value);
}

DOMString HTMLIFrameElement::marginWidth() const
{
    if(!impl) return DOMString();
    return ((ElementImpl *)impl)->getAttribute(ATTR_MARGINWIDTH);
}

void HTMLIFrameElement::setMarginWidth( const DOMString &value )
{
    if(impl) ((ElementImpl *)impl)->setAttribute(ATTR_MARGINWIDTH, value);
}

DOMString HTMLIFrameElement::name() const
{
    if(!impl) return DOMString();
    return ((ElementImpl *)impl)->getAttribute(ATTR_NAME);
}

void HTMLIFrameElement::setName( const DOMString &value )
{
    if(impl) ((ElementImpl *)impl)->setAttribute(ATTR_NAME, value);
}

DOMString HTMLIFrameElement::scrolling() const
{
    if(!impl) return DOMString();
    return ((ElementImpl *)impl)->getAttribute(ATTR_SCROLLING);
}

void HTMLIFrameElement::setScrolling( const DOMString &value )
{
    if(impl) ((ElementImpl *)impl)->setAttribute(ATTR_SCROLLING, value);
}

DOMString HTMLIFrameElement::src() const
{
    if(!impl) return DOMString();
    return ((ElementImpl *)impl)->getAttribute(ATTR_SRC);
}

void HTMLIFrameElement::setSrc( const DOMString &value )
{
    if(impl) ((ElementImpl *)impl)->setAttribute(ATTR_SRC, value);
}

DOMString HTMLIFrameElement::width() const
{
    if(!impl) return DOMString();
    return ((ElementImpl *)impl)->getAttribute(ATTR_WIDTH);
}

void HTMLIFrameElement::setWidth( const DOMString &value )
{
    if(impl) ((ElementImpl *)impl)->setAttribute(ATTR_WIDTH, value);
}

// --------------------------------------------------------------------------

HTMLModElement::HTMLModElement() : HTMLElement()
{
}

HTMLModElement::HTMLModElement(const HTMLModElement &other) : HTMLElement(other)
{
}

HTMLModElement::HTMLModElement(HTMLModElementImpl *impl) : HTMLElement(impl)
{
}

HTMLModElement &HTMLModElement::operator = (const Node &other)
{
    if( other.elementId() != ID_INS &&
	other.elementId() != ID_DEL )
    {
	impl = 0;
	return *this;
    }
    Node::operator = (other);
    return *this;
}

HTMLModElement &HTMLModElement::operator = (const HTMLModElement &other)
{
    HTMLElement::operator = (other);
    return *this;
}

HTMLModElement::~HTMLModElement()
{
}

DOMString HTMLModElement::cite() const
{
    if(!impl) return DOMString();
    return ((ElementImpl *)impl)->getAttribute(ATTR_CITE);
}

void HTMLModElement::setCite( const DOMString &value )
{
    if(impl) ((ElementImpl *)impl)->setAttribute(ATTR_CITE, value);
}

DOMString HTMLModElement::dateTime() const
{
    if(!impl) return DOMString();
    return ((ElementImpl *)impl)->getAttribute(ATTR_DATETIME);
}

void HTMLModElement::setDateTime( const DOMString &value )
{
    if(impl) ((ElementImpl *)impl)->setAttribute(ATTR_DATETIME, value);
}

// --------------------------------------------------------------------------

HTMLQuoteElement::HTMLQuoteElement() : HTMLElement()
{
}

HTMLQuoteElement::HTMLQuoteElement(const HTMLQuoteElement &other) : HTMLElement(other)
{
}

HTMLQuoteElement::HTMLQuoteElement(HTMLQuoteElementImpl *impl) : HTMLElement(impl)
{
}

HTMLQuoteElement &HTMLQuoteElement::operator = (const Node &other)
{
    if(other.elementId() != ID_Q)
    {
	impl = 0;
	return *this;
    }
    Node::operator = (other);
    return *this;
}

HTMLQuoteElement &HTMLQuoteElement::operator = (const HTMLQuoteElement &other)
{
    HTMLElement::operator = (other);
    return *this;
}

HTMLQuoteElement::~HTMLQuoteElement()
{
}

DOMString HTMLQuoteElement::cite() const
{
    if(!impl) return DOMString();
    return ((ElementImpl *)impl)->getAttribute(ATTR_CITE);
}

void HTMLQuoteElement::setCite( const DOMString &value )
{
    if(impl) ((ElementImpl *)impl)->setAttribute(ATTR_CITE, value);
}

