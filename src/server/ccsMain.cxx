
#include <clang-c/Index.h>
#include <inotifytools/inotifytools.h>

#include <QCoreApplication>
#include <QStringList>
#include <QTime>
#include <QDebug>

#include "ClangIndex.h"
#include "ClangString.h"
#include "ClangTranslationUnit.h"

#include "CCServer.h"
#include "CodeCompletionService.h"
#include "FileChangeNotifier.h"
#include "SourceFinder.h"
#include "TranslationUnitManager.h"

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
   }

   int inotify_init = inotifytools_initialize();
   qDebug("[inotifytools: init = %d]", inotify_init);

   ClangIndex index;

   SourceFinder finder;
   finder.ignoreDirs() << "3rdparty";
   finder.ignoreFileRegexps() << QRegExp("moc_.*");
   QStringList sourceFiles;

   qDebug() << app.arguments();
   if (app.arguments().size() > 1)
   {
      sourceFiles = finder.findSourceFiles(app.arguments()[1]);
   }
   else
   {
      sourceFiles = finder.findSourceFiles(".");
   }

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

   CodeCompletionService ccService(tuManager);

   CCServer server(ccService);

   app.exec();

   inotifytools_cleanup();
   return 0;
}
