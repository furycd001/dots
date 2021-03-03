#if !defined(QT_NO_QSHM)

#if !defined(QT_QSHM_H)
#define QT_QSHM_H

#include <qstring.h>
#include <sys/types.h>
#include <sys/ipc.h>

class QSharedMemory {
public:
	QSharedMemory(){};
	QSharedMemory(int, QString);
	~QSharedMemory(){};

	bool create();
	void destroy();

	bool attach();
	void detach();

	void setPermissions(mode_t mode);
	void * base() { return shmBase; };

private:
	void *shmBase;
	int shmSize;
	QString shmFile;
#if defined(QT_POSIX_QSHM)
	int shmFD;
#else
	int shmId;
	key_t key;
	int idInitted;
#endif
};

#endif
#endif
