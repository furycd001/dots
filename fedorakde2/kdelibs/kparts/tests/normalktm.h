
#ifndef __normalktm_h__
#define __normalktm_h__

#include <kparts/part.h>
#include <ktmainwindow.h>

class KAction;
class QWidget;

class Shell : public KTMainWindow
{
  Q_OBJECT
public:
  Shell();
  virtual ~Shell();

protected slots:
  void slotFileOpen();
  void slotFileOpenRemote();
  void slotFileEdit();
  void slotFileCloseEditor();

protected:
  void embedEditor();

private:

  KAction * m_paEditFile;
  KAction * m_paCloseEditor;

  KParts::ReadOnlyPart *m_part1;
  KParts::Part *m_part2;
  KParts::ReadWritePart *m_editorpart;
  QWidget *m_splitter;
};

#endif
