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

set(kICE_SRCS
    accept.c
    authutil.c
    connect.c
    error.c
    getauth.c
    iceauth.c
    listen.c
    listenwk.c
    locking.c
    misc.c
    ping.c
    process.c
    protosetup.c
    register.c
    replywait.c
    setauth.c
    shutdown.c
    watch.c
    transport.c
    globals.c
    )


add_library(kICE OBJECT ${kICE_SRCS})
target_compile_definitions(kICE PRIVATE
    -DICE_t
    -DTRANS_SERVER
    -DTRANS_CLIENT
    -DUNIXCONN
    -DANSICPP
    )
set_target_properties(kICE PROPERTIES
	POSITION_INDEPENDENT_CODE ON
	)
target_include_directories(kICE PRIVATE ${PROJECT_SOURCE_DIR}/dcop)
