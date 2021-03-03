#include "grapher.h"

Graph::Graph( GraphModel& mdl ) :
    model(mdl),
    style(Bar),
    pieRotationTimer(0),
    pieRotation(0)
{
    // Create a menubar for the widget
    //
    menubar = new QMenuBar( this );
    stylemenu = new QPopupMenu;
    stylemenu->setCheckable(TRUE);
    for ( Style s = Pie; styleName[s]; s = Style(s+1)) {
	stylemenu->insertItem(styleName[s], s+100);
    }
    connect(stylemenu, SIGNAL(activated(int)),
	this, SLOT(setStyleFromMenu(int)));
    setStyle(Pie);

    menubar->insertItem("Style", stylemenu);
    menubar->insertSeparator();
     
    QPopupMenu* help = new QPopupMenu;
    help->insertItem( "About plugin...", this, SIGNAL(aboutPlugin()) );
    help->insertItem( "About data...", this, SIGNAL(aboutData()) );
    menubar->insertItem("Help", help);
}

Graph::~Graph()
{
}

void Graph::setStyle(Style s)
{
    if (style != s) {
	if (pieRotationTimer)
	    killTimer(pieRotationTimer);
	stylemenu->setItemChecked(100+style, FALSE);
	style = s;
	if ( style == Pie )
	    pieRotationTimer = startTimer( 80 );
	else
	    pieRotationTimer = 0;
	stylemenu->setItemChecked(100+style, TRUE);
	update();
    }
}

void Graph::timerEvent(QTimerEvent*)
{
    pieRotation = ( pieRotation + 6 ) % 360; 
    repaint(false);    
}

void Graph::setStyle(const char* stext)
{
    for ( Style s = Pie; styleName[s]; s = Style(s+1) ) {
	if ( qstricmp(stext,styleName[s])==0 ) {
	    setStyle(s);
	    return;
	}
    }
}

void Graph::enterInstance()
{    
    menubar->show();
}

void Graph::leaveInstance()
{
    menubar->hide();
}

void Graph::paintError(const char* e)
{
    QPainter p(this);
    int w = width();
    p.drawText(w/8, 0, w-w/4, height(), AlignCenter|WordBreak, e);
}

void Graph::paintBar(QPaintEvent* event)
{
    if ( model.colType(0) != GraphModel::Numeric ) {
	paintError("First column not numeric, cannot draw bar graph\n");
	return;
    }

    QList<GraphModel::Datum>& data = model.graphData();

    double max = 0.0;

    for (GraphModel::Datum* rowdata = data.first();
	rowdata; rowdata = data.next())
    {
	if (rowdata[0].dbl > max) max = rowdata[0].dbl;
    }

    const uint w = width();
    const uint h = height();

    QPainter p(this);

    p.setClipRect(event->rect());

    if ( w > data.count() ) {
	// More pixels than data
	int x = 0;
	int i = 0;
	QFontMetrics fm=fontMetrics();
	int fh = fm.height();

	for (GraphModel::Datum* rowdata = data.first();
	    rowdata; rowdata = data.next())
	{
	    QColor c;
	    c.setHsv( (i * 255)/data.count(), 255, 255 );// rainbow effect
	    p.setBrush(c);
	    int bw = (w-w/4-x)/(data.count()-i);
	    int bh = int((h-h/4-1)*rowdata[0].dbl/max);
	    p.drawRect( w/8+x, h-h/8-1-bh, bw, bh );
	    
	    // ### This causes a crash, so comment out for now	    
	    /*if (model.colType(1) == GraphModel::Label) {
		p.drawText(w/8+x, h-h/8, bw, fh+h/8,
		    WordBreak|AlignTop|AlignHCenter,
		    *rowdata[1].str);
	    }*/
	    i++;
	    x+=bw;
	}
    } else {
	// More data than pixels
	int x = 0;
	int i = 0;
	double av = 0.0;
	int n = 0;
	for (GraphModel::Datum* rowdata = data.first(); rowdata;
	    rowdata = data.next())
	{
	    int bx = i*w/data.count();

	    if (bx > x) {
		QColor c;
		c.setHsv( (x * 255)/w, 255, 255 );// rainbow effect
		p.setPen(c);
		int bh = int(h*av/n/max);

		p.drawLine(x,h-1,x,h-bh);

		av = 0.0;
		n = 0;
		x = bx;
	    }

	    av += rowdata[0].dbl;
	    n++;

	    i++;
	}
    }
}

