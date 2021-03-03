/* This file is part of the KDE libraries
    Copyright (C) 2000 David Smith  <dsmith@algonet.se>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#ifndef KSHELLCOMPLETION_H
#define KSHELLCOMPLETION_H

#include <qstring.h>
#include <qstringlist.h>

#include "kurlcompletion.h"

class KShellCompletionPrivate;

/**
 * This class does shell-like completion of file names.
 * A string passed to makeCompletion() will be interpreted as a shell 
 * command line. Completion will be done on the last argument on the line. 
 * Returned matches consist of the first arguments (uncompleted) plus the 
 * completed last argument.
 *
 * @short Shell-like completion of file names
 * @author David Smith <dsmith@algonet.se>
 * @version $Id:
 */
class KShellCompletion : public KURLCompletion 
{
	Q_OBJECT

public:
	KShellCompletion();

	/**
	 * Find completions to the given text
	 * The first match is returned and emitted in the signal match() 
	 */
	QString makeCompletion(const QString&);

protected:
	// Called by KCompletion
	void postProcessMatch( QString *match );
	void postProcessMatches( QStringList *matches );

private:
	// Find the part of text that should be completed
	void splitText(const QString &text, QString &text_start, QString &text_compl);
	// Insert quotes and neseccary escapes
	bool quoteText(QString *text, bool force, bool skip_last);
	QString unquote(const QString &text);
		                                                                        
	QString m_text_start; // part of the text that was not completed
	QString m_text_compl; // part of the text that was completed (unchanged)

    QChar m_word_break_char;
	QChar m_quote_char1;
	QChar m_quote_char2;
	QChar m_escape_char;  
				
	KShellCompletionPrivate *d;
};

#endif // KSHELLCOMPLETION_H
