#ifndef __DEBUG_IFACE_IMPL_H__
#define __DEBUG_IFACE_IMPL_H__


#include "keditor/debug_iface.h"


class KateView;


class DebugIfaceImpl : public KEditor::DebugDocumentIface
{
  Q_OBJECT

public:

  DebugIfaceImpl(KateView *edit, KEditor::Document *parent, KEditor::Editor *editor);

  virtual bool markExecutionPoint(int line);   
  virtual bool setBreakPoint(int line, bool enabled, bool pending);   
  virtual bool unsetBreakPoint(int line);
  

private slots:

  void slotToggledBreakpoint(int line);
  void slotEnabledBreakpoint(int lint);


private:

  KateView *m_edit;

};


#endif
