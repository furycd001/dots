#include "rc2ui.h"
#include <qdir.h>

/// some little helpers ///

const QString blockStart1 = "/////////////////////////////////////////////////////////////////////////////";
const QString blockStart2 = "//";

void RC2UI::wi()
{
    for ( int i = 0; i < indentation; i++ )
    *out << "    ";
}

void RC2UI::indent()
{
    indentation++;
}

void RC2UI::undent()
{
    indentation--;
}

QString RC2UI::stripQM( const QString& string )
{
    return string.mid( 1, string.length()-2 );
}

QStringList RC2UI::splitStyles( const QString& styles, char sep )
{
    QString s = styles;
    QString style;
    QStringList l;
    while ( s.find( sep ) > -1 ) {
	style = s.left( s.find( sep ) );
	l << style.stripWhiteSpace();
	s = s.right( s.length() - style.length() -1 );
    }
    if ( !s.isEmpty() )
	l << s.stripWhiteSpace();
    return l;
}

QString RC2UI::parseNext( QString& arg, char sep )
{
    QString next = arg.left( arg.find(sep) );
    arg = arg.right( arg.length() - next.length() - 1 );
    return next;
}

void RC2UI::writeClass( const QString& name )
{
    wi(); *out << "<class>" << name << "</class>" << endl;
}

void RC2UI::writeCString( const QString& name, const QString& value )
{
    wi(); *out << "<property>" << endl; indent();
    wi(); *out << "<name>" << name << "</name>" << endl;
    wi(); *out << "<cstring>" << value << "</cstring>" << endl; undent();
    wi(); *out << "</property>" << endl;
}

void RC2UI::writeString( const QString& name, const QString& value )
{
    wi(); *out << "<property>" << endl; indent();
    wi(); *out << "<name>" << name << "</name>" << endl;
    wi(); *out << "<string>" << value << "</string>" << endl; undent();
    wi(); *out << "</property>" << endl;
}

void RC2UI::writeRect( const QString& name, int x, int y, int w, int h )
{
    wi(); *out << "<property>" << endl; indent();
    wi(); *out << "<name>" << name << "</name>" << endl;
    wi(); *out << "<rect>" << endl; indent();
    wi(); *out << "<x>" << int(double(x)*1.5) << "</x>" << endl;
    wi(); *out << "<y>" << int(double(y)*1.65) << "</y>" << endl;
    wi(); *out << "<width>" << int(double(w)*1.5) << "</width>" << endl;
    wi(); *out << "<height>" << int(double(h)*1.65) << "</height>" << endl; undent();
    wi(); *out << "</rect>" << endl; undent();
    wi(); *out << "</property>" << endl;
}

void RC2UI::writeFont( const QString& family, int pointsize )
{
    wi(); *out << "<property>" << endl; indent();
    wi(); *out << "<name>font</name>" << endl;
    wi(); *out << "<font>" << endl; indent();
    wi(); *out << "<family>" << family << "</family>" << endl;
    wi(); *out << "<pointsize>" << pointsize << "</pointsize>" << endl; undent();
    wi(); *out << "</font>" << endl; undent();
    wi(); *out << "</property>" << endl;
}

void RC2UI::writeBool( const QString& name, bool value )
{
    wi(); *out << "<property>" << endl; indent();
    wi(); *out << "<name>" << name << "</name>" << endl;
    wi(); *out << "<bool>" << (value ? "true" : "false") << "</bool>" << endl; undent();
    wi(); *out << "</property>" << endl;
}

void RC2UI::writeNumber( const QString& name, int value )
{
    wi(); *out << "<property>" << endl; indent();
    wi(); *out << "<name>" << name << "</name>" << endl;
    wi(); *out << "<number>" << value << "</number>" << endl; undent();
    wi(); *out << "</property>" << endl;
}

void RC2UI::writeEnum( const QString& name, const QString& value )
{
    wi(); *out << "<property>" << endl; indent();
    wi(); *out << "<name>" << name << "</name>" << endl;
    wi(); *out << "<enum>" << value << "</enum>" << endl; undent();
    wi(); *out << "</property>" << endl;
}

void RC2UI::writeSet( const QString& name, const QString& value )
{
    wi(); *out << "<property>" << endl; indent();
    wi(); *out << "<name>" << name << "</name>" << endl;
    wi(); *out << "<set>" << value << "</set>" << endl; undent();
    wi(); *out << "</property>" << endl;
}

