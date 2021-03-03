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

set(TOOLS_SRCS
    qdir_unix.cpp
    qfile_unix.cpp
    qfileinfo_unix.cpp
    qbig5codec.cpp
    qbitarray.cpp
    qbuffer.cpp
    qcollection.cpp
    qcstring.cpp
    qdatastream.cpp
    qdatetime.cpp
    qdir.cpp
    qeucjpcodec.cpp
    qeuckrcodec.cpp
    qfile.cpp
    qfileinfo.cpp
    qgarray.cpp
    qgbkcodec.cpp
    qgcache.cpp
    qgdict.cpp
    qglist.cpp
    qglobal.cpp
    qgvector.cpp
    qiodevice.cpp
    qjiscodec.cpp
    qjpunicode.cpp
    qmap.cpp
    qregexp.cpp
    qrtlcodec.cpp
    qsjiscodec.cpp
    qstring.cpp
    qstringlist.cpp
    qtextcodec.cpp
    qtextstream.cpp
    qtsciicodec.cpp
    qutfcodec.cpp
)

if(UNIX)
    list(APPEND TOOLS_SRCS qfile_unix.cpp)
elseif(MSVC)
    list(APPEND TOOLS_SRCS qfile_win.cpp)
endif()

set(TOOLS_HEADERS
    qarray.h
    qasciicache.h
    qasciidict.h
    qbig5codec.h
    qbitarray.h
    qbuffer.h
    qcache.h
    qcollection.h
    qconfig-dist.h
    qconfig.h
    qconfig-large.h
    qconfig-medium.h
    qconfig-minimal.h
    qconfig-small.h
    qcstring.h
    qdatastream.h
    qdatetime.h
    qdict.h
    qdir.h
    qeucjpcodec.h
    qeuckrcodec.h
    qfeatures.h
    qfile.h
    qfileinfo.h
    qgarray.h
    qgbkcodec.h
    qgcache.h
    qgdict.h
    qgeneric.h
    qglist.h
    qglobal.h
    qgvector.h
    qintcache.h
    qintdict.h
    qiodevice.h
    qjiscodec.h
    qjpunicode.h
    qkoi8codec.h
    qlist.h
    qmap.h
    qmodules.h
    qptrdict.h
    qqueue.h
    qregexp.h
    qrtlcodec.h
    qshared.h
    qsjiscodec.h
    qsmartptr.h
    qsortedlist.h
    qstack.h
    qstring.h
    qstringlist.h
    qstrlist.h
    qstrvec.h
    qtextcodec.h
    qtextstream.h
    qtl.h
    qtsciicodec.h
    qutfcodec.h
    qvaluelist.h
    qvaluestack.h
    qvector.h
)
install(FILES ${TOOLS_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/qt2)

add_qt2_object_library(tools
    SOURCES
        ${TOOLS_SRCS}
    COMPILE_DEFINITIONS
        ${QT_DEFINITIONS}
    COMPILE_OPTIONS
        -Wno-format-security
    )
