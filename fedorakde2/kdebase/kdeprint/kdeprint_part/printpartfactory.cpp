/*
 *  This file is part of the KDE libraries
 *  Copyright (c) 2001 Michael Goffioul <goffioul@imec.be>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License version 2 as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA 02111-1307, USA.
 **/

#include "printpartfactory.h"
#include "printpart.h"

#include <kinstance.h>
#include <kaboutdata.h>

extern "C"
{
	void* init_libkdeprint_part()
	{
		return new PrintPartFactory;
	}
};

KInstance*	PrintPartFactory::m_instance = 0L;

PrintPartFactory::PrintPartFactory(QObject *parent, const char *name)
: KLibFactory(parent, name)
{
}

PrintPartFactory::~PrintPartFactory()
{
	if (m_instance)
	{
		delete m_instance->aboutData();
		delete m_instance;
	}
	m_instance = 0;
}

QObject* PrintPartFactory::createObject(QObject *parent, const char *name, const char*, const QStringList&)
{
	return new PrintPart((QWidget*)parent, name);
}

KInstance* PrintPartFactory::instance()
{
	if (!m_instance)
		m_instance = new KInstance(aboutData());
	return m_instance;
}

KAboutData* PrintPartFactory::aboutData()
{
	return new KAboutData("kdeprint_part", "A Konqueror Plugin for Print Management", "0.1");
}

#include "printpartfactory.moc"
