TEMPLATE	= app
CONFIG		+= qt warn_on release
HEADERS		= structureparser.h
SOURCES		= tagreader.cpp \
                  structureparser.cpp
INTERFACES	=
TARGET          = tagreader
REQUIRES	= xml
