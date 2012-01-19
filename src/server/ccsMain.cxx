
#include <CLucene.h>
#include <clang-c/Index.h>
#include <inotifytools/inotifytools.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QFileInfo>
#include <QtCore/QMap>
#include <QtCore/QStringList>
#include <QtCore/QTime>

#include <ClangIndex.h>
#include <ClangString.h>
#include <ClangTranslationUnit.h>

#include <CCSMessaging.h>
#include <CodeCompletionService.h>
#include <SourceFinder.h>

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
   
   SourceFinder finder;
   finder.ignoreDirs() << "3rdparty";
   finder.ignoreFileRegexps() << QRegExp("moc_.*");
   QFileInfoList sourceFiles = finder.findSourceFiles(".");
   
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
