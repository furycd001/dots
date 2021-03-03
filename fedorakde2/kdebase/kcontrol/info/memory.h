#ifndef _MEMORY_H_KDEINFO_INCLUDED_
#define _MEMORY_H_KDEINFO_INCLUDED_

#include <qwidget.h>
#include <qframe.h>
#include <qlabel.h>
#include <qtabdialog.h>
#include <qpushbutton.h>
#include <qtimer.h>

#include <kcmodule.h>
#include <config.h>

#ifdef HAVE_LONG_LONG
/* better to use long-long, because some 32bit-machines have more total 
   memory (with swap) than just the 4GB which fits into a 32bit-long */
typedef unsigned long long t_memsize;
#else
typedef unsigned long t_memsize;
#endif

#define COLOR_USED_MEMORY QColor(255,0,0)
#define COLOR_USED_SWAP   QColor(255,134,64)
#define COLOR_FREE_MEMORY QColor(127,255,212)

class KMemoryWidget:public KCModule {
  Q_OBJECT 
	  
  public:
    KMemoryWidget(QWidget * parent, const char *name = 0);
    ~KMemoryWidget();

  private:
    QString Not_Available_Text;
    QTimer *timer;

    bool ram_colors_initialized,
	swap_colors_initialized, 
	all_colors_initialized;

    QColor ram_colors[4];
    QString ram_text[4];
    
    QColor swap_colors[2];
    QString swap_text[2];
    
    QColor all_colors[3];
    QString all_text[3];

    void update();

    bool Display_Graph(int widgetindex,
				      int count,
				      t_memsize total,
				      t_memsize *used, 
				      QColor *color,
				      QString *text);
    public slots:
    void update_Values();
};


#endif // _MEMORY_H_KDEINFO_INCLUDED_

