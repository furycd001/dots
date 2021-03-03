TEMPLATE	= app
CONFIG		+= qt warn_on release
HEADERS		= nntp.h http.h view.h
SOURCES		= main.cpp \
		  nntp.cpp http.cpp view.cpp
TARGET		= networkprotocol
REQUIRES=network full-config