void RC2UI::writeStyles( const QStringList styles, bool isFrame )
{
    if ( isFrame ) {
	bool defineFrame = FALSE;
	QString shadow = "NoFrame";
	QString shape = "StyledPanel";
	int width = 2;
	if ( styles.contains( "WS_EX_STATICEDGE" ) ) {
	    shadow = "Plain";
	    width = 1;
	    defineFrame = TRUE;
	}
	if ( styles.contains( "WS_EX_CLIENTEDGE" ) ) {
	    shadow = "Sunken";
	    defineFrame = TRUE;
	}
	if ( styles.contains( "WS_EX_DLGMODALFRAME" ) ) {
	    shadow = "Raised";
	    defineFrame = TRUE;
	}
	if ( !styles.contains( "WS_BORDER" ) ) {
	    shape = "NoFrame";
	    defineFrame = TRUE;
	}

	if ( defineFrame ) {
	    writeEnum( "frameShape", "StyledPanel" );
	    writeEnum( "frameShadow", shadow );
	    writeNumber( "lineWidth", width );
	}
    }

    if ( styles.contains("WS_DISABLED") )
	writeBool("enabled", FALSE );
    if ( styles.contains("WS_EX_ACCEPTFILES") )
	writeBool("acceptDrops", TRUE );
    if ( styles.contains("WS_EX_TRANSPARENT") )
	writeBool("autoMask", TRUE );
    if ( !styles.contains("WS_TABSTOP") )
	writeEnum("focusPolicy", "NoFocus");
}

/*!
  Constructs a RC2UI object
*/

RC2UI::RC2UI( QTextStream* input )
{
    writeToFile = TRUE;
    in = input;
    indentation = 0;
    out = 0;
}

/*!
  Destructs the RC2UI object
*/

RC2UI::~RC2UI()
{
}

/*!
  Parses the input stream and writes the output to files.
*/

bool RC2UI::parse()
{
    while ( !in->eof() ) {
	while ( line != blockStart1 && !in->eof() )
	    line = in->readLine();
	if ( in->eof() )
	    return FALSE;
 	while ( line != blockStart2 && !in->eof() )
	    line = in->readLine();
	if ( in->eof() )
	    return FALSE;

	line = in->readLine();

	if ( line.left(3) == "// " && !in->eof() ) {
	    QString type = line.right( line.length() - 3 );
	    if ( in->readLine() == "//" && in->readLine().isEmpty() && !in->eof() ) {
		if ( type == "Dialog" ) {
		    if ( !makeDialog() )
			return FALSE;
		} 
/*
		  else if ( type == "Bitmap" ) {
		    if ( !makeBitmap() )
			return FALSE;
		} else if ( type == "String Table" ) {
		    if ( !makeStringTable() )
			return FALSE;
		} else if ( type == "Accelerator" ) {
		    if ( !makeAccelerator() )
			return FALSE;
		} else if ( type == "Cursor" ) {
		    if ( !makeCursor() )
			return FALSE;
		} else if ( type == "HTML" ) {
		    if ( !makeHTML() )
			return FALSE;
		} else if ( type == "Icon" ) {
		    if ( !makeIcon() )
			return FALSE;
		} else if ( type == "Version" ) {
		    if ( !makeVersion() )
			return FALSE;
		}
*/
	    }
	} else
	    return FALSE;
    }
    return TRUE;
}

/*!
  Parses the input stream and writes the output in \a get.
*/

bool RC2UI::parse( QStringList& get )
{
    writeToFile = FALSE;
    bool result = parse();
    get = target;
    return result;
}

/*!
  Retrieves a unique name starting with \a start
*/
QString RC2UI::useName( const QString& start )
{
    QString name = start;
    int id = 1;

    while ( usedNames.contains( name ) ) {
	name = start + QString( "%1" ).arg( id );
	id++;
    }

    usedNames.append(name);

    return name;
}


/*!
  Builds a number of UI dialog out of the current input stream
*/

