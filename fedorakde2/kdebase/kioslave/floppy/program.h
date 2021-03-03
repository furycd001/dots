#ifndef PROGRAM_H
#define PROGRAM_H

#include <qstringlist.h>

/**
 * start programs and write to thieir stdin, stderr,
 * and read from stdout
 **/
class Program
{
public:
	Program(const QStringList &args);
	~Program();
	bool start();
	bool isRunning();

   int stdinFD() {return mStdin[1];};
   int stdoutFD() {return mStdout[0];};
   int stderrFD() {return mStderr[0];};
   int pid()      {return m_pid;};
   int kill();
   int select(int secs, int usecs, bool& stdoutReceived, bool& stderrReceived/*, bool& stdinWaiting*/);
protected:
	int mStdout[2];
	int mStdin[2];
	int mStderr[2];
   int m_pid;
	QStringList mArgs;
	bool mStarted;
};

#endif

