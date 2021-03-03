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

add_subdirectory(kssl)

set(kssl_SOURCES
    ksslcertchain.cc
    ksslcertificatefactory.cc
    ksslsettings.cc
    ksslcertdlg.cc
    ksslinfodlg.cc
    kssl.cc
    ksslutils.cc
    ksslconnectioninfo.cc
    ksslpeerinfo.cc
    ksslx509map.cc
    ksslcertificatehome.cc
    ksslcertificate.cc
    ksslpkcs12.cc
    kopenssl.cc
    ksslcertificatecache.cc
    )

set(kssl_HEADERS
    kssl.h
    kssldefs.h
    ksslall.h
    ksslcertificate.h
    ksslcertificatehome.h
    ksslx509map.h
    kopenssl.h
    ksslinfodlg.h
    ksslpeerinfo.h
    ksslcertificatecache.h
    ksslcertificatefactory.h
    ksslpkcs12.h
    ksslcertdlg.h
    ksslcertchain.h
    ksslconnectioninfo.h
    ksslutils.h
    ksslsettings.h
    )

qt2_wrap_cpp(kssl_SOURCES
    SOURCES
        ksslcertdlg.h
        ksslinfodlg.h
    )

kde2_library(libkssl
    SOURCES
        ${kssl_SOURCES}
    LIBS
        kde2::kdeui
        OpenSSL::SSL
        OpenSSL::Crypto
    COMPILE_DEFINITIONS
        -DOPENSSL_API_COMPAT=0x10000000L
    )

install(FILES ${kssl_HEADERS} DESTINATION ${KDE2_INCLUDEDIR}/kssl)
