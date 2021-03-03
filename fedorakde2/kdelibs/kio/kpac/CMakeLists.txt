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

set(libkpac_SOURCES
    kproxybindings.cpp
    kpac_impl.cpp
    kpac_downloader.cpp
    kpac_discovery.cpp
    )

set(kpac_noinst_HEADERS
    kproxybindings.h
    kpac_impl.h
    kpac_downloader.h
    kpac_discovery.h
    dhcp.h
    )

qt2_wrap_cpp(libkpac_SOURCES
    SOURCES
        kpac_discovery.h
        kpac_downloader.h
    )

kde2_module(libkpac
    SOURCES
        ${libkpac_SOURCES}
    LIBS
        kde2::kio
        kde2::kjs
    )

add_executable(kpac_dhcp_helper kpac_dhcp_helper.c)
install(TARGETS kpac_dhcp_helper RUNTIME DESTINATION ${KDE2_BINDIR})
