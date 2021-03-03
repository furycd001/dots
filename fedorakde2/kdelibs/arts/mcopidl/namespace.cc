    /*

    Copyright (C) 2000 Stefan Westerfeld
                       stefan@space.twc.de

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

#include <string.h>
#include <stdio.h>
#include <list>
#include <string>
#include <map>
#include "namespace.h"
#include <assert.h>

using namespace std;

/* generic utilities */

static list<string> symbolToList(string symbol)
{
	list<string> result;
	string current;

	string::iterator si;
	for(si = symbol.begin(); si != symbol.end(); si++)
	{
		if(*si != ':')
		{
			current += *si;
		}
		else
		{
			if(current != "")
				result.push_back(current);

			current = "";
		}
	}

	result.push_back(current);
	return result;
}

static string listToSymbol(list<string>& symlist)
{
	string s;
	list<string>::iterator si;
	for(si = symlist.begin(); si != symlist.end(); si++)
	{
		if(s != "") s += "::";
		s += *si;
	}
	return s;
}

/* ModuleHelper */
static list<string> modulePath;
static map<string,bool> moduleDefinitions;

void ModuleHelper::enter(const char *name)
{
	modulePath.push_back(name);
}

void ModuleHelper::leave()
{
	assert(!modulePath.empty());
	modulePath.pop_back();
}

string prependModulePath(string s)
{
	if(modulePath.empty())
		return s;
	else
		return listToSymbol(modulePath)+"::"+s;
}

void ModuleHelper::define(const char *name)
{
	moduleDefinitions[prependModulePath(name)] = true;
}

char *ModuleHelper::qualify(const char *name)
{
	char *result = 0;

	// TODO: nested namespaces

	string inCurrentModule = prependModulePath(name);
	if(moduleDefinitions[inCurrentModule])
	{
		result = strdup(inCurrentModule.c_str());
	}
	else if(moduleDefinitions[name])
	{
		result = strdup(name);
	}
	else
	{
		fprintf(stderr,"warning: qualifyName failed for %s\n",name);
		result = strdup(name);
	}

	return result;
}


/* NamespaceHelper */
NamespaceHelper::NamespaceHelper(FILE *outputfile) : out(outputfile)
{
}

NamespaceHelper::~NamespaceHelper()
{
	leaveAll();
}

void NamespaceHelper::setFromSymbol(string symbol)
{
	list<string> symlist = symbolToList(symbol);
	symlist.pop_back();

	/* check that the current namespace doesn't contain wrong parts at end */
	list<string>::iterator ni,si;
	ni = currentNamespace.begin();
	si = symlist.begin();
	long wrong = currentNamespace.size();
	while(ni != currentNamespace.end() && si != symlist.end() && *ni == *si)
	{
		ni++;
		si++;
		wrong--;
	}
	while(wrong--)
	{
		fprintf(out,"};\n");
	}

	/* enter new components at the end */
	while(si != symlist.end())
	{
		fprintf(out,"namespace %s {\n",(*si++).c_str());
	}
	currentNamespace = symlist;
}

void NamespaceHelper::leaveAll()
{
	setFromSymbol("unqualified");
}

string NamespaceHelper::printableForm(string symbol)
{
	list<string> symlist = symbolToList(symbol);
	list<string> current = currentNamespace;

	while(!current.empty())
	{
		// namespace longer than symbol?
		assert(!symlist.empty());

		if(*current.begin() == *symlist.begin())
		{
			current.pop_front();
			symlist.pop_front();
		}
		else
		{
			return "::"+symbol;
		}
	}

	return listToSymbol(symlist);
}

string NamespaceHelper::nameOf(string symbol)
{
	if(symbol == "") return "";

	list<string> symlist = symbolToList(symbol);
	return symlist.back();
}

string NamespaceHelper::namespaceOf(string symbol)
{
	list<string> symlist = symbolToList(symbol);
	if(symlist.size() < 2) return "";

	symlist.pop_back();
	return listToSymbol(symlist);
}
