TEMPLATE	= app
CONFIG		+= qt warn_on release
HEADERS		= quoteparser.h \
		  richtext.h
SOURCES		= main.cpp \
		  quoteparser.cpp \
		  richtext.cpp
INTERFACES	=
TARGET		= xmlquotes
REQUIRES	= xml large-config
