
#include <QtCore/QDir>
#include <QtCore/QStack>
#include <QtCore/QTime>

#include <SourceFinder.h>

SourceFinder::SourceFinder()
{
   srcFileFilters_ << "*.h" << "*.hpp";
   srcFileFilters_ << "*.c" << "*.cc" << "*.cpp" << "*.cxx";
}

SourceFinder::~SourceFinder()
{
}

QStringList& SourceFinder::ignoreDirs()
{
   return ignoreDirs_;
}

QList<QRegExp>& SourceFinder::ignoreFileRegexps()
{
   return ignoreFileRegexps_;
}

QFileInfoList SourceFinder::findSourceFiles(QString startingDir)
{
   QFileInfoList foundFiles;
   QStack<QDir> dirStack;
   QList<QDir> processedDirs;
   
   QTime timer;
   timer.start();
   
   dirStack.push(QDir(startingDir));
   while (!dirStack.isEmpty())
   {
      QDir dir = dirStack.pop();
      if (processedDirs.contains(dir))
      {
         continue;
      }
      processedDirs.append(dir);
      
      // source files
      foreach (QFileInfo info, dir.entryInfoList(srcFileFilters_,
                                                 QDir::Files | QDir::NoDotAndDotDot | QDir::Readable))
      {
         bool ignore = false;
         foreach (const QRegExp& regex, ignoreFileRegexps_)
         {
            if (regex.exactMatch(info.fileName()))
            {
               ignore = true;
               break;
            }
         }
         if (!ignore)
         {
            foundFiles.append(info);
         }
      }
      
      // directories
      foreach (QFileInfo info, dir.entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::Readable))
      {
         if (!ignoreDirs_.contains(info.fileName()))
         {
            dirStack.push(QDir(info.absoluteFilePath()));
         }
      }
   }
   qDebug("found %d source files in %dms", foundFiles.size(), timer.elapsed());
   return foundFiles;
}


