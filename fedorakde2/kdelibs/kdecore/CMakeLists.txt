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

set(kdecore_SRCS
    libintl.cpp kdebug.cpp netwm.cpp kapp.cpp
    kconfigbase.cpp kstyle.cpp
    kconfig.cpp  ksimpleconfig.cpp kdesktopfile.cpp
    kconfigbackend.cpp kglobalaccel.cpp
    kstddirs.cpp kuniqueapp.cpp ksock.cpp
    kprocess.cpp kprocctrl.cpp klocale.cpp krfcdate.cpp
    kiconeffect.cpp kicontheme.cpp kiconloader.cpp
    kstdaccel.cpp kwin.cpp kwinmodule.cpp krootprop.cpp kcharsets.cpp
    kpixmap.cpp kckey.cpp kaccel.cpp kcrash.cpp
    kurl.cpp kregexp.cpp
    kglobal.cpp kglobalsettings.cpp dmalloc.cpp
    kallocator.cpp kmimesourcefactory.cpp
    kinstance.cpp kpalette.cpp kipc.cpp klibloader.cpp ktempfile.cpp
    ksavefile.cpp krandomsequence.cpp kstringhandler.cpp kcompletion.cpp
    kcmdlineargs.cpp kaboutdata.cpp kcompletionbase.cpp knotifyclient.cpp
    kaudioplayer.cpp kdestyle.cpp kdcoppropertyproxy.cpp
    kextsock.cpp ksockaddr.cpp netsupp.cpp kbufferedio.cpp
    kpixmapprovider.cpp kurldrag.cpp kpixmapsplitter.cpp
    kmdcodec.cpp ksocks.cpp vsnprintf.c
    ksycoca.cpp ksycocadict.cpp ksycocafactory.cpp
    kxmessages.cpp kstartupinfo.cpp kdesktopwidget.cpp
    kprocio.cpp
    )

set(libkdecore_HEADERS
    kconfig.h kconfigdata.h ksimpleconfig.h kconfigbase.h kdesktopfile.h
    kurl.h ksock.h kaboutdata.h kcmdlineargs.h kconfigbackend.h
    kapp.h kuniqueapp.h kcharsets.h kprocess.h kprocctrl.h
    klocale.h kicontheme.h kiconloader.h kdebug.h kstdaccel.h
    kwinmodule.h kwin.h krootprop.h kpixmap.h kaccel.h
    kglobalaccel.h kapplication.h kstandarddirs.h kregexp.h kstyle.h
    kcompletion.h kstringhandler.h kstringhandler.h kstddirs.h kglobal.h
    kglobalsettings.h ksharedptr.h kde_dmalloc.h kallocator.h kcrash.h
    krfcdate.h kuniqueapplication.h kinstance.h kpalette.h kipc.h
    klibloader.h ktempfile.h ksavefile.h krandomsequence.h kstringhandler.h
    knotifyclient.h kiconeffect.h kaudioplayer.h kdestyle.h kdcoppropertyproxy.h
    kdatastream.h netwm.h netwm_def.h netwm_p.h kpixmapprovider.h
    kunload.h kstaticdeleter.h kextsock.h ksockaddr.h kprocio.h
    kasyncio.h kbufferedio.h kurldrag.h kmimesourcefactory.h kpixmapsplitter.h
    ksocks.h ksycoca.h ksycocaentry.h ksycocatype.h kmdcodec.h
    kxmessages.h kstartupinfo.h kdesktopwidget.h
    )

set(libkdecore_NOINST_HEADERS
    kckey.h kregpriv.h kcompletion_private.h netsupp.h ksycocafactory.h
    ksycocadict.h kextsocklookup.h
    )

install(FILES ${libkdecore_HEADERS} DESTINATION ${KDE2_INCLUDEDIR}/kdecore)
install(FILES kmdcodec.h DESTINATION ${KDE2_INCLUDEDIR}/kio)

kde2_wrap_kidl(kdecore_SRCS SOURCES ksycoca.h)

qt2_wrap_cpp(kdecore_SRCS
    SOURCES
        kaccel.h
        kapp.h
        kaudioplayer.h
        kbufferedio.h
        kcompletion.h
        kconfigbase.h
        kconfig.h
        kdesktopfile.h
        kdestyle.h
        kextsock.h
        kextsocklookup.h
        kglobalaccel.h
        klibloader.h
        kprocctrl.h
        kprocess.h
        kprocio.h
        ksimpleconfig.h
        ksockaddr.h
        ksock.h
        kstartupinfo.h
        kstyle.h
        ksycoca.h
        kuniqueapp.h
        kwinmodule.h
        kxmessages.h
    )

qt2_wrap_moc(kdecore_SRCS
    SOURCES
        kasyncio.h
        kdesktopwidget.h
)

kde2_library(libkdecore
    SOURCES
        ${kdecore_SRCS}
    PRIVATE_LIBS
        ltdl
        dl
    COMPILE_OPTIONS
        -Wno-format-security
    LIBS
        Qt::Qt2
        kde2::DCOP
        ${X11_LIBRARIES}
    VERSION
        3.0.0
    )

kde2_library(libkdefakes
    SOURCES
        fakes.c vsnprintf.c
    HEADERS
        kdefakes.h # Just a dummy file
    VERSION
        3.0.0
    )

install(FILES kdebugrc DESTINATION ${KDE2_CONFDIR})
install(FILES kdebug.areas DESTINATION ${KDE2_CONFDIR} RENAME kdebug2.areas)
install(FILES charsets.config DESTINATION ${KDE2_CONFDIR} RENAME kde2-charsets)
install(FILES eventsrc DESTINATION ${KDE2_CONFDIR}/knotify)
install(FILES 40.colors DESTINATION ${KDE2_CONFDIR}/colors RENAME "40 Colors")
install(FILES Web.colors DESTINATION ${KDE2_CONFDIR}/colors RENAME "Web")
install(FILES Royal.colors DESTINATION ${KDE2_CONFDIR}/colors RENAME "Royal")

configure_file(kde-config.cpp.in ${CMAKE_CURRENT_BINARY_DIR}/kde-config.cpp @ONLY)

add_executable(kde-config ${CMAKE_CURRENT_BINARY_DIR}/kde-config.cpp)
target_link_libraries(kde-config kde2::kdecore)
install(TARGETS kde-config  RUNTIME DESTINATION ${KDE2_BINDIR})

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()
