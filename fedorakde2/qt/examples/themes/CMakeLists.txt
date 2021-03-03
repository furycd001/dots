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

set(THEMES_SRCS
    themes.cpp
    main.cpp
    ../buttongroups/buttongroups.cpp
    ../lineedits/lineedits.cpp
    ../listboxcombo/listboxcombo.cpp
    ../checklists/checklists.cpp
    ../progressbar/progressbar.cpp
    ../rangecontrols/rangecontrols.cpp
    ../richtext/richtext.cpp
    wood.cpp
    metal.cpp
    )

set(THEMES_HEADERS
    themes.h
    ../buttongroups/buttongroups.h
    ../lineedits/lineedits.h
    ../listboxcombo/listboxcombo.h
    ../checklists/checklists.h
    ../progressbar/progressbar.h
    ../rangecontrols/rangecontrols.h
    ../richtext/richtext.h
    wood.h
    metal.h
    )

qt2_wrap_moc(THEMES_SRCS SOURCES ${THEMES_HEADERS})

add_executable(themes ${THEMES_SRCS})
target_link_libraries(themes Qt::Qt2)
