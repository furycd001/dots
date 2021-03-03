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

set(PUBLIC_HEADERS
    midiout.h
    player.h
    track.h
    midimapper.h
    midfile.h
    dattypes.h
    midistat.h
    deviceman.h
    synthout.h
    fmout.h
    gusout.h
    alsaout.h
    voiceman.h
    notearray.h
    mt32togm.h
    midispec.h
    libkmid.h
    )

set(LIBKMID_SOURCES
    midiout.cc
    player.cc
    track.cc
    midimapper.cc
    midfile.cc
    dattypes.cc
    midistat.cc
    deviceman.cc
    synthout.cc
    fmout.cc
    gusout.cc
    alsaout.cc
    voiceman.cc
    mt32togm.cc
    notearray.cc
    libkmid.cc
    )

kde2_library(libkmid
    SOURCES
        ${LIBKMID_SOURCES}
    INCLUDE_DIRECTORIES
        ${ALSA_INCLUDE_DIRS}
    LIBS
        kde2::kdecore
        ${ALSA_LIBRARY}
    )
