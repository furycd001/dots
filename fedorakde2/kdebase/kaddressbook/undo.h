/* This file is part of KDE PIM
   Copyright (C) 1999 Don Sanders <sanders@kde.org>

   License: BSD
*/

#ifndef UNDO_H 
#define UNDO_H 

#include <qstack.h>
#include <qstring.h>

class Command
{
public:
  virtual ~Command() {};
  virtual QString name() { return ""; };
  virtual void redo() {}; // egcs requires these methods to have 
  virtual void undo() {}; // implementations (Seems like a bug)
};

class UndoStack : public QStack< Command >
{

public:
  static UndoStack *instance();
  void undo();

  //signals:
  //  void changed();

protected:
  UndoStack();
  static UndoStack* instance_;
};

class RedoStack : public QStack< Command >
{

public:
  static RedoStack *instance();
  void redo();

  //signals:
  //  void changed();

protected:
  RedoStack();
  static RedoStack* instance_;
};

#endif
