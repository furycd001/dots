/***************************************************************************
                          schemaeditor.cpp  -  description
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

#define TABLE_COLORS 20


#include "schemaeditor.h"

#include <dcopclient.h>

#include <qlabel.h>
#include <kcolorbutton.h>
#include <qcombobox.h>
#include <kdebug.h>
#include <qcheckbox.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <qfile.h>
#include <qlistbox.h>
#include <qinputdialog.h>

//#include <errno.h>

#include <qtextstream.h>
#include <qslider.h>
#include <qlineedit.h>
#include <klocale.h>
#include <kfiledialog.h>
#include <kurl.h>
#include <qtoolbutton.h>
#include <kmessagebox.h>
#include <qlist.h>
#include <ksharedpixmap.h>
#include <kpixmap.h>
#include <kimageeffect.h>
#include <qimage.h>

SchemaEditor::SchemaEditor(QWidget * parent, const char *name)
:SchemaDialog(parent, name)
{
    schMod= false;
    loaded = false;
    oldSlot = 0;
    oldSchema = -1;
    color.resize(20);
    type.resize(20);
    bold.resize(20);
    transparent.resize(20);
    defaultSchema = "";
    spix = new KSharedPixmap;

    filename.setAutoDelete(true);

    connect(spix, SIGNAL(done(bool)), SLOT(previewLoaded(bool)));

    DCOPClient *client = kapp->dcopClient();
    if (!client->isAttached())
	client->attach();
    QByteArray data;

    QDataStream args(data, IO_WriteOnly);
    args << 1;
    client->send("kdesktop", "KBackgroundIface", "setExport(int)", data);




    transparencyCheck->setChecked(true);
    transparencyCheck->setChecked(false);


    loadAllSchema();

    connect(imageBrowse, SIGNAL(clicked()), this, SLOT(imageSelect()));
    connect(saveButton, SIGNAL(clicked()), this, SLOT(saveCurrent()));
    connect(removeButton, SIGNAL(clicked()), this, SLOT(removeCurrent()));
    connect(colorCombo, SIGNAL(activated(int)), this, SLOT(slotColorChanged(int)));
    connect(typeCombo, SIGNAL(activated(int)), this, SLOT(slotTypeChanged(int)));
    connect(schemaList, SIGNAL(highlighted(int)), this, SLOT(readSchema(int)));
    connect(shadeColor, SIGNAL(changed(const QColor&)), this, SLOT(updatePreview()));
    connect(shadeSlide, SIGNAL(valueChanged(int)), this, SLOT(updatePreview()));
    connect(transparencyCheck, SIGNAL(toggled(bool)), this, SLOT(updatePreview()));
    connect(backgndLine, SIGNAL(returnPressed()), this, SLOT(updatePreview()));

    connect(shadeColor, SIGNAL(changed(const QColor&)), this, SLOT(schemaModified()));
    connect(shadeSlide, SIGNAL(valueChanged(int)), this, SLOT(schemaModified()));
    connect(transparencyCheck, SIGNAL(toggled(bool)), this, SLOT(schemaModified()));
    connect(backgndLine, SIGNAL(returnPressed()), this, SLOT(schemaModified()));
    connect(transparentCheck, SIGNAL(toggled(bool)), this, SLOT(schemaModified()));
    connect(boldCheck, SIGNAL(toggled(bool)), this, SLOT(schemaModified()));
    connect(colorButton, SIGNAL(changed(const QColor&)), this, SLOT(schemaModified()));

}


QString SchemaEditor::schema()
{
    if (defaultSchemaCB->isChecked())
     if(schemaList->currentItem()>=0)
	return *filename.at(schemaList->currentItem());

    return defaultSchema;

}


void SchemaEditor::setSchema(QString sch)
{
    defaultSchema = sch;

    QString *it;
    int i = 0, sc = -1;
    for (it = filename.first(); it != 0; it = filename.next()) {
	if (sch == (*it))
	    sc = i;
	i++;
    }

    oldSchema = sc;
    if (sc == -1)
	sc = 0;
    schemaList->setCurrentItem(sc);
//    readSchema(sc);
}

SchemaEditor::~SchemaEditor()
{
}



void SchemaEditor::updatePreview()
{

    if (transparencyCheck->isChecked()) {
	if (loaded) {
	    float rx = (100.0 - shadeSlide->value()) / 100;
	    QImage ima(pix.convertToImage());
	    ima = KImageEffect::fade(ima, rx, shadeColor->color());
	    QPixmap pm;
	    pm.convertFromImage(ima);
	    previewPixmap->setPixmap(pm);
	    previewPixmap->setScaledContents(true);
	}
	 else  //try to reload
	{
           if(!spix->loadFromShared(QString("DESKTOP1"))) 
              kdDebug(0) << "cannot load" << endl;
	
	}
      } else {
	QPixmap pm;
	pm.load(backgndLine->text());
	previewPixmap->setPixmap(pm);
	previewPixmap->setScaledContents(true);
    }

}

void SchemaEditor::previewLoaded(bool l)
{
    if (l) {
	QWMatrix mat;
	pix =
	    spix->xForm(mat.
			scale(180.0 / spix->QPixmap::width(),
			      100.0 / spix->QPixmap::height()));
	kdDebug(0) << "Loaded" << endl;
	loaded = true;
	if (transparencyCheck->isChecked()) {
	    updatePreview();
	}

    } else
	kdDebug(0) << "error loading" << endl;

}



void SchemaEditor::loadAllSchema()
{
    QStringList list = KGlobal::dirs()->findAllResources("data", "konsole/*.schema");
    QStringList::ConstIterator it;
    schemaList->clear();

//    schemaList->setCurrentItem(0);
    filename.clear();
    int i = 0;
    for (it = list.begin(); it != list.end(); ++it) {

	QString name = (*it);
	
	filename.append(new QString(name));

	QString title = readSchemaTitle(name);

	if (title.isNull() || title.isEmpty())
	    schemaList->insertItem(i18n("untitled"),i);
	else
	 schemaList->insertItem(title,i);
	
	i++;
    }

}

void SchemaEditor::imageSelect()
{
    QString start;
    start = backgndLine->text();
    if (start.isEmpty())
	start = KGlobal::dirs()->kde_default("wallpaper");


    KURL url = KFileDialog::getImageOpenURL(start, 0, i18n("Select a background image"));
// KURL url=KFileDialog::getOpenURL(start,"",0,i18n("Select a background image"));

    backgndLine->setText(url.path());
    updatePreview();
}

void SchemaEditor::slotTypeChanged(int slot)
{
    schemaModified();

    bool active = false;
    if (slot == 0)
	active = true;
    colorButton->setEnabled(active);
    boldCheck->setEnabled(active);
    transparentCheck->setEnabled(active);
}


void SchemaEditor::slotColorChanged(int slot)
{
    kdDebug(0) << slot << endl;
    color[oldSlot] = colorButton->color();
    type[oldSlot] = typeCombo->currentItem();
    bold[oldSlot] = boldCheck->isChecked();
    transparent[oldSlot] = transparentCheck->isChecked();

    transparentCheck->setChecked(transparent[slot]);
    boldCheck->setChecked(bold[slot]);
    typeCombo->setCurrentItem(type[slot]);
    colorButton->setColor(color[slot]);
    oldSlot = slot;
}

void SchemaEditor::removeCurrent()
{
    if(schemaList->currentItem()==-1)
        return;
    QString base = *filename.at(schemaList->currentItem());
    if(base==schema())
     setSchema("");
    if (!QFile::remove(base))
	KMessageBox::error(this,
			   i18n("Cannot remove the schema.\nMaybe it is a system schema\n"),
			   i18n("Error removing schema"));

    loadAllSchema();

    setSchema(defaultSchema);

}

void SchemaEditor::saveCurrent()
{

    //This is to update the color table
    slotColorChanged(0);

    QString base;
    if (schemaList->currentText() == titleLine->text())
	base = *filename.at(schemaList->currentItem());
    else
	base = titleLine->text().stripWhiteSpace().simplifyWhiteSpace() + ".schema";
    QString name = QInputDialog::getText(i18n("Save schema as..."),
					 i18n("Filename:"), base);

    if (name.isNull() || name.isEmpty())
	return;

    QString fullpath;
    if (name[0] == '/')
	fullpath = name;
    else
	fullpath = KGlobal::dirs()->saveLocation("data", "konsole/") + name;

    kdDebug(0) << fullpath << endl;



    QFile f(fullpath);
    if (f.open(IO_WriteOnly)) {
	QTextStream t(&f);

	t << "# schema for konsole autogenerated with the schema editor" << endl;
	t << endl;
	t << "title " << titleLine->text() << endl;
	t << endl;
	if (transparencyCheck->isChecked()) {
	    QColor c = shadeColor->color();
	    QString tra;
	    tra.sprintf("transparency %1.2f %3d %3d %3d",
			1.0 * (100 - shadeSlide->value()) / 100, c.red(), c.green(), c.blue());
	    t << tra << endl;
	}

	if (!backgndLine->text().isEmpty()) {
	    QString smode;
	    int mode;
	    mode = modeCombo->currentItem();
	    if (mode == 0)
		smode = "tile";
	    if (mode == 1)
		smode = "center";
	    if (mode == 2)
		smode = "full";

	    QString image;
	    image.sprintf("image %s %s",
			  (const char *) smode.latin1(),
			  (const char *) backgndLine->text().local8Bit());
	    t << image << endl;
	}
	t << endl;
	t << "# foreground colors" << endl;
	t << endl;
	t << "# note that the default background color is flagged" << endl;
	t << "# to become transparent when an image is present." << endl;
	t << endl;
	t << "#   slot    transparent bold" << endl;
	t << "#      | red grn blu  | |" << endl;
	t << "#      V V--color--V  V V" << endl;

	for (int i = 0; i < 20; i++) {
	    QString scol;
	    if (type[i] == 0)
		scol.sprintf("color %2d %3d %3d %3d %2d %1d # %s", i,
			     color[i].red(), color[i].green(), color[i].blue(),
			     transparent[i], bold[i],
			     (const char *) colorCombo->text(i).local8Bit());
	    else if (type[i] == 1)
		scol.sprintf("sysfg %2d             %2d %1d # %s", i,
			     transparent[i], bold[i],
			     (const char *) colorCombo->text(i).local8Bit());
	    else
		scol.sprintf("sysbg %2d             %2d %1d # %s", i,
			     transparent[i], bold[i],
			     (const char *) colorCombo->text(i).local8Bit());

	    t << scol << endl;
	}


	f.close();
    } else
	KMessageBox::error(this, i18n("Cannot save the schema.\nMaybe permission denied\n"),
			   i18n("Error saving schema"));


    loadAllSchema();
    setSchema(defaultSchema);

}

void SchemaEditor::schemaModified()
{
    schMod=true;
}

QString SchemaEditor::readSchemaTitle(const QString & file)
{
    /*
       Code taken from konsole/src/schema.C

     */


    QString fPath = locate("data", "konsole/" + file);

    if (fPath.isNull())
	fPath = locate("data", file);

    if (fPath.isNull())
	return 0;

    FILE *sysin = fopen(QFile::encodeName(fPath), "r");
    if (!sysin) 
	return 0;
    
    
    char line[100];
    while (fscanf(sysin, "%80[^\n]\n", line) > 0)
	if (strlen(line) > 5)
	    if (!strncmp(line, "title", 5)) {
		fclose(sysin);
		return line + 6;
	    }

    return 0;
}

