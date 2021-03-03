# This file is part of the KDE kdebase package
#
# Copyright (C) 2000 David Faure <faure@kde.org>
#
# This file is distributed under the BSD license. See the file "BSD"
# in the subdirectory "licenses" of the package for the full license
# text which has to be applied for this file.
#!/bin/sh

port=`sed -e 's/,.*//' ~/.kxmlrpcd`
auth=`sed -e 's/.*,//' ~/.kxmlrpcd`

cat > cmd.xml <<EOF
<?xml version="1.0"?>
<methodCall>
	<methodName>KDesktopIface.popupExecuteCommand</methodName>
	<params>
		<param>
			<value>$auth</value>
		</param>
	</params>
</methodCall>
EOF

length=`wc -c cmd.xml | sed -e 's/cmd.xml//;s/ //g'`

cat > head.xml <<EOF
POST /kdesktop HTTP/1.0
Content-Type: text/xml
Content-length: $length

EOF

( echo open localhost $port
  sleep 2
  cat head.xml cmd.xml
  sleep 2
) | telnet -8E
