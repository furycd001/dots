/*
    This file is part of the KDE libraries

    Copyright (C) 1999 Lars Knoll (knoll@kde.org)

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

    $Id$

    This widget holds some useful definitions needed for layouting Elements
*/
#ifndef HTML_LAYOUT_H
#define HTML_LAYOUT_H


/*
 * this namespace contains definitions for various types needed for
 * layouting.
 */
namespace khtml
{

    const int UNDEFINED = -1;

    // alignment
    enum VAlign { VNone=0, Bottom, VCenter, Top, Baseline };
    enum HAlign { HDefault, Left, HCenter, Right, HNone = 0 };

    /*
     * %multiLength and %Length
     */
    enum LengthType { Undefined = 0, Variable = 1, Relative, Percent, Fixed, Static };
    struct Length
    {
	Length() { value = 0; type = Variable; }
	Length(LengthType t) { value = 0; type = t; }
	Length(int v, LengthType t)
	    {
		value = v;
		type = t;
	    }
	Length(const Length &l)
	    {
		value = l.value;
		type = l.type;
	    }
	    
	bool operator==(const Length& o) const
	{
	    return type==o.type && value==o.value;
	}
        
        bool operator!=(const Length& o) const
	{
	    return type!=o.type || value!=o.value;
	}
	
	/*
	 * works only for Fixed and Percent, returns -1 otherwise
	 */
	int width(int maxWidth) const
	    {
		switch(type)
		{
		case Fixed:
		    return value;
		case Percent:
		    return maxWidth*value/100;
		case Undefined:
		case Variable:
		    return maxWidth;
		default:
		    return -1;
		}
	    }
	/*
	 * returns the minimum width value which could work...
	 */
	int minWidth(int maxWidth) const
	    {
		switch(type)
		{
		case Fixed:
		    return value;
		case Percent:
		    return maxWidth*value/100;
		case Undefined:
		case Variable:
		default:
		    return 0;
		}
	    }
	bool isUndefined() const { return (type == Undefined); }
	bool isVariable() const { return (type == Variable); }
	bool isRelative() const { return (type == Relative); }
	bool isPercent() const { return (type == Percent); }
	bool isFixed() const { return (type == Fixed); }
        bool isStatic() const { return (type == Static); }
	int value : 29;
	LengthType type : 3;
    };
    
};

#endif
