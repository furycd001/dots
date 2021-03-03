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

set(libkhtmldom_SRCS
    dom_misc.cpp html_block.cpp html_inline.cpp css_rule.cpp dom_node.cpp
    html_document.cpp html_list.cpp css_stylesheet.cpp dom_string.cpp html_element.cpp
    html_misc.cpp css_value.cpp dom_text.cpp html_object.cpp dom_doc.cpp
    dom_xml.cpp html_head.cpp dom_element.cpp html_base.cpp html_image.cpp
    html_form.cpp dom2_range.cpp html_table.cpp dom2_traversal.cpp dom2_events.cpp
    dom2_views.cpp
    )

set(libkhtmldom_HEADERS
    dom_exception.h html_base.h html_inline.h
    css_rule.h        dom_html.h        html_block.h      html_list.h
    css_stylesheet.h  dom_misc.h        html_document.h   html_misc.h
    css_value.h       dom_node.h        html_element.h    html_object.h
    dom_core.h        dom_string.h      html_form.h       html_table.h
    dom_doc.h         dom_text.h        html_head.h
    dom_element.h     dom_xml.h         html_image.h
    dom2_range.h      dom2_traversal.h  dom2_events.h
    dom2_views.h
    )

install(FILES ${libkhtmldom_HEADERS} DESTINATION ${KDE2_INCLUDEDIR}/dom)

add_library(libkhtmldom OBJECT ${libkhtmldom_SRCS})

set_target_properties(libkhtmldom PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    )
