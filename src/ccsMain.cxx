
#include <clang-c/Index.h>

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

int main(int argc, char* argv[])
{
   clang_enableStackTraces();
   QCoreApplication app(argc, argv);

   {
      ClangString version(clang_getClangVersion());
      qDebug("[%s]", qPrintable(version));
   }

   ClangIndex index;

   const QStringList HEADER_SUFFIXES = QStringList() << "h" << "hpp";
   const QStringList IMPL_SUFFIXES = QStringList() << "cxx" << "cpp" << "c" << "c++";
   const QStringList SRC_SUFFIXES = HEADER_SUFFIXES + IMPL_SUFFIXES;
   
   QFileInfoList sourceFiles;
   {
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
               dirStack.push(QDir(info.filePath()));
            }
            else if (SRC_SUFFIXES.contains(info.suffix()))
            {
               sourceFiles.append(info);
            }
         }
      }
   }
   
   QStringList includeDirs;
   {
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
   {
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
   
   if (app.arguments().size() > 1)
   {
      QStringList args = app.arguments();
      QString completionFilename = args[1];
      unsigned int completionLine = args[2].toUInt();
      unsigned int completionColumn = args[3].toUInt();
      
      QFileInfo fileInfo(completionFilename);
      if (!transUnits.contains(fileInfo.filePath()))
      {
         qWarning("Unable to find file '%s' for completion",
                  qPrintable(fileInfo.filePath()));
      }
      else
      {
         QTime timer;
         timer.start();
         
         CXCodeCompleteResults* results =
            clang_codeCompleteAt(*transUnits[fileInfo.filePath()]->transUnit(),
                                 qPrintable(completionFilename),
                                 completionLine, completionColumn,
                                 NULL, 0,
                                 clang_defaultCodeCompleteOptions());
         if (results == NULL)
         {
            qWarning("Code completion failed");
         }
         else
         {
            qDebug("found %d completion results in %dms",
                   results->NumResults, timer.elapsed());
            clang_disposeCodeCompleteResults(results);
         }
      }
   }
   
   qDeleteAll(transUnits);
   
   return 0;
}
