
#include <CLucene.h>
#include <clang-c/Index.h>
#include <inotifytools/inotifytools.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QStringList>
#include <QtCore/QTime>

#include "ClangIndex.h"
#include "ClangString.h"
#include "ClangTranslationUnit.h"

#include "CCSMessaging.h"
#include "CodeCompletionService.h"
#include "TranslationUnitManager.h"
#include "FileChangeNotifier.h"
#include "SourceFinder.h"

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
   QStringList sourceFiles = finder.findSourceFiles(".");
   
   QStringList includeDirs;
   {  // building list of include dirs
      foreach (QFileInfo info, sourceFiles)
      {
         QString dir = info.path();
         if (!includeDirs.contains(dir))
         {
            // qDebug("include dir: %s", qPrintable(dir));
            includeDirs.append(dir);
         }
      }
   }
   
   
   FileChangeNotifier notifier(sourceFiles);
   TranslationUnitManager tuManager(index, includeDirs, QDir(".ccsd"));
   QObject::connect(&notifier, SIGNAL(fileChanged(QString)),
                    &tuManager, SLOT(updateTranslationUnit(QString)));
   
   CCSMessaging messaging;
   CodeCompletionService ccService(tuManager);
   QObject::connect(&messaging, SIGNAL(requestReceived(CCSMessages::CodeCompletionRequest)),
                    &ccService, SLOT(processRequest(CCSMessages::CodeCompletionRequest)));
   QObject::connect(&ccService, SIGNAL(completionComplete(CCSMessages::CodeCompletionResponse)),
                    &messaging, SLOT(sendResponse(CCSMessages::CodeCompletionResponse)));
   
   
   app.exec();
   
   inotifytools_cleanup();
   return 0;
}