void SchemaEditor::readSchema(int num)
{
    /*
       Code taken from konsole/src/schema.C

     */
     
    if (oldSchema != -1) {
    
    
	if (defaultSchemaCB->isChecked()) {

	    defaultSchema = *filename.at(oldSchema);

	}
    
    if(schMod) {
	    disconnect(schemaList, SIGNAL(highlighted(int)), this, SLOT(readSchema(int)));
	    
	    schemaList->setCurrentItem(oldSchema);
	    if(KMessageBox::questionYesNo(this, i18n("The schema has been modified.\n"
				"Do you want to save the changes ?"),
		   i18n("Schema modified"))==KMessageBox::Yes)
	
		    saveCurrent();   
		   
	    schemaList->setCurrentItem(num);
	    connect(schemaList, SIGNAL(highlighted(int)), this, SLOT(readSchema(int)));
	    schMod=false;
    
    }
     

    
    }

    QString fPath = locate("data", "konsole/" + *filename.at(num));

    if (fPath.isNull())
	fPath = locate("data", *filename.at(num));

    if (fPath.isNull()) {
	KMessageBox::error(this, i18n("Cannot find the schema."),
			   i18n("Error loading schema"));


	return;
    }
    defaultSchemaCB->setChecked(fPath == defaultSchema);

    FILE *sysin = fopen(QFile::encodeName(fPath), "r");
    if (!sysin) {
	KMessageBox::error(this, i18n("Cannot load the schema."),
			   i18n("Error loading schema"));
	loadAllSchema();
	return;
    }

    char line[100];


    titleLine->setText(i18n("untitled"));
    transparencyCheck->setChecked(false);
    backgndLine->setText("");

    while (fscanf(sysin, "%80[^\n]\n", line) > 0) {
	if (strlen(line) > 5) {

	    if (!strncmp(line, "title", 5)) {
		titleLine->setText(line + 6);
	    }



	    if (!strncmp(line, "image", 5)) {
		char rend[100], path[100];
		int attr = 1;
		if (sscanf(line, "image %s %s", rend, path) != 2)
		    continue;
		if (!strcmp(rend, "tile"))
		    attr = 2;
		else if (!strcmp(rend, "center"))
		    attr = 3;
		else if (!strcmp(rend, "full"))
		    attr = 4;
		else
		    continue;

		backgndLine->setText(locate("wallpaper", path));
		modeCombo->setCurrentItem(attr - 2);

	    }


	    if (!strncmp(line, "transparency", 12)) {
		float rx;
		int rr, rg, rb;
		// Transparency needs 4 parameters: fade strength and the 3
		// components of the fade color.
		if (sscanf(line, "transparency %g %d %d %d", &rx, &rr, &rg, &rb) != 4)
		    continue;

		transparencyCheck->setChecked(true);
		shadeSlide->setValue(100 - rx * 100);
		shadeColor->setColor(QColor(rr, rg, rb));

	    }

	    if (!strncmp(line, "color", 5)) {
		int fi, cr, cg, cb, tr, bo;
		if (sscanf(line, "color %d %d %d %d %d %d", &fi, &cr, &cg, &cb, &tr, &bo) != 6)
		    continue;
		if (!(0 <= fi && fi <= TABLE_COLORS))
		    continue;
		if (!(0 <= cr && cr <= 255))
		    continue;
		if (!(0 <= cg && cg <= 255))
		    continue;
		if (!(0 <= cb && cb <= 255))
		    continue;
		if (!(0 <= tr && tr <= 1))
		    continue;
		if (!(0 <= bo && bo <= 1))
		    continue;
		color[fi] = QColor(cr, cg, cb);
		transparent[fi] = tr;
		bold[fi] = bo;
		type[fi] = 0;

	    }
	    if (!strncmp(line, "sysfg", 5)) {
		int fi, tr, bo;
		if (sscanf(line, "sysfg %d %d %d", &fi, &tr, &bo) != 3)
		    continue;
		if (!(0 <= fi && fi <= TABLE_COLORS))
		    continue;
		if (!(0 <= tr && tr <= 1))
		    continue;
		if (!(0 <= bo && bo <= 1))
		    continue;
		color[fi] = kapp->palette().normal().text();
		transparent[fi] = tr;
		bold[fi] = bo;
		type[fi] = 1;
	    }
	    if (!strncmp(line, "sysbg", 5)) {
		int fi, tr, bo;
		if (sscanf(line, "sysbg %d %d %d", &fi, &tr, &bo) != 3)
		    continue;
		if (!(0 <= fi && fi <= TABLE_COLORS))
		    continue;
		if (!(0 <= tr && tr <= 1))
		    continue;
		if (!(0 <= bo && bo <= 1))
		    continue;
		color[fi] = kapp->palette().normal().base();
		transparent[fi] = tr;
		bold[fi] = bo;
		type[fi] = 2;
	    }
	}
    }
    fclose(sysin);
    int ii = colorCombo->currentItem();
    transparentCheck->setChecked(transparent[ii]);
    boldCheck->setChecked(bold[ii]);
    typeCombo->setCurrentItem(type[ii]);
    colorButton->setColor(color[ii]);

    boldCheck->setDisabled(type[ii]);
    transparentCheck->setDisabled(type[ii]);
    colorButton->setDisabled(type[ii]);

    oldSchema = num;
    updatePreview();
    schMod=false;
    return;
}
