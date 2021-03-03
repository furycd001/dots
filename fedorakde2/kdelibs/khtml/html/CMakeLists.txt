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

set(libkhtmlhtml_SRCS
    htmlparser.cpp htmltokenizer.cpp dtd.cpp html_headimpl.cpp html_blockimpl.cpp
    html_elementimpl.cpp html_inlineimpl.cpp html_documentimpl.cpp html_baseimpl.cpp html_imageimpl.cpp
    html_listimpl.cpp html_miscimpl.cpp html_formimpl.cpp html_objectimpl.cpp html_tableimpl.cpp
    )

set(libkhtmlhtml_HEADERS
    dtd.h html_headimpl.h html_tableimpl.h html_baseimpl.h html_imageimpl.h
    htmlparser.h html_blockimpl.h html_inlineimpl.h html_documentimpl.h html_listimpl.h
    htmltokenizer.h html_elementimpl.h html_miscimpl.h html_formimpl.h html_objectimpl.h
    )

qt2_wrap_cpp(libkhtmlhtml_SRCS
    SOURCES
        html_documentimpl.h
        htmltokenizer.h
    )

get_target_property(KJS_INCLUDES kde2::kjs INCLUDE_DIRECTORIES)

add_library(libkhtmlhtml OBJECT ${libkhtmlhtml_SRCS})
target_include_directories(libkhtmlhtml PUBLIC ${KJS_INCLUDES})
set_target_properties(libkhtmlhtml PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    )