bool RC2UI::makeDialog()
{
    line = in->readLine();
    do {
        QFile fileOut;
	QString buffer;
	int count;
	QCString className;
	uint x, y, w, h;
	uint endDesc;
	bool space = FALSE;
	for ( endDesc = 0; endDesc < line.length() ; endDesc++ ) {
	    char c = (QChar)line.at(endDesc);
	    if ( space && (c >= '0') && (c <= '9') )
		break;
	    space = c==' ';
	}
	
	QString desc = line.left(endDesc-1);
	line = line.right( line.length() - endDesc );

	className = parseNext( desc, ' ' );

 	count = sscanf( line, "%d, %d, %d, %d", &x, &y, &w, &h );
	
	if ( !count && count == EOF )
	    return FALSE;

	char property[256];
	QStringList styles;
	QStringList extendedStyles;
	QString caption = "";
	QString baseClass = "";
	QString widgetType;
	QString widgetName;
	QString arguments;
	int pointsize;
	QString fontname;
	do {
	    if ( in->eof() )
		return TRUE;
	    line = "";
	    do {
		line += in->readLine();
	    } while ( line[(int)line.length()-1] == '|' || 
		      line[(int)line.length()-1] == ',' );
	    count = sscanf( line, "%s", property );
	    line = line.right( line.length() - line.find(" ") -1 );
	    if ( QString(property) == "STYLE" ) {
		styles = splitStyles(line);
		if ( styles.contains( "WS_CAPTION" ) )
		    baseClass = "QDialog";
		else
		    baseClass = "QWidget";
	    } else if ( QString(property) == "CAPTION" ) {
		caption = stripQM( line );
	    } else if ( QString(property) == "FONT" ) {
		QString pt = line.left( line.find(",") );
		pointsize = pt.toInt();
		fontname = stripQM(line.right( line.length() - line.find(",") - 2 ));
	    }
	} while ( line != "BEGIN" );

	if ( writeToFile ) {
	    
	    QString outputFile = QString(className) + ".ui";
	    fileOut.setName( outputFile );
	    if (!fileOut.open( IO_WriteOnly ) )
		qFatal( "rc2ui: Could not open output file '%s'", outputFile.latin1() );
	    out = new QTextStream( &fileOut );

	} else {
	    out = new QTextStream( &buffer, IO_WriteOnly );
	}

	*out << "<!DOCTYPE UI><UI>" << endl;
	writeClass( className );
	wi(); *out << "<widget>"<< endl; indent();
	writeClass( baseClass );
	writeCString( "name", className );
	writeRect( "geometry", x, y, w, h );
	writeString( "caption", caption );
	writeFont( fontname, pointsize );

	do {
	    line = in->readLine().stripWhiteSpace();
	    if ( line == "END" )
		continue;

	    widgetType = parseNext(line, ' ');
	    arguments = line.stripWhiteSpace();
	    while ( arguments[(int)arguments.length()-1] == ',' || 
		    arguments[(int)arguments.length()-1] == '|'  )
		arguments += " "+in->readLine().stripWhiteSpace();

	    wi(); *out << "<widget>" << endl; indent();

	    WidgetType ID = IDUnknown;
	    QString controlType;
	    QString widgetID;
	    QString widgetText;
	    bool hasText = FALSE;
	    bool isControl = FALSE;
	    bool isFrame = FALSE;

	    if ( widgetType == "PUSHBUTTON" ) {
		ID = IDPushButton;
		hasText = TRUE;
	    } else if ( widgetType == "DEFPUSHBUTTON" ) {
		ID = IDPushButton;
		hasText = TRUE;
	    } else if ( widgetType == "LTEXT" ) {
		ID = IDLabel;
		hasText = TRUE;
	    } else if ( widgetType == "CTEXT" ) {
		ID = IDLabel;
		hasText = TRUE;
	    } else if ( widgetType == "RTEXT" ) {
		ID = IDLabel;
		hasText = TRUE;
	    } else if ( widgetType == "EDITTEXT" ) {
		ID = IDLineEdit;
	    } else if ( widgetType == "GROUPBOX" ) {
		ID = IDGroupBox;
		hasText = TRUE;
	    } else if ( widgetType == "COMBOBOX" ) {
		ID = IDComboBox;
	    } else if ( widgetType == "LISTBOX" ) {
		ID = IDListBox;
	    } else if ( widgetType == "SCROLLBAR" ) {
		ID = IDScrollBar;
	    } else if ( widgetType == "CHECKBOX" ) {
		ID = IDCheckBox;
		hasText = TRUE;
	    } else if ( widgetType == "RADIOBUTTON" ) {
		ID = IDRadioButton;
		hasText = TRUE;
	    } else if ( widgetType == "CONTROL" ) {
		isControl = TRUE;
		widgetText = stripQM(parseNext( arguments ));
		widgetID = parseNext( arguments );
		controlType = stripQM(parseNext( arguments ));
		styles = splitStyles(parseNext( arguments ));
		
		if ( controlType == "Static" ) {
		    ID = IDLabel;
		} else if ( controlType == "Button" ) {
		    if ( styles.contains("BS_AUTOCHECKBOX") ||
			 styles.contains("BS_3STATE") )
			ID = IDCheckBox;
		    else if ( styles.contains("BS_AUTORADIOBUTTON") )
			ID = IDRadioButton;
		} else if ( controlType == "msctls_updown32" ) {
		    ID = IDSpinBox;
		} else if ( controlType == "msctls_progress32" ) {
		    ID = IDProgressBar;
		} else if ( controlType == "msctls_trackbar32" ) {
		    ID = IDSlider;
		} else if ( controlType == "SysListView32" ) {
		    ID = IDIconView;
		} else if ( controlType == "SysTreeView32" ) {
		    ID = IDListView;
		} else if ( controlType == "SysTabControl32" ) {
		    ID = IDTabWidget;
		} else if ( controlType == "SysAnimate32" ) {
		    ID = IDLabel;
		} else if ( controlType == "RICHEDIT" ) {
		    ID = IDMultiLineEdit;
		} else if ( controlType == "ComboBoxEx32" ) {
		    ID = IDComboBox;
		} else if ( controlType == "" ) {
		    ID = IDCustom;
		} else {
		    ID = IDUnknown;
		}
	    } else
		ID = IDUnknown;

	    if ( hasText )
		widgetText = stripQM(parseNext( arguments ));

	    if ( isControl ) {
		x = parseNext( arguments ).toInt();
		y = parseNext( arguments ).toInt();
		w = parseNext( arguments ).toInt();
		h = parseNext( arguments ).toInt();
	    } else {
		widgetID = parseNext( arguments );
		x = parseNext( arguments ).toInt();
		y = parseNext( arguments ).toInt();
		w = parseNext( arguments ).toInt();
		h = parseNext( arguments ).toInt();
		styles.clear();
	    }

	    do {
		extendedStyles = splitStyles(parseNext( arguments ));
		for ( uint i = 0; i < extendedStyles.count(); i++ )
		    styles << (*extendedStyles.at(i));
	    } while ( arguments.find(',') > -1 );

	    switch ( ID ) {
	    case IDWidget:
		break;
	    case IDPushButton: 
		{
		    writeClass("QPushButton");
		    writeCString( "name", useName("PushButton_"+widgetID) );
		    writeRect( "geometry", x, y, w, h );
		    writeString( "text", widgetText );
		    if ( widgetType == "DEFPUSHBUTTON" )
			writeBool( "default", TRUE );
		}
		break;
	    case IDLabel:
		{
		    isFrame = TRUE,
		    writeClass("QLabel");
		    writeCString( "name", useName("Label_"+widgetID) );
		    writeRect( "geometry", x,y,w,h );
		    writeString( "text", widgetText );
		    QString align;
		    if ( !styles.contains("SS_CENTERIMAGE") )
			align += "|AlignTop";
		    else
			align += "|AlignVCenter";
		    if ( widgetType == "LTEXT" ) {
			align += "|AlignLeft";
		    } else if ( widgetType == "CTEXT") {
			align += "|AlignHCenter";
		    } else if ( widgetType == "RTEXT") {
			align += "|AlignRight";
		    }
		    writeSet("alignment", align );
		}
		break;
	    case IDCheckBox:
		{
		    writeClass("QCheckBox");
		    writeCString("name", useName("CheckBox_"+widgetID) );
		    writeRect("geometry", x,y,w,h);
		    writeString("text", widgetText );
		    if ( styles.contains( "BS_3STATE" ) )
			writeBool( "tristate", TRUE );
		}
		break;
	    case IDRadioButton:
		{
		    writeClass("QRadioButton");
		    writeCString("name", useName("RadioButton_"+widgetID) );
		    writeRect("geometry", x,y,w,h);
		    writeString("text", widgetText );
		}
		break;
	    case IDGroupBox:
		{
		    isFrame = TRUE;
		    writeClass("QGroupBox");
		    writeCString( "name", useName("GroupBox_"+widgetID) );
		    writeRect( "geometry", x,y,w,h );
		    writeString( "title", widgetText );
		    if ( !styles.contains( "WS_BORDER" ) )
			styles.append( "WS_BORDER" );
		}
		break;
	    case IDLineEdit:
		{
		    if ( !styles.contains("ES_MULTILINE") ) {
			writeClass("QLineEdit");
			writeCString( "name", useName("LineEdit_"+widgetID) );
		    } else {
			writeClass("QMultiLineEdit");
			writeCString( "name", useName("MultiLineEdit_"+widgetID) );
		    }
		    writeRect( "geometry", x,y,w,h );
		    QString align = "AlignTop";
		    if ( styles.contains("ES_CENTER") )
			align+="|AlignHCenter";
		    else if ( styles.contains("ES_RIGHT") )
			align+="|AlignRight";
		    else
			align+="|AlignLeft";
		    writeSet("alignment", align);
		}
		break;
	    case IDMultiLineEdit:
		{
		    writeClass("QMultiLineEdit");
		    writeCString("name", useName("MultiLineEdit_"+widgetID) );
		    writeRect("geometry", x,y,w,h );
		}
		break;
	    case IDIconView:
		{
		    isFrame = TRUE;
		    writeClass("QIconView");
		    writeCString("name", useName("IconView_"+widgetID) );
		    writeRect("geometry", x,y,w,h );
		    if ( !styles.contains( "LVS_SINGLESEL" ) )
			writeEnum( "selectionMode", "Extended" );
		    if ( styles.contains( "LVS_NOLABELWRAP" ) )
			writeBool("wordWrapIconText", FALSE );
		}
		break;
	    case IDListView:
		{
		    isFrame = TRUE;
		    writeClass("QListView");
		    writeCString("name", useName("ListView_"+widgetID) );
		    writeRect("geometry", x,y,w,h );
		    if ( styles.contains( "TVS_LINESATROOT" ) )
			writeBool( "rootIsDecorated", TRUE );
		    if ( styles.contains( "TVS_FULLROWSELECT" ) )
			writeBool( "allColumnsShowFocus", TRUE );
		}
		break;
	    case IDProgressBar:
		{
		    isFrame = TRUE;
		    writeClass("QProgressBar");
		    writeCString("name", useName("ProgressBar_"+widgetID) );
		    writeRect("geometry", x,y,w,h );
		    if ( styles.contains("TBS_VERT") )
			writeEnum("orientation", "Vertical");
		    else
			writeEnum("orientation", "Horizontal");
		}
		break;
	    case IDTabWidget:
		{
		    writeClass("QTabWidget");
		    writeCString("name", useName("TabWidget_"+widgetID) );
		    writeRect("geometry", x,y,w,h );
		    wi(); *out << "<widget>" << endl; indent();
		    writeClass("QWidget");
		    wi(); *out << "<attribute>" << endl; indent();
		    wi(); *out << "<name>title</name>" << endl;
		    wi(); *out << "<string>Tab1</string>" << endl; undent();
		    wi(); *out << "</attribute>" << endl; undent();
		    wi(); *out << "</widget>" << endl;
		}
		break;
	    case IDSpinBox:
		{
		    isFrame = TRUE;
		    writeClass("QSpinBox");
		    writeCString("name", useName("SpinBox_"+widgetID) );
		    writeRect("geometry", x,y,w,h);
		}
		break;
	    case IDSlider:
		{
		    writeClass("QSlider");
		    writeCString("name", useName("Slider_"+widgetID) );
		    writeRect("geometry", x,y,w,h );
		    if ( styles.contains("TBS_VERT") )
			writeEnum("orientation", "Vertical");
		    else
			writeEnum("orientation", "Horizontal");
		    if ( !styles.contains("TBS_NOTICKS") )
			writeEnum("tickmarks", "Left" );
		}
		break;
	    case IDComboBox:
		{
		    writeClass("QComboBox");
		    writeCString("name", useName("ComboBox_"+widgetID) );
		    if ( isControl )
			writeRect( "geometry", x,y,w,14 );
		    else 
			writeRect( "geometry", x,y,w,h );
		}
		break;
	    case IDListBox:
		{
		    isFrame = TRUE;
		    writeClass("QListBox");
		    writeCString("name", useName("ListBox_"+widgetID) );
		    writeRect( "geometry", x,y,w,h );
		    if ( styles.contains("WS_HSCROLL") )
			writeEnum("hScrollBarMode", "Auto");
		    else
			writeEnum("hScrollBarMode", "AlwaysOff");
		    if ( styles.contains("WS_VSCROLL") )
			writeEnum("vScrollBarMode", "Auto");
		    else
			writeEnum("vScrollBarMode", "AlwaysOff");
		    if ( styles.contains("LBS_EXTENDEDSEL") )
			writeEnum("selectionMode", "Extended");
		    else if ( styles.contains("LBS_MULTIPLESEL") )
			writeEnum("selectionMode", "Multi");
		    else if ( styles.contains("LBS_NOSEL") )
			writeEnum("selectionMode", "NoSelection");
		    else 
			writeEnum("selectionMode", "Single");
		    if ( !styles.contains( "NO WS_BORDER" ) )
			styles.append( "WS_BORDER" );
		}
		break;
	    case IDScrollBar:
		{
		    writeClass("QScrollBar");
		    writeCString("name", useName("ScrollBar_"+widgetID) );
		    writeRect("geometry", x,y,w,h );
		    if ( styles.contains("SBS_VERT") )
			writeEnum("orientation", "Vertical");
		    else
			writeEnum("orientation", "Horizontal");
		}
		break;
	    case IDCustom:
		{
		    writeClass("QLabel");
		    writeCString("name", useName("Custom_"+widgetID) );
		    writeRect("geometry", x,y,w,h );
		    writeString("text", "Create a custom widget and place it here." );
		}
	    default:
		{
		    writeClass("QLabel");
		    writeCString("name", useName("Unknown_"+widgetID) );
		    writeRect("geometry", x,y,w,h );
		    writeString("text", QString("No support for %1.").arg(controlType) );
		}
		break;
	    }

	    writeStyles( styles, isFrame );

	    styles.clear();

	    undent();
	    wi(); *out << "</widget>" << endl;
	} while ( line != "END" );

	undent();
	wi(); *out << "</widget>" << endl;
	*out << "</UI>" << endl;

	do {
	    line = in->readLine();
	} while ( line.isEmpty() );

	if ( !writeToFile )
	    target.append( buffer.copy() );

	if (out) {
	    delete out;
	    out = 0;
	}
	fileOut.close();

	if ( writeToFile )
	    printf( QDir::currentDirPath() + "/" + fileOut.name() + '\n' );

    } while ( line != blockStart1 );

    return TRUE;
}

