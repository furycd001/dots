#ifndef __KWRITE_PART_H__
#define __KWRITE_PART_H__


#include <qlist.h>


#include <kparts/part.h>


class KURL;


#include "keditor/editor.h"
#include "document_impl.h"


class KatePart : public KEditor::Editor
{
  Q_OBJECT

public:

  KatePart(QObject *parent, const char *name);
  virtual ~KatePart();

  virtual KEditor::Document *document(const KURL &url);
  virtual KEditor::Document *createDocument(QWidget *parentWidget=0, const KURL &url="");


private slots:

  void documentDestroyed();


private:

  QList<DocumentImpl> _documents;
  
};


#endif
