#include <qsharedmemory.h>

#if !defined(QT_QWS_NO_SHM)

#include <sys/ipc.h>
#include <sys/types.h>

#if defined(QT_POSIX_QSHM)
#include <fcntl.h>
#include <sys/mman.h>

QSharedMemory::QSharedMemory (int size, QString filename)
{
  shmSize = size;
  shmFile = filename;
}

bool QSharedMemory::create ()
{
  shmFD = shm_open (shmFile.latin1 (), O_RDWR | O_EXCL | O_CREAT, 0666);
  if (shmFD == -1)
    return FALSE;
  else if (ftruncate (shmFD, shmSize) == -1)
    {
      close (shmFD);
      return FALSE;
    }

  return TRUE;
}

void
QSharedMemory::destroy ()
{
  shm_unlink (shmFile.latin1 ());
}

bool QSharedMemory::attach ()
{
  shmBase = mmap (0, shmSize, PROT_READ | PROT_WRITE, MAP_SHARED, shmFD, 0);

  if (shmBase == MAP_FAILED)
    return FALSE;

  close (shmFD);
  return TRUE;
}

void
QSharedMemory::detach ()
{
  munmap (shmBase, shmSize);
}

void
QSharedMemory::setPermissions (mode_t mode)
{
  mprotect (shmBase, shmSize, mode);	// Provide defines to make prot work properly
}

#else // Assume SysV for backwards compat
#include <sys/shm.h>

QSharedMemory::QSharedMemory (int size, QString filename)
{
  shmSize = size;
  shmFile = filename;
  key = ftok (shmFile.latin1 (), 'Q');
  idInitted = FALSE;
}

bool QSharedMemory::create ()
{
  shmId = shmget (key, shmSize, IPC_CREAT | 0666);
  if (shmId == -1)
    return FALSE;
  else
    return TRUE;
}

void
QSharedMemory::destroy ()
{
  struct shmid_ds shm;
  shmctl (shmId, IPC_RMID, &shm);
}

bool QSharedMemory::attach ()
{
  if (shmId == -1)
    shmId = shmget (key, shmSize, 0);

  shmBase = shmat (shmId, 0, 0);
  if ( *((int*)(&shmBase)) == -1 || shmBase == 0)
    return FALSE;
  else
    return TRUE;
}

void
QSharedMemory::detach ()
{
  shmdt (shmBase);
}

void
QSharedMemory::setPermissions (mode_t mode)
{
  struct shmid_ds shm;
  shmctl (shmId, IPC_STAT, &shm);
  shm.shm_perm.mode = mode;
  shmctl (shmId, IPC_SET, &shm);
}

#endif

#endif
