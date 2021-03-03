#ifndef __STATUS_IFACE_IMPL_H__
#define __STATUS_IFACE_IMPL_H__


#include "keditor/status_iface.h"


class KateView;


class StatusIfaceImpl : public KEditor::StatusDocumentIface
{
  Q_OBJECT

public:

  StatusIfaceImpl(KateView *edit, KEditor::Document *parent, KEditor::Editor *editor);

  virtual bool modified();
  virtual QString status();


private slots:

  void slotStatusChanged();
  void slotMessage(const QString &text);


private:

  KateView *m_edit;

};


#endif
