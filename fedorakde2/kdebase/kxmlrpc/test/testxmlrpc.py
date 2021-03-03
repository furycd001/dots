# This file is part of the KDE kdebase package
#
# Copyright (C) 1999-2000 Kurt Granroth <granroth@kde.org>
#
# This file is distributed under the BSD license. See the file "BSD"
# in the subdirectory "licenses" of the package for the full license
# text which has to be applied for this file.
#
#!/usr/bin/python

from xmlrpclib import *
import os

rc = open(os.environ['HOME'] + '/.kxmlrpcd', 'r')
config = string.split(rc.read(), ',')
port = config[0]
auth = config[1]

server = Server("http://localhost:" + port +"/xmlrpcDCOP")

print server.types.currentTime(auth)
print "Kurt Granroth <" + server.email.lookup(auth, "Kurt Granroth") + ">"
print "Adding Joe User <joeuser@host.com>"
server.email.addAddress(auth, "Joe User", "joeuser@host.com")
print "Joe User <" + server.email.lookup(auth, "Joe User") + ">"

struct = {}
struct['one']  = 1;
struct['two']  = 2;
struct['nine'] = 9;

print server.types.returnMyself(auth, struct)

server = Server("http://localhost:" + port +"/trader")

query = {}
query['ServiceType'] = "text/plain"
query['Constraint']  = "Type == 'Application'"
print server.trader.query(auth, query)
