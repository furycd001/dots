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

set(kjs_html_SRCS
    kjs_html.cpp kjs_dom.cpp kjs_window.cpp kjs_navigator.cpp kjs_text.cpp
    kjs_binding.cpp kjs_proxy.cpp kjs_css.cpp kjs_range.cpp kjs_traversal.cpp
    kjs_events.cpp kjs_views.cpp kjs_debugwin.cpp
    )

set(kjs_html_HEADERS
    kjs_html.h kjs_dom.h kjs_window.h kjs_navigator.h kjs_text.h
    kjs_binding.h kjs_proxy.h kjs_css.h kjs_range.h kjs_traversal.h
    kjs_events.h kjs_views.h kjs_debugwin.h
    )

qt2_wrap_cpp(kjs_html_SRCS
    SOURCES
    kjs_debugwin.h
    kjs_window.h
    )

kde2_module(kjs_html
    SOURCES
        ${kjs_html_SRCS}
LIBS
    kde2::kjs kde2::khtml
    )
