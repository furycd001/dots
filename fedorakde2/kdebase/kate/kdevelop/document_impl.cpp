#include <qmultilineedit.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qtabwidget.h>


#include <kdebug.h>


#include "kate_factory.h"
#include "document_impl.h"
#include "clipboard_iface_impl.h"
#include "undo_iface_impl.h"
#include "cursor_iface_impl.h"
#include "edit_iface_impl.h"
#include "status_iface_impl.h"
#include "debug_iface_impl.h"


#include "document/katedocument.h"
#include "view/kateview.h"


DocumentImpl::DocumentImpl(KEditor::Editor *parent, QWidget *parentWidget)
  : KEditor::Document(parent)
{
  setInstance( KatePartFactory::instance() );
  
  m_document = new KateDocument();
  m_view = static_cast<KateView*>(m_document->createView(parentWidget, ""));
  setWidget(m_view);

  // create interfaces
  new ClipboardIfaceImpl(m_view, this, parent);
  new CursorIfaceImpl(m_view, this, parent);
  new UndoIfaceImpl(m_view, this, parent);
  new EditIfaceImpl(m_view, this, parent);
  new StatusIfaceImpl(m_view, this, parent);
  //new DebugIfaceImpl(m_view, this, parent);

//  setXMLFile("kate_part.rc", false);
  
  insertChildClient(m_view);

  m_view->setFocus();
}


DocumentImpl::~DocumentImpl()
{
kdDebug() << "DocumentImpl " << this << " desctructed!!!" << endl;
}


bool DocumentImpl::openFile()
{
  bool result = m_document->openURL(KURL(m_file));

  if (result)
    resetModifiedTime();

  emit KEditor::Document::loaded(this);
  
  return result;
}


bool DocumentImpl::saveFile()
{
  if (!shouldBeSaved())
    return false;

  QFile f(m_file);
  if (!f.open(IO_WriteOnly))
    return false;

  QTextStream ts(&f);
  ts << m_document->text();

  f.close();

  resetModifiedTime();

  emit KEditor::Document::saved(this);
  
  return true;
}


#include "document_impl.moc"
