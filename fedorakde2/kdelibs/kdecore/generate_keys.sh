#!/bin/sh

## This script extracts the key names from the QT header
##
## Copyright (c) 1999 Nicolas HADACEK (hadacek@kde.org)
## Distributed under the GNU General Public License

# remove files
rm -f kckey.h kckey.cpp kckey_a

# extract key names and code from QT header
sed -n '/enum Key/!d
   		:1
		N
		/}/!b1
		p' $1 \
| sed -n '/=/p' \
| sed -n 's/\s*Key_/{ "/
		s/,.*$/ },/
		s/ =/",/
		s/Key_/Qt::Key_/
		$s/.*/& }/
		p' \
> kckey_a
list=`grep '{ "' kckey_a | sed -e 's#.*{ "\([^"]*\)".*#\1#'`
for i in $list; do 
    if grep -q "i18n(\"QAccel\", \"$i\");" ../common_texts.cpp; then
        sed -e "s#^\(.*\"$i\",.*\$\)#\1 // translated#" kckey_a > kckey_a.new && mv kckey_a.new kckey_a
    fi
done

# write header file
begin_line="// This file has been automatically genrated by \"generate_keys.sh\""
echo -e $begin_line \
        "\n// Distributed under the GNU Library General Public License" \
		"\n#ifndef KCKEY_H" \
		"\n#define KCKEY_H" \
		"\n\ntypedef struct {" \
		"\n\tconst char *name;" \
		"\n\tint code;" \
		"\n} KKeys;" \
		"\n\n#define MAX_KEY_LENGTH           15   // should be calculated (gawk ?)" \
		"\n#define MAX_KEY_MODIFIER_LENGTH   21  // \"SHIFT + CRTL + ALT + \" : " \
		"\n#define MAX_FCTN_LENGTH           50  // arbitrary limit" \
		"\n#define NB_KEYS                " `cat kckey_a | wc -l` \
		"\nextern const KKeys KKEYS[NB_KEYS];" \
		"\n\n#endif" \
> kckey.h

# write source file
echo -e $begin_line \
        "\n// Distributed under the GNU Library General Public License" \
		"\n\n#include <qnamespace.h>" \
		"\n#include \"kckey.h\"" \
		"\n\nconst KKeys KKEYS[NB_KEYS] = {" \
> kckey.cpp

cat kckey_a >> kckey.cpp
				
echo -e "};" >> kckey.cpp

# cleaning
rm -f kckey_a
