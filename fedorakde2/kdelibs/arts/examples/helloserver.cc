    /*

    Copyright (C) 1999 Stefan Westerfeld
                       stefan@space.twc.de

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    Permission is also granted to link this program with the Qt
    library, treating Qt like a library that normally accompanies the
    operating system kernel, whether or not that is in fact the case.

    */

#include "core.h"
#include "hello_impl.h"

#include <stdio.h>
#include <vector>
#include <string>

using namespace std;
using namespace Arts;

int main()
{
	Dispatcher dispatcher(0,Dispatcher::startUnixServer);
	Hello server;

	string reference = server.toString();
	printf("%s\n",reference.c_str());

	Hello h = Reference(reference);
	if(!h.isNull())
		h.hello("local test");
	else
		printf("Local access to the Hello_impl object failed?\n");

	dispatcher.run();
	return 0;
}
