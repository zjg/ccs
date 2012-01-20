#ifndef FILECHANGENOTIFIER_H
#define FILECHANGENOTIFIER_H

#include <QtCore/QFileInfo>
#include <QtCore/QObject>

class QTimer;

class FileChangeNotifier : public QObject
{
   Q_OBJECT
   
public:
   explicit FileChangeNotifier(QFileInfoList filesToWatch);
   virtual ~FileChangeNotifier();
   
signals:
   void fileChanged(QFileInfo file);
   
private slots:
   void pollInotify();
   
private:
   QTimer* inotifyPollTimer_;
};

#endif
