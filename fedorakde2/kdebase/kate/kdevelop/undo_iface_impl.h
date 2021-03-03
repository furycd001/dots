#ifndef __UNDO_IFACE_IMPL_H__
#define __UNDO_IFACE_IMPL_H__


#include "keditor/undo_iface.h"


class KateView;


class UndoIfaceImpl : public KEditor::UndoDocumentIface
{
  Q_OBJECT

public:

  UndoIfaceImpl(KateView *edit, KEditor::Document *parent, KEditor::Editor *editor);

  virtual bool undo();
  virtual bool redo();

  virtual bool undoAvailable();
  virtual bool redoAvailable();


private slots:

  void slotUndoStatus();


private:

  KateView *m_edit;

  bool _undo, _redo;

};


#endif