/*! Not yet implemented
*/

bool RC2UI::makeBitmap()
{
    return TRUE;
}

/*! Not yet implemented
*/

bool RC2UI::makeAccelerator()
{
    return TRUE;
}

/*! Not yet implemented
*/

bool RC2UI::makeCursor()
{
    return TRUE;
}

/*! Not yet implemented
*/

bool RC2UI::makeHTML()
{
    return TRUE;
}

/*! Not yet implemented
*/

bool RC2UI::makeIcon()
{
    return TRUE;
}

/*! 
  Writes a stringtable from the input stream to a c++ header file.
  All strings are assigned using QT_TR_NOOP to enable easy translation.
*/

bool RC2UI::makeStringTable()
{
    if ( !writeToFile )
	return TRUE;

    QFile fileOut;
    line = in->readLine();
    do {
	char stringtable[256];
	char discard[12];
	sscanf( line, "%s %s", stringtable, discard );
	if ( QString(stringtable) != "STRINGTABLE" )
	    return TRUE;
	do {
	    line = in->readLine();
	} while ( line != "BEGIN" );

	QString outputFile = QString(stringtable).lower() + ".h";
	if (outputFile ) {
	    fileOut.setName( outputFile );
	    if (!fileOut.open( IO_WriteOnly ) )
		qFatal( "rc2ui: Could not open output file '%s'", outputFile.latin1() );
	    out = new QTextStream( &fileOut );
	}

	*out << "#ifndef STRINGTABLE_H" << endl;
	*out << "#define STRINGTABLE_H" << endl;
	*out << endl;
	*out << "#include <qstring.h>" << endl;
	*out << "#include <qobject.h>" << endl;
	*out << endl;

	QString ID;
	QString value;
	do {
	    line = in->readLine().stripWhiteSpace();
	    if ( line == "END" )
		continue;
	    
	    ID = parseNext(line, ' ');
	    value = parseNext(line).stripWhiteSpace();

	    *out << "static const QString " << ID << "= QT_TR_NOOP(" << value << ");" << endl;

	} while ( line != "END" );

	*out << endl;
	*out << "#endif // STRINGTABLE_H" << endl;

	do {
	    line = in->readLine();
	} while ( line.isEmpty() );

	if ( out ) {
	    delete out;
	    out = 0;
	}
    } while ( line != blockStart1 );

    return TRUE;
}

/*! Not yet implemented
*/

bool RC2UI::makeVersion()
{
    return TRUE;
}
