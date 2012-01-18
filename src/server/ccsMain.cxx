
#include <CLucene.h>
#include <clang-c/Index.h>
#include <inotifytools/inotifytools.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QMap>
#include <QtCore/QStack>
#include <QtCore/QStringList>
#include <QtCore/QTime>

#include <ClangIndex.h>
#include <ClangString.h>
#include <ClangTranslationUnit.h>

#include <CCSMessaging.h>
#include <CodeCompletionService.h>

int main(int argc, char* argv[])
{
   clang_enableStackTraces();
   QCoreApplication app(argc, argv);

   {  // library tests
      ClangString version(clang_getClangVersion());
      qDebug("[%s]", qPrintable(version));
      
      qDebug("[inotifytools: max watches = %d, max instances = %d, max queue = %d]",
             inotifytools_get_max_user_watches(),
             inotifytools_get_max_user_instances(),
             inotifytools_get_max_queued_events());
      
      qDebug("[inotifytools: init = %d]",
             inotifytools_initialize());
      
      lucene::debug::LuceneBase cluceneObj;
   }

   ClangIndex index;

   const QStringList HEADER_SUFFIXES = QStringList() << "h" << "hpp";
   const QStringList IMPL_SUFFIXES = QStringList() << "cxx" << "cpp" << "c" << "c++";
   const QStringList SRC_SUFFIXES = HEADER_SUFFIXES + IMPL_SUFFIXES;
   
   const QStringList IGNORE_DIRS = QStringList() << "3rdparty" << ".obj";
   
   QFileInfoList sourceFiles;
   {  // finding source files
      QStack<QDir> dirStack;
      dirStack.push(QDir::current());
      while (!dirStack.isEmpty())
      {
         QDir dir = dirStack.pop();
         // qDebug("listing dir: %s", qPrintable(dir.path()));
         foreach (QFileInfo info, dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot))
         {
            // qDebug("found: %s", qPrintable(info.filePath()));
            
            if (info.isDir())
            {
               if (!IGNORE_DIRS.contains(info.fileName()))
               {
                  dirStack.push(QDir(info.filePath()));
               }
            }
            else if (SRC_SUFFIXES.contains(info.suffix()))
            {
               sourceFiles.append(info);
            }
         }
      }
   }
   
   QStringList includeDirs;
   {  // building list of include dirs
      foreach (QFileInfo info, sourceFiles)
      {
         QString dir = info.path();
         if (!includeDirs.contains(dir))
         {
            qDebug("include dir: %s", qPrintable(dir));
            includeDirs.append(dir);
         }
      }
   }
   
   QMap<QString, ClangTranslationUnit*> transUnits;
   {  // parsing source files
      foreach (QFileInfo info, sourceFiles)
      {
         qDebug("parsing source file: %s", qPrintable(info.filePath()));
         
         QTime timer;
         timer.start();
         
         transUnits[info.filePath()] = new ClangTranslationUnit(index, info);
         transUnits[info.filePath()]->parse(includeDirs);
         
         qDebug("   ... took %dms", timer.elapsed());
      }
   }
   
   {  // running code completion server
      CCSMessaging messaging;
      CodeCompletionService ccService(transUnits);
      QObject::connect(&messaging, SIGNAL(requestReceived(CCSMessages::CodeCompletionRequest)),
                       &ccService, SLOT(processRequest(CCSMessages::CodeCompletionRequest)));
   
      app.exec();
   }

   qDeleteAll(transUnits);

   inotifytools_cleanup();
   return 0;
}