void Graph::paintPie(QPaintEvent* event)
{
    if ( model.colType(0) != GraphModel::Numeric ) {
	paintError("First column not numeric, cannot draw pie graph\n");
	return;
    }

    QList<GraphModel::Datum>& data = model.graphData();

    double total = 0.0;

    GraphModel::Datum* rowdata;

    for (rowdata = data.first();
	rowdata; rowdata = data.next())
    {
	total += rowdata[0].dbl;
    }

    // Only use first column for pie chart
    if ( !total ) return;

    int apos = (pieRotation-90)*16;

    const int w = width();
    const int h = height();

    const int xd = w - w/5;
    const int yd = h - h/5;

    pm.resize(width(),height());
    pm.fill(backgroundColor());
    QPainter p(&pm);
    p.setFont(font());

    p.setClipRect(event->rect());

    int i = 0;

    for (rowdata = data.first();
	rowdata; rowdata = data.next())
    {
	QColor c;

	c.setHsv( ( i * 255)/data.count(), 255, 255 );// rainbow effect
	p.setBrush( c );			// solid fill with color c

	int a = int(( rowdata[0].dbl * 360.0 ) / total * 16.0 + 0.5);
	p.drawPie( w/10, h/10, xd, yd, -apos, -a );
	apos += a;
	i++;
    }

    if (model.colType(1) == GraphModel::Label) {
	double apos = (pieRotation-90)*M_PI/180;

	for (rowdata = data.first();
	    rowdata; rowdata = data.next())
	{
	    double a = rowdata[0].dbl * 360 / total * M_PI / 180;
	    int x = int(cos(apos+a/2)*w*5/16 + w/2 + 0.5);
	    int y = int(sin(apos+a/2)*h*5/16 + h/2 + 0.5);
	    
	    // ### This causes a crash, so comment out for now
	    /*p.drawText(x-w/8, y-h/8, w/4, h/4,
		WordBreak|AlignCenter,
		*rowdata[1].str);*/
	    apos += a;
	}
    }

    QPainter p2(this);
    p2.setClipRect(event->rect());
    p2.drawPixmap(0,0,pm);
}

void Graph::paintWait(QPaintEvent*)
{
    QPainter p(this);
    p.drawText(rect(), AlignCenter, "Loading...");
}

void Graph::paintEvent(QPaintEvent* event)
{    
    if (!model.nCols()) {
	paintWait(event);
    } else {
	switch (style) {
	  case Pie:
	    paintPie(event);
	    break;
	  case Bar:
	    paintBar(event);
	    break;
	}
    }
    
}

void Graph::setStyleFromMenu(int id)
{
    setStyle(Style(id-100));
}

const char* Graph::styleName[] = { "Pie", "Bar", 0 };




Grapher::Grapher()
{
    data.setAutoDelete(TRUE);
    firstline = TRUE;
    ncols = 0;
    line.open(IO_WriteOnly|IO_Truncate);
}

Grapher::~Grapher()
{
}

QList<GraphModel::Datum>& Grapher::graphData()
{
    return data;
}

GraphModel::ColType Grapher::colType(int col) const
{
    return coltype[col];
}

int Grapher::nCols() const
{
    return ncols;
}


QNPWidget* Grapher::newWindow()
{
    // Create a Graph - our subclass of QNPWidget.
    Graph *graph = new Graph(*this);

    // Look at the arguments from the EMBED tag.
    //   GRAPHSTYLE chooses pie or bar
    //   FONTFAMILY and FONTSIZE choose the font
    //
    const char* style = arg("GRAPHSTYLE");
    if ( style ) graph->setStyle(style);

    const char* fontfamily = arg("FONTFAMILY");
    const char* fontsize = arg("FONTSIZE");
    int ptsize = fontsize ? atoi(fontsize) : graph->font().pointSize();
    if (fontfamily) graph->setFont(QFont(fontfamily, ptsize));

    connect(graph, SIGNAL(aboutPlugin()), this, SLOT(aboutPlugin()));
    connect(graph, SIGNAL(aboutData()), this, SLOT(aboutData()));

    return graph;
}

