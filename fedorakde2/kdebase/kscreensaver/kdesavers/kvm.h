//-----------------------------------------------------------------------------
//
// kvm screensaver
//

#ifndef __KVM_H__
#define __KVM_H__

#include <qtimer.h>
#include <qlist.h>
#include <qdialog.h>
#include <qlineedit.h>
#include <kscreensaver.h>

extern "C" {
#include "vm.h"
#include "vm_random.h"
}

#define	THREAD_MAX_STACK_SIZE	10
#define	MAX_THREADS_NUM		20

#define	MAX_REFRESH_TIMEOUT	40

typedef struct {
  QWidget *w; 
  int grid_width, grid_height;
  int grid_margin_x;
  int grid_margin_y;
  int char_width, char_height;
  bool insert_top_p, insert_bottom_p;
  int density;
  struct tvm_pool*	pool;
  char*	modified;
  int	show_threads;

  QPixmap images;
  int image_width, image_height;
  int nglyphs;

} m_state;


class kVmSaver : public KScreenSaver
{
	Q_OBJECT
public:
	kVmSaver( WId id );
	virtual ~kVmSaver();

	void setSpeed( int spd );
	void setRefreshTimeout( const int refreshTimeout );

protected:
	void blank();
	void readSettings();
        int getRandom( const int max_value );
        void modifyArea( const int op );

protected slots:
	void slotTimeout();

protected:
	QTimer      timer;
	int         colorContext;

	int         speed;
	m_state*    pool_state;
        int	refreshStep;
        int	refreshTimeout;
};


class kVmSetup : public QDialog
{
	Q_OBJECT
public:
	kVmSetup( QWidget *parent = NULL, const char *name = NULL );

protected:
	void readSettings();

private slots:
        void slotSpeed( int ); 
        void slotRefreshTimeout( int num ); 
	void slotOkPressed();
	void slotAbout();

private:
	QWidget *preview;
	kVmSaver *saver;

	int                     speed;
        int	refreshTimeout;
};


#endif

