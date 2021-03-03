TEMPLATE	= app
CONFIG		+= qt warn_on release
HEADERS		= dropsite.h \
		  secret.h
SOURCES		= dropsite.cpp \
		  main.cpp \
		  secret.cpp
TARGET		= dragdrop
DEPENDPATH=../../include
REQUIRES=full-config
