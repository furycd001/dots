/****************************************************************************
** $Id: qt/examples/validator/motor.h   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef MOTOR_H
#define MOTOR_H

#include <qvalidator.h>
#include <qspinbox.h>


class MotorValidator: public QValidator
{
    Q_OBJECT
public:
    MotorValidator( QSpinBox * parent, const char * name );
    ~MotorValidator();

    void setRange( int bottom, int top, int step );

    int bottom() { return b; }
    int top() { return t; }
    int step() { return s; }

    QValidator::State validate( QString &, int & ) const;

private:
    int b, t, s;
};


#endif
