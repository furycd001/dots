
printers.cpp:
	: > printers.cpp
	for i in `find /usr/share/cups/model -type f`; do echo "// $$i" >> printers.cpp; perl ./readppd < $$i >> printers.cpp; done
	iconv -flatin1 -tutf-8 printers.cpp > printers.new && mv printers.new printers.cpp

messages: printers.cpp
	$(XGETTEXT) --no-location printers.cpp -o $(podir)/../ppdtranslations.pot
