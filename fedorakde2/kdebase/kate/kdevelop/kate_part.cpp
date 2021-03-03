#include <kinstance.h>
#include <kparts/partmanager.h>
#include <kdebug.h>


#include "kate/view.h"


#include "kate_part.h"
#include "kate_factory.h"


KatePart::KatePart(QObject *parent, const char *name )
  : KEditor::Editor(parent, name)
{
  setInstance(KatePartFactory::instance());
}


KatePart::~KatePart()
{
}


KEditor::Document *KatePart::document(const KURL &url)
{
  QListIterator<DocumentImpl> it(_documents);
  for ( ; it.current(); ++it)
    if (it.current()->url() == url)
      return it.current();

  return 0;
}


KEditor::Document *KatePart::createDocument(QWidget *parentWidget, const KURL &url)
{
  DocumentImpl *impl = new DocumentImpl(this, parentWidget);
  if (!url.isEmpty())
    impl->openURL(url);

  _documents.append(impl);
  connect(impl, SIGNAL(destroyed()), this, SLOT(documentDestroyed()));
  
  return impl;
}


void KatePart::documentDestroyed()
{
  _documents.remove(static_cast<const DocumentImpl*>(sender()));
}


#include "kate_part.moc"
