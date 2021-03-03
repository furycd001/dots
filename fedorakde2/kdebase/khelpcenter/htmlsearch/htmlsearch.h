#ifndef __HTMLSEARCH_H__
#define __HTMLSEARCH_H__


#include <qstring.h>
#include <qstringlist.h>


class QWidget;
class QProgressDialog;


class KProcess;


class ProgressDialog;


class HTMLSearch : public QObject
{
  Q_OBJECT

public:

  HTMLSearch();

  bool generateIndex(QString lang, QWidget *parent=0);

  QString search(QString lang, QString words, QString method="and", int matches=10,
		 QString format="builtin-long", QString sort="score");


protected slots:

  void htdigStdout(KProcess *proc, char *buffer, int buflen);
  void htdigExited(KProcess *proc);
  void htmergeExited(KProcess *proc);
  void htsearchStdout(KProcess *proc, char *buffer, int buflen);
  void htsearchExited(KProcess *proc);


protected:

  QString dataPath(const QString& lang);

  bool saveFilesList(const QString& lang);
  void scanDir(const QString& dir);

  bool createConfig(const QString& lang);


private:

  QStringList   _files;
  KProcess      *_proc;
  int           _filesToDig, _filesDigged, _filesScanned;
  volatile bool _htdigRunning, _htmergeRunning, _htsearchRunning;
  QString       _searchResult;
  ProgressDialog *progress;

};


#endif