void Grapher::consumeLine()
{
    line.close();
    line.open(IO_ReadOnly);

    QTextStream ts( &line );

    if (firstline) {
	firstline = FALSE;
	ncols=0;
	QList<ColType> typelist;
	typelist.setAutoDelete(TRUE);
	do {
	    QString typestr;
	    ts >> typestr >> ws;
	    ColType* t = 0;
	    if ( typestr == "num" ) {
		t = new ColType(Numeric);
	    } else if ( typestr == "label" ) {
		t = new ColType(Label);
	    }
	    if (t) typelist.append(t);
	} while (!ts.eof());
	coltype = new ColType[ncols];
	for (ColType* t = typelist.first(); t; t = typelist.next()) {
	    coltype[ncols++] = *t;
	}
    } else {
	int col=0;
	Datum *rowdata = new Datum[ncols];
	while ( col < ncols && !ts.eof() ) {
	    switch (coltype[col]) {
	      case Numeric: {
		double value;
		ts >> value >> ws;
		rowdata[col].dbl = value;
		break;
	      }
	      case Label: {
		QString* value = new QString;
		ts >> *value >> ws;
		rowdata[col].str = value;
		break;
	      }
	    }
	    col++;
	}

	data.append(rowdata);
    }

    line.close();
    line.open(IO_WriteOnly|IO_Truncate);
}

int Grapher::write(QNPStream* /*str*/, int /*offset*/, int len, void* buffer)
{
    // The browser calls this function when data is available on one
    // of the streams the plugin has requested.  Since we are only
    // processing one stream - the URL in the SRC argument of the EMBED
    // tag, we assume the QNPStream is that one.  Also, since we do not
    // override QNPInstance::writeReady(), we must accepts ALL the data
    // that is sent to this function.
    //
    char* txt = (char*)buffer;
    for (int i=0; i<len; i++) {
	char ch = txt[i];
	switch ( ch ) {
	  case '\n':
	    consumeLine();
	    break;
	  case '\r': // ignore;
	    break;
	  default:
	    line.putch(ch);
	}
    }

    if ( widget() ) {
	widget()->update();
    }

    return len;
}

void Grapher::aboutPlugin()
{
    getURL( "http://www.trolltech.com/nsplugin/", "_blank" );
}

void Grapher::aboutData()
{
    const char* page = arg("DATAPAGE");
    if (page)
	getURL( page, "_blank" );
    else
	QMessageBox::message("Help", "No help for this data");
}


//
// GrapherPlugin is the start of everything.  It is a QNPlugin subclass,
// and it is responsible for describing the plugin to the browser, and
// creating instances of the plugin when it appears in web page.
//

class GrapherPlugin : public QNPlugin {
public:
    GrapherPlugin()
    {
    }

    QNPInstance* newInstance()
    {
	// Make a new Grapher, our subclass of QNPInstance.
	return new Grapher;
    }

    const char* getMIMEDescription() const
    {
	// Describe the MIME types which this plugin can
	// process.  Just the concocted "application/x-graphable"
	// type, with the "g1n" filename extension.
	//
	return "application/x-graphable:g1n:Graphable ASCII numeric data";
    }

    const char * getPluginNameString() const
    {
	// The name of the plugin.  This is the title string used in
	// the "About Plugins" page of the browser.
	//
	return "Qt-based Graph Plugin";
    }

    const char * getPluginDescriptionString() const
    {
	// A longer description of the plugin.
	//
	return "A Qt-based LiveConnected plug-in that graphs numeric data";
    }

};

//
// Finally, we provide the implementation of QNPlugin::create(), to
// provide our subclass of QNPlugin.
//

QNPlugin* QNPlugin::create()
{
    return new GrapherPlugin;
}

