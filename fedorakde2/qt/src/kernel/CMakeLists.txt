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

if(ENABLE_XFT)
    list(APPEND QT_DEFINITIONS QT_XFT)
    include_directories(${FREETYPE_INCLUDE_DIR_ft2build})
endif()


set(KERNEL_SRCS
    qabstractlayout.cpp
    qaccel.cpp
    qapplication.cpp
    qapplication_x11.cpp
    qasyncimageio.cpp
    qasyncio.cpp
    qbitmap.cpp
    qclipboard.cpp
    qclipboard_x11.cpp
    qcolor.cpp
    qcolor_p.cpp
    qcolor_x11.cpp
    qconnection.cpp
    qcursor.cpp
    qcursor_x11.cpp
    qdialog.cpp
    qdnd_x11.cpp
    qdragobject.cpp
    qdrawutil.cpp
    qdropsite.cpp
    qevent.cpp
    qfocusdata.cpp
    qfont.cpp
    qfontdatabase.cpp
    qfont_x11.cpp
    qguardedptr.cpp
    qiconset.cpp
    qimage.cpp
    qjpegio.cpp
    qlayout.cpp
    qlayoutengine.cpp
    qlocalfs.cpp
    qmetaobject.cpp
    qmime.cpp
    qmngio.cpp
    qmotifdnd_x11.cpp
    qmovie.cpp
    qnetworkprotocol.cpp
    qnpsupport.cpp
    qobject.cpp
    qpaintdevicemetrics.cpp
    qpaintdevice_x11.cpp
    qpainter.cpp
    qpainter_x11.cpp
    qpalette.cpp
    qpicture.cpp
    qpixmapcache.cpp
    qpixmap.cpp
    qpixmap_x11.cpp
    qpngio.cpp
    qpointarray.cpp
    qpoint.cpp
    qpolygonscanner.cpp
    qprinter.cpp
    qprinter_x11.cpp
    qpsprinter.cpp
    qrect.cpp
    qregion.cpp
    qregion_x11.cpp
    qrichtext.cpp
    qsemimodal.cpp
    qsharedmemory.cpp
    qsignal.cpp
    qsignalmapper.cpp
    qsimplerichtext.cpp
    qsize.cpp
    qsizegrip.cpp
    qsocketnotifier.cpp
    qsound.cpp
    qsound_x11.cpp
    qstyle.cpp
    qstylesheet.cpp
    qthread_unix.cpp
    qtimer.cpp
    qtranslator.cpp
    qurl.cpp
    qurlinfo.cpp
    qurloperator.cpp
    qvariant.cpp
    qwidget.cpp
    qwidgetcreate_x11.cpp
    qwidget_x11.cpp
    qwindow.cpp
    qwmatrix.cpp
)

set(KERNEL_HEADERS
    q1xcompatibility.h
    qabstractlayout.h
    qaccel.h
    qapplication.h
    qasyncimageio.h
    qasyncio.h
    qbitmap.h
    qbrush.h
    qclipboard.h
    qcolor.h
    qconnection.h
    qcursor.h
    qdialog.h
    qdragobject.h
    qdrawutil.h
    qdropsite.h
    qevent.h
    qfocusdata.h
    qfontdatabase.h
    qfont.h
    qfontinfo.h
    qfontmetrics.h
    qgif.h
    qguardedptr.h
    qiconset.h
    qimage.h
    qjpegio.h
    qkeycode.h
    qlayout.h
    qlocalfs.h
    qmetaobject.h
    qmime.h
    qmngio.h
    qmovie.h
    qnamespace.h
    qnetworkprotocol.h
    qobjectdefs.h
    qobjectdict.h
    qobject.h
    qobjectlist.h
    qpaintdevicedefs.h
    qpaintdevice.h
    qpaintdevicemetrics.h
    qpainter.h
    qpalette.h
    qpen.h
    qpicture.h
    qpixmapcache.h
    qpixmap.h
    qpngio.h
    qpointarray.h
    qpoint.h
    qpolygonscanner.h
    qprinter.h
    qrect.h
    qregion.h
    qsemimodal.h
    qsession.h
    qsessionmanager.h
    qsharedmemory.h
    qsignal.h
    qsignalmapper.h
    qsignalslotimp.h
    qsimplerichtext.h
    qsizegrip.h
    qsize.h
    qsizepolicy.h
    qsocketnotifier.h
    qsound.h
    qstyle.h
    qstylesheet.h
    qt.h
    qthread.h
    qtimer.h
    qtranslator.h
    qt_x11.h
    qurl.h
    qurlinfo.h
    qurloperator.h
    qvariant.h
    qvfbhdr.h
    qwidget.h
    qwidgetintdict.h
    qwidgetlist.h
    qwindowdefs.h
    qwindow.h
    qwmatrix.h
)
install(FILES ${KERNEL_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/qt2)

qt2_wrap_cpp(KERNEL_SRCS
    SOURCES
    qapplication_x11.cpp
    qmovie.cpp
    qthread_unix.cpp
    )

qt2_wrap_moc(KERNEL_SRCS
    SOURCES
    qabstractlayout.h
    qaccel.h
    qapplication.h
    qasyncio.h
    qclipboard.h
    qdialog.h
    qdragobject.h
    qguardedptr.h
    qlayout.h
    qtranslator.h
    qnetworkprotocol.h
    qsemimodal.h
    qsessionmanager.h
    qsignalmapper.h
    qsizegrip.h
    qsocketnotifier.h
    qsound.h
    qstyle.h
    qstylesheet.h
    qtimer.h
    qlocalfs.h
    qurloperator.h
    qwidget.h
    )

add_qt2_object_library(kernel
    SOURCES
        ${KERNEL_SRCS}
    COMPILE_DEFINITIONS
        ${QT_DEFINITIONS}
    COMPILE_OPTIONS
        -Wno-format-security
    INCLUDE_DIRS
        $<$<NOT:$<BOOL:${SYSTEM_LIBPNG}>>:$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/3rdparty/libpng>>
	${X11_INCLUDE_DIRS}
	${X11_X11_INCLUDE_PATH}
    GENERATE_MOCS
    )
