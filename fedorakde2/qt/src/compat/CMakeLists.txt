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

set(COMPAT_HEADERS
    qapp.h
    qbitarry.h
    qbttngrp.h
    qchkbox.h
    qclipbrd.h
    qcollect.h
    qcombo.h
    qconnect.h
    qdatetm.h
    qdrawutl.h
    qdstream.h
    qfiledef.h
    qfiledlg.h
    qfileinf.h
    qfontdta.h
    qfontinf.h
    qfontmet.h
    qgrpbox.h
    qintcach.h
    qiodev.h
    qlcdnum.h
    qlined.h
    qmenudta.h
    qmetaobj.h
    qmlined.h
    qmsgbox.h
    qmultilinedit.h
    qobjcoll.h
    qobjdefs.h
    qpaintdc.h
    qpaintd.h
    qpdevmet.h
    qpmcache.h
    qpntarry.h
    qpopmenu.h
    qprndlg.h
    qprogbar.h
    qprogdlg.h
    qpsprn.h
    qpushbt.h
    qradiobt.h
    qrangect.h
    qscrbar.h
    qsocknot.h
    qtabdlg.h
    qtablevw.h
    qtstream.h
    qwidcoll.h
    qwindefs.h
)
install(FILES ${COMPAT_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/qt2)


