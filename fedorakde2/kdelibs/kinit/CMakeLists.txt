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

include_directories( ${PROJECT_SOURCE_DIR}/kio/klauncher )

set(kdeinit_SOURCES
    kinit.cpp
    setproctitle.cpp
    )

add_executable(kdeinit ${kdeinit_SOURCES})
target_link_libraries(kdeinit
    kde2::DCOP
    kde2::kparts
    kde2::ksycoca
    kde2::kdeui
    kde2::kio
    kde2::kdecore
    ltdl
    dl
    )

add_executable(kdeinit_wrapper wrapper.c)

add_executable(kshell shell.c)

add_executable(kdeinit_shutdown wrapper.c)

add_executable(lnusertemp lnusertemp.c)

add_executable(kwrapper kwrapper.c)

install(TARGETS kdeinit kdeinit_wrapper kshell kdeinit_shutdown lnusertemp kwrapper
    RUNTIME DESTINATION ${KDE2_BINDIR})
