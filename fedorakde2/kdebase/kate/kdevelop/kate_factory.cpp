/*
 * Copyright (C) 2001  <kurt@granroth.org>
 */


#include <kinstance.h>
#include <kaction.h>


#include "kate_factory.h"
#include "kate_part.h"


// It's usually safe to leave the factory code alone.. with the
// notable exception of the KAboutData data
#include <kaboutdata.h>
#include <klocale.h>

KInstance* KatePartFactory::s_instance = 0L;
KAboutData* KatePartFactory::s_about = 0L;

KatePartFactory::KatePartFactory()
    : KParts::Factory()
{}


KatePartFactory::~KatePartFactory()
{
  delete s_instance;
  delete s_about;

  s_instance = 0L;
}


QObject* KatePartFactory::createObject(QObject *parent, const char *name,
    const char *, const QStringList &)
{
  // Create an instance of our Part
  KatePart* obj = new KatePart(parent, name );

  return obj;
}


KInstance* KatePartFactory::instance()
{
  if ( !s_instance )
  {
    s_about = new KAboutData("kate_part", I18N_NOOP("KateEditorPart"), "0.1");
    s_about->addAuthor("Matthias H�lzer-Kl�pfel", 0, "mhk@caldera.de");
    s_instance = new KInstance(s_about);
  }
  return s_instance;
}


extern "C"
{
  void* init_libkateeditorpart()
  {
    return new KatePartFactory;
  }
};


#include "kate_factory.moc"
