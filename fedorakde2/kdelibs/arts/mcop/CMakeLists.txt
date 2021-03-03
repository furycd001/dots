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

set(libmcop_SRCS
    common.tcc buffer.cc connection.cc core.cc dispatcher.cc
    iomanager.cc object.cc socketconnection.cc tcpconnection.cc
    unixconnection.cc tcpserver.cc unixserver.cc objectmanager.cc
    factory.cc idlfilereg.cc ifacerepo_impl.cc mcoputils.cc
    startupmanager.cc md5.c md5auth.cc referenceclean.cc
    datapacket.cc asyncstream.cc notification.cc flowsystem.cc
    extensionloader.cc tmpglobalcomm.cc mcopconfig.cc connect.cc
    reference.cc type.cc trader_impl.cc dynamicrequest.cc
    anyref.cc loopback.cc debug.cc delayedreturn.cc thread.cc
    dynamicskeleton.cc
    )

kde2_library(libmcop
    SOURCES
        ${libmcop_SRCS}
    HEADERS
        buffer.h
        common.h
        common.tcc
        connection.h
        core.h
        dispatcher.h
        factory.h
        flowsystem.h
        idlfilereg.h
        ifacerepo_impl.h
        iomanager.h
        mcoputils.h
        object.h
        objectmanager.h
        pool.h
        socketconnection.h
        startupmanager.h
        tcpconnection.h
        tcpserver.h
        type.h
        unixconnection.h
        unixserver.h
        referenceclean.h
        datapacket.h
        asyncstream.h
        notification.h
        extensionloader.h
        mcopconfig.h
        connect.h
        reference.h
        weakreference.h
        dynamicrequest.h
        anyref.h
        debug.h
        delayedreturn.h
        dynamicskeleton.h
        thread.h
        core.idl
    LIBS
        ltdl
        dl
    COMPILE_DEFINITIONS
        -DEXTENSION_DIR="${CMAKE_INSTALL_FULL_LIBDIR}"
        -DTRADER_DIR="${CMAKE_INSTALL_FULL_LIBDIR}/mcop"
    )
