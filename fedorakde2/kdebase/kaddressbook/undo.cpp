/* This file is part of KDE PIM
    Copyright (C) 1999 Don Sanders <sanders@kde.org>

    License: BSD
*/

#include "undo.h"

UndoStack* UndoStack::instance_ = 0;

UndoStack::UndoStack()
{
  // setAutoDelete( true );
}

UndoStack* UndoStack::instance()
{
  if (!instance_)
    instance_ = new UndoStack();
  return instance_;
}

void UndoStack::undo()
{
  if (isEmpty())
    return;
  Command *command = pop();
  command->undo();
  RedoStack::instance()->push( command );
}

RedoStack* RedoStack::instance_ = 0;

RedoStack::RedoStack()
{
  setAutoDelete( true );
}

RedoStack* RedoStack::instance()
{
  if (!instance_)
    instance_ = new RedoStack();
  return instance_;
}

void RedoStack::redo()
{
  Command *command;
  if (isEmpty())
    return;
  command = pop();
  command->redo();
  UndoStack::instance()->push( command );
}
