TEMPLATE	= app
CONFIG		+= qt warn_on release
HEADERS		= mainwindow.h \
		  qfileiconview.h \
		../dirview/dirview.h
SOURCES		= main.cpp \
		  mainwindow.cpp \
		  qfileiconview.cpp \
		../dirview/dirview.cpp
TARGET		= fileiconview
DEPENDPATH=../../include
REQUIRES=iconview full-config
