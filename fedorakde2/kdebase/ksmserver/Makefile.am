# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
# X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
# AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

SUBDIRS = . 

INCLUDES=  $(all_includes)
ksmserver_LDFLAGS = $(all_libraries) $(KDE_RPATH)
ksmserver_LDADD = $(LIB_KDECORE)

bin_PROGRAMS = ksmserver
noinst_HEADERS = global.h server.h shutdown.h

ksmserver_METASOURCES = AUTO
ksmserver_SOURCES = server.cpp shutdown.cpp main.cpp

check_PROGRAMS = testsh
testsh_SOURCES = test.cpp shutdown.cpp

testsh_LDFLAGS = $(all_libraries) $(KDE_RPATH)
testsh_LDADD = $(LIB_KDECORE)

messages:
	$(XGETTEXT) $(ksmserver_SOURCES) -o $(podir)/ksmserver.pot

