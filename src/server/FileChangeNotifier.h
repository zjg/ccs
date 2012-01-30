#ifndef FILECHANGENOTIFIER_H
#define FILECHANGENOTIFIER_H

#include <QObject>

class QTimer;

class FileChangeNotifier : public QObject
{
   Q_OBJECT
   
public:
   explicit FileChangeNotifier(QStringList filesToWatch);
   virtual ~FileChangeNotifier();
   
signals:
   void fileChanged(QString file);
   
private slots:
   void pollInotify();
   
private:
   QTimer* inotifyPollTimer_;
};

#endif
