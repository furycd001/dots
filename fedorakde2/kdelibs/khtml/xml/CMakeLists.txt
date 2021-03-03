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

set(libkhtmlxml_SRCS
    dom_docimpl.cpp dom_nodeimpl.cpp dom_textimpl.cpp dom_elementimpl.cpp dom_stringimpl.cpp
    dom2_rangeimpl.cpp dom2_traversalimpl.cpp xml_tokenizer.cpp dom_xmlimpl.cpp dom2_eventsimpl.cpp
    dom2_viewsimpl.cpp
    )

set(libkhtmlxml_HEADERS
    dom_docimpl.h dom_nodeimpl.h dom_textimpl.h dom_elementimpl.h dom_stringimpl.h
    dom2_rangeimpl.h dom2_traversalimpl.h xml_tokenizer.h dom_xmlimpl.h dom2_eventsimpl.h
    dom2_viewsimpl.h
    )

qt2_wrap_cpp(libkhtmlxml_SRCS
    SOURCES
        dom_docimpl.h
        xml_tokenizer.h
    )

add_library(libkhtmlxml OBJECT ${libkhtmlxml_SRCS})

set_target_properties(libkhtmlxml PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    )
