#=============================================================================
# Copyright 2017       Helio Chissini de Castro <helio@kde.org>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products
#    derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(WIDGETS_SRCS
    qaction.cpp
    qbutton.cpp
    qbuttongroup.cpp
    qcdestyle.cpp
    qcheckbox.cpp
    qcombobox.cpp
    qcommonstyle.cpp
    qdial.cpp
    qeffects.cpp
    qframe.cpp
    qgrid.cpp
    qgroupbox.cpp
    qhbox.cpp
    qhbuttongroup.cpp
    qheader.cpp
    qhgroupbox.cpp
    qinterlacestyle.cpp
    qlabel.cpp
    qlcdnumber.cpp
    qlineedit.cpp
    qlistbox.cpp
    qlistview.cpp
    qmainwindow.cpp
    qmenubar.cpp
    qmenudata.cpp
    qmotifplusstyle.cpp
    qmotifstyle.cpp
    qmultilineedit.cpp
    qplatinumstyle.cpp
    qpopupmenu.cpp
    qprogressbar.cpp
    qpushbutton.cpp
    qradiobutton.cpp
    qrangecontrol.cpp
    qscrollbar.cpp
    qscrollview.cpp
    qsgistyle.cpp
    qslider.cpp
    qspinbox.cpp
    qsplitter.cpp
    qstatusbar.cpp
    qtabbar.cpp
    qtableview.cpp
    qtabwidget.cpp
    qtextbrowser.cpp
    qtextview.cpp
    qtoolbar.cpp
    qtoolbutton.cpp
    qtooltip.cpp
    qvalidator.cpp
    qvbox.cpp
    qvbuttongroup.cpp
    qvgroupbox.cpp
    qwhatsthis.cpp
    qwidgetstack.cpp
    qwindowsstyle.cpp
    )

set(WIDGETS_HEADERS
    qaction.h
    qbuttongroup.h
    qbutton.h
    qcdestyle.h
    qcheckbox.h
    qcombobox.h
    qcommonstyle.h
    qdial.h
    qframe.h
    qgrid.h
    qgroupbox.h
    qhbox.h
    qhbuttongroup.h
    qheader.h
    qhgroupbox.h
    qinterlacestyle.h
    qlabel.h
    qlcdnumber.h
    qlineedit.h
    qlistbox.h
    qlistview.h
    qmainwindow.h
    qmenubar.h
    qmenudata.h
    qmotifplusstyle.h
    qmotifstyle.h
    qmultilineedit.h
    qplatinumstyle.h
    qpopupmenu.h
    qprogressbar.h
    qpushbutton.h
    qradiobutton.h
    qrangecontrol.h
    qscrollbar.h
    qscrollview.h
    qsgistyle.h
    qslider.h
    qspinbox.h
    qsplitter.h
    qstatusbar.h
    qtabbar.h
    qtableview.h
    qtabwidget.h
    qtextbrowser.h
    qtextview.h
    qtoolbar.h
    qtoolbutton.h
    qtooltip.h
    qvalidator.h
    qvbox.h
    qvbuttongroup.h
    qvgroupbox.h
    qwhatsthis.h
    qwidgetstack.h
    qwindowsstyle.h
)

qt2_wrap_cpp(WIDGETS_SRCS
    SOURCES
    qeffects.cpp
    qmainwindow.cpp
    qtoolbar.cpp
    qtooltip.cpp
    qwhatsthis.cpp
)

qt2_wrap_moc(WIDGETS_SRCS
    SOURCES ${WIDGETS_HEADERS})

add_qt2_object_library(widgets
    SOURCES
        ${WIDGETS_SRCS}
    COMPILE_DEFINITIONS
        ${QT_DEFINITIONS}
    )

install(FILES ${WIDGETS_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/qt2)
