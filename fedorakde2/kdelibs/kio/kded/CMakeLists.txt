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

set(kded_SOURCES
    kded.cpp
    )

set(kdedmodule_SOURCES
    kbuildservicetypefactory.cpp
    kbuildservicefactory.cpp
    kbuildservicegroupfactory.cpp
    kdedmodule.cpp
    )

set(kbuildsycoca_module_SOURCES
    kbuildsycoca.cpp
    kbuildservicetypefactory.cpp
    kbuildservicefactory.cpp
    kbuildservicegroupfactory.cpp
    kbuildimageiofactory.cpp
    kbuildprotocolinfofactory.cpp
    kctimefactory.cpp
    )

qt2_wrap_cpp(kdedmodule_SOURCES
    SOURCES
        kdedmodule.h
    )

qt2_wrap_cpp(kded_SOURCES
    SOURCES
        kded.h
    )

kde2_library(kdedmodule
    SOURCES
        ${kdedmodule_SOURCES}
    LIBS
        Qt::Qt2
        kde2::DCOP
        kde2::ksycoca
    HEADERS
        kdedmodule.h
)

qt2_wrap_cpp(kbuildsycoca_module_SOURCES
    SOURCES
        kbuildsycoca.h
    )

kde2_kinit_executable(kded
    SOURCES
        ${kded_SOURCES}
    LIBS
        kde2::kdedmodule
    )

kde2_kinit_executable(kbuildsycoca
    SOURCES
        ${kbuildsycoca_module_SOURCES}
    LIBS
        Qt::Qt2
        kde2::ksycoca
    OUTPUT_NAME
        kbuildsycoca
    )

kde2_kinit_executable(kdontchangethehostname
    SOURCES
        khostname.cpp
    LIBS
        kde2::kdecore
    )
target_link_libraries(kdontchangethehostname
    kde2::kdecore
    )

install(FILES kdedmodule.desktop DESTINATION ${KDE2_SERVICETYPESDIR})
