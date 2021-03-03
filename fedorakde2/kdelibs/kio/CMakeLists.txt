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

add_subdirectory(klauncher)
add_subdirectory(kded)
add_subdirectory(file)
add_subdirectory(ftp)
add_subdirectory(http)
add_subdirectory(pics)
if (BUILD_TESTS)
    add_subdirectory(tests)
endif()
#add_subdirectory(ksendbugmail)
add_subdirectory(kpac)
add_subdirectory(gzip)
add_subdirectory(bzip2)
add_subdirectory(kssld)

set(ksycoca_SOURCES
    chmodjob.cpp
    kdirwatch.cpp
    ktrader.cpp
    ktraderparse.cpp
    ktraderparsetree.cpp
    kservicefactory.cpp
    kservicetype.cpp
    kmimetype.cpp
    kservicegroup.cpp
    kuserprofile.cpp
    kservicegroupfactory.cpp
    kservice.cpp
    kservicetypefactory.cpp
    kurifilter.cpp
    kfilterbase.cpp
    kfilterdev.cpp
    kshellcompletion.cpp
    kurlcompletion.cpp
    kautomount.cpp
    krun.cpp
    kfileitem.cpp
    kdirlister.cpp
    kimageio.cpp
    lex.c
    yacc.c
    kscan.cpp
    ktar.cpp
    previewjob.cpp
    )

set(kio_SOURCES
    kshred.cpp
    kprotocolmanager.cpp
    slave.cpp
    slaveinterface.cpp
    sessiondata.cpp
    scheduler.cpp
    connection.cpp
    job.cpp
    global.cpp
    slaveconfig.cpp
    netaccess.cpp
    paste.cpp
    kmimemagic.cpp
    tcpslavebase.cpp
    slavebase.cpp
    passdlg.cpp
    progressbase.cpp
    defaultprogress.cpp
    statusbarprogress.cpp
    kdirnotify.cpp
    observer.cpp
    kemailsettings.cpp
    authinfo.cpp
    kprotocolinfo.cpp
    kprotocolinfofactory.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/kdirnotify_stub.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/kdirnotify_skel.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/uiserver_stub.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/observer_skel.cpp
    )

kde2_kidl(kdirnotify.h uiserver.h observer.h)

set(kio_uiserver_SOURCES
    uiserver.cpp
    renamedlg.cpp
    skipdlg.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/uiserver_skel.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/observer_stub.cpp
    )

set(kio_uiserver_HEADERS
    uiserver.h
    renamedlg.h
    skipdlg.h
    )

set(global_HEADERS
    kservicetype.h kmimetype.h kmimemagic.h kservice.h
    krun.h kdirwatch.h kautomount.h kuserprofile.h
    kshred.h ktar.h ktrader.h kurifilter.h kurlcompletion.h
    kshellcompletion.h kfileitem.h kdirlister.h kservicegroup.h
    progressbase.h defaultprogress.h statusbarprogress.h
    kimageio.h kdirnotify.h kurlpixmapprovider.h
    kprotocolinfo.h kprotocolmanager.h
    kfilterbase.h kfilterdev.h kemailsettings.h kscan.h
    )

set(kio_HEADERS
    connection.h
    slaveinterface.h slave.h slaveconfig.h
    sessiondata.h global.h passdlg.h
    netaccess.h scheduler.h
    jobclasses.h paste.h slavebase.h
    progressbase.h defaultprogress.h
    statusbarprogress.h uiserver.h
    ${CMAKE_CURRENT_BINARY_DIR}/uiserver_stub.h
    ${CMAKE_CURRENT_BINARY_DIR}/kdirnotify_stub.h
    tcpslavebase.h
    renamedlg.h skipdlg.h
    observer.h chmodjob.h
    kpac.h kmdbase.h authinfo.h
    ioslave_defaults.h http_slave_defaults.h previewjob.h
)

set(kio_MOCABLE_HEADERS
    connection.h
    slaveinterface.h slave.h slaveconfig.h
    sessiondata.h passdlg.h
    netaccess.h scheduler.h
    jobclasses.h
    progressbase.h defaultprogress.h
    statusbarprogress.h uiserver.h
    renamedlg.h skipdlg.h
    observer.h chmodjob.h
    previewjob.h
)

install(FILES ${kio_HEADERS} job.h DESTINATION ${KDE2_INCLUDEDIR}/kio)
install(FILES ${global_HEADERS} DESTINATION ${KDE2_INCLUDEDIR}/ksycoca)

set(global_NOINST_HEADERS
    kservicetypefactory.h
    kservicefactory.h
    kmessageboxwrapper.h
    ktraderparse.h
    ktraderparsetree.h
    kimageiofactory.h
    kprotocolinfofactory.h
    )

qt2_wrap_cpp(moc_SOURCES
    SOURCES
    krun.h kdirwatch.h kautomount.h
    kshred.h ktrader.h kurifilter.h kurlcompletion.h
    kshellcompletion.h kdirlister.h
    progressbase.h defaultprogress.h statusbarprogress.h
    kfilterbase.h kscan.h
    )

qt2_wrap_cpp(kio_SOURCES
    SOURCES
        ${kio_MOCABLE_HEADERS}
    )

qt2_wrap_cpp(kio_uiserver_SOURCES
    SOURCES
        ${kio_uiserver_HEADERS}
    )

kde2_library(libkio
    SOURCES
        ${moc_SOURCES}
        ${kio_SOURCES}
    LIBS
        kde2::kdeui
        kde2::kdesu
        kde2::kssl
        ZLIB::ZLIB
    INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/kded
    VERSION
        3.0.0
    )

kde2_library(libksycoca
    SOURCES
        ${moc_SOURCES}
        ${ksycoca_SOURCES}
    LIBS
        kde2::kio
        ltdl
    VERSION
        3.0.0
    )

kde2_kinit_executable(kio_uiserver
    SOURCES
        ${kio_uiserver_SOURCES}
    LIBS
        kde2::ksycoca
        kde2::kssl
        kde2::kdesu
    )

install(FILES magic DESTINATION ${KDE2_MIMEDIR} RENAME magic2)

install(FILES
    application.desktop kurifilterplugin.desktop kcomprfilter.desktop kscan.desktop
    DESTINATION ${KDE2_SERVICETYPESDIR}
    )

install(FILES
    kio_uiserver.desktop kmailservice.protocol telnet.protocol rlogin.protocol
    DESTINATION ${KDE2_SERVICESDIR}
    )

install(FILES
    kioslave.upd useragent.pl proxytype.pl
    DESTINATION ${KDE2_DATADIR}/kconf_update
    )
