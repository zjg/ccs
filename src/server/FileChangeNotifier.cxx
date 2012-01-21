
#include <inotifytools/inotify.h>
#include <inotifytools/inotifytools.h>

#include <QtCore/QByteArray>
#include <QtCore/QSet>
#include <QtCore/QTimer>
#include <QtCore/QVector>

#include <FileChangeNotifier.h>

FileChangeNotifier::FileChangeNotifier(QFileInfoList filesToWatch)
{
   QList<QByteArray> filenames;
   QVector<const char*> inotifyFilenames;
   foreach (QFileInfo info, filesToWatch)
   {
      filenames.append(qPrintable(info.absoluteFilePath()));
      inotifyFilenames.append(filenames.last().constData());
   }
   inotifyFilenames.append(0);
   
   int rval = inotifytools_watch_files(inotifyFilenames.data(),
                                       IN_CLOSE | IN_MODIFY | IN_DELETE);
   if (rval == 0)
   {
      qDebug("inotifytools_watch_files returned error %d",
             inotifytools_error());
   }
   
   inotifyPollTimer_ = new QTimer(this);
   connect(inotifyPollTimer_, SIGNAL(timeout()),
           this, SLOT(pollInotify()));
   inotifyPollTimer_->start(250);
}

FileChangeNotifier::~FileChangeNotifier()
{
}

void FileChangeNotifier::pollInotify()
{
   QSet<QString> modifiedFiles;
   inotify_event* event = inotifytools_next_event(0);
   while (event)
   {
      QString filename = inotifytools_filename_from_wd(event->wd);
      QString eventType = inotifytools_event_to_str(event->mask);
      qDebug("inotify event: %s - %s",
             qPrintable(filename), qPrintable(eventType));
      
      if (IN_MODIFY & event->mask)
      {
         modifiedFiles.insert(filename);
      }
      
      event = inotifytools_next_event(0);
   }
   
   foreach (QString filename, modifiedFiles)
   {
      emit fileChanged(QFileInfo(filename));
   }
}

