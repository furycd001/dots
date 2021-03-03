TEMPLATE	= app
CONFIG		= qt warn_on release
OBJECTS_DIR	= .
HEADERS	= command.h \
		  defs.h \
		  formwindow.h \
		  layout.h \
		  mainwindow.h \
		  metadatabase.h \
		  pixmapchooser.h \
		  propertyeditor.h \
		  resource.h \
		  sizehandle.h \
		  orderindicator.h \
		  widgetfactory.h \
		  config.h \
		  hierarchyview.h \
		  listboxeditorimpl.h \
		  connectioneditorimpl.h \
		  newformimpl.h \
		  formlist.h \
		  help.h \
		  editslotsimpl.h \
		  listvieweditorimpl.h \
		  connectionviewerimpl.h \
		  customwidgeteditorimpl.h \
		  paletteeditorimpl.h \
		  styledbutton.h \
		  previewstack.h \
		  iconvieweditorimpl.h \
		  helpdialogimpl.h \
		  topicchooserimpl.h \
		  multilineeditorimpl.h \
		  formsettingsimpl.h \
		  asciivalidator.h \
		  import.h \
		  ../shared/widgetdatabase.h \
		  ../shared/domtool.h \
		  ../integration/kdevelop/kdewidgets.h \
		  qmodules.h \
		  splashloader.h

SOURCES	= command.cpp \
		  formwindow.cpp \
		  defs.cpp \
		  layout.cpp \
		  main.cpp \
		  mainwindow.cpp \
		  metadatabase.cpp \
		  pixmapchooser.cpp \
		  propertyeditor.cpp \
		  resource.cpp \
		  sizehandle.cpp \
		  orderindicator.cpp \
		  widgetfactory.cpp \
		  config.cpp \
		  hierarchyview.cpp \
		  listboxeditorimpl.cpp \
		  connectioneditorimpl.cpp \
		  newformimpl.cpp \
		  formlist.cpp \
		  help.cpp \
		  editslotsimpl.cpp \
		  listvieweditorimpl.cpp \
		  connectionviewerimpl.cpp \
		  customwidgeteditorimpl.cpp \
		  paletteeditorimpl.cpp \
		  styledbutton.cpp \
		  previewstack.cpp \
		  iconvieweditorimpl.cpp \
		  helpdialogimpl.cpp \
		  topicchooserimpl.cpp \
		  multilineeditorimpl.cpp \
		  formsettingsimpl.cpp \
		  asciivalidator.cpp \
		  import.cpp \
		  ../shared/widgetdatabase.cpp \
		  ../integration/kdevelop/kdewidgets.cpp \
		  ../shared/domtool.cpp \
		  splashloader.cpp

INTERFACES	= listboxeditor.ui \
		  connectioneditor.ui \
		  editslots.ui \
		  newform.ui \
		  listvieweditor.ui \
		  connectionviewer.ui \
		  customwidgeteditor.ui \
		  texteditpreview.ui \
		  listviewpreview.ui \
		  paletteeditor.ui \
		  radiopreview.ui \
		  checkboxpreview.ui \
		  widgetpreview.ui \
		  iconvieweditor.ui \
		  preferences.ui \
		  helpdialog.ui \
		  topicchooser.ui \
		  multilineeditor.ui \
		  formsettings.ui \
		  about.ui \
		  pixmapfunction.ui \
		  createtemplate.ui
		
TARGET		= designer
INCLUDEPATH	= ../shared ../util ../../../src/3rdparty/zlib/ $(KDEDIR)/include
unix:LIBS		+= -L$(QTDIR)/lib -lqutil -L$(KDEDIR)/lib -lkdecore -lkdeui -lDCOP
win32:LIBS	+= $(QTDIR)/lib/qutil.lib
DEFINES		+= DESIGNER HAVE_KDE
DESTDIR		= $(QTDIR)/bin
