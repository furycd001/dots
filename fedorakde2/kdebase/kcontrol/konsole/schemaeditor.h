/***************************************************************************
                          schemaeditor.h  -  description
                             -------------------
    begin                : mar apr 17 16:44:59 CEST 2001
    copyright            : (C) 2001 by Andrea Rizzi
    email                : rizzi@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SCHEMAEDITOR_H
#define SCHEMAEDITOR_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qlist.h>
#include <kapp.h>
#include <qwidget.h>
class QPixmap;
class KSharedPixmap;

#include "schemadialog.h"

/** SchemaEditor is the base class of the porject */
class SchemaEditor : public SchemaDialog
{
  Q_OBJECT 
  public:
    /** construtor */
    SchemaEditor(QWidget* parent=0, const char *name=0);
    /** destructor */
    ~SchemaEditor();

    QString schema();
    void setSchema(QString);

  public slots:
  	void slotColorChanged(int);
  	void imageSelect();  	
	void slotTypeChanged(int);
	void readSchema(int);
	void saveCurrent();
	void removeCurrent();
	void previewLoaded(bool l);		
  private slots:
        void schemaModified();
  	void loadAllSchema();
	void updatePreview();
  private:
	bool schMod;
  	QArray<QColor> color;
	QArray<int> type; // 0= custom, 1= sysfg, 2=sysbg
	QArray<bool> transparent;
	QArray<bool> bold;
	QPixmap pix;
	KSharedPixmap *spix;
	QList<QString> filename;
	QString defaultSchema;	
	bool loaded;
	int oldSchema;
	int oldSlot;
	QString readSchemaTitle(const QString& filename);

};

#endif
