#ifndef __DOCUMENT_IMPL_H__
#define __DOCUMENT_IMPL_H__


#include <keditor/editor.h>


class KateDocument;
class KateView;


class DocumentImpl : public KEditor::Document
{
  Q_OBJECT

public:

  DocumentImpl(KEditor::Editor *parent, QWidget *parentWidget=0);
  virtual ~DocumentImpl();
  
  virtual bool saveFile();


protected:

  virtual bool openFile();


private:

  KateDocument *m_document;
  KateView *m_view;

};


#endif
