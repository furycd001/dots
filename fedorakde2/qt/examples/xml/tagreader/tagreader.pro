TEMPLATE	= app
CONFIG          += qt warn_on release
win32:CONFIG	+= console
HEADERS		= structureparser.h
SOURCES		= tagreader.cpp \
                  structureparser.cpp
INTERFACES	=
TARGET          = tagreader
REQUIRES	= xml
