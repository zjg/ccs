
#include <clang-c/Index.h>

#include <Qt/QtCore>

#include <ClangIndex.h>
#include <ClangString.h>
#include <ClangTranslationUnit.h>

int main(int argc, char* argv[])
{
   clang_enableStackTraces();

   ClangString version(clang_getClangVersion());
   qDebug("[%s]", qPrintable(version));

   ClangIndex index;
   
   const QStringList HEADER_SUFFIXES = QStringList() << "h" << "hpp";
   const QStringList IMPL_SUFFIXES = QStringList() << "cxx" << "cpp" << "c" << "c++";
   const QStringList SRC_SUFFIXES = HEADER_SUFFIXES + IMPL_SUFFIXES;
   
   QFileInfoList sourceFiles;
   QStack<QDir> dirStack;
   dirStack.push(QDir::current());
   while (!dirStack.isEmpty())
   {
      QDir dir = dirStack.pop();
      qDebug("listing dir: %s", qPrintable(dir.path()));
      foreach (QFileInfo info, dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot))
      {
         qDebug("found: %s", qPrintable(info.filePath()));
         
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
   
   QStringList includeDirs;
   foreach (QFileInfo info, sourceFiles)
   {
      QString dir = info.path();
      if (!includeDirs.contains(dir))
      {
         qDebug("include dir: %s", qPrintable(dir));
         includeDirs.append(dir);
      }
   }
   
   QList<ClangTranslationUnit*> transUnits;
   
   foreach (QFileInfo info, sourceFiles)
   {
      if (IMPL_SUFFIXES.contains(info.suffix()))
      {
         qDebug("parsing source file: %s", qPrintable(info.filePath()));
         
         QTime timer;
         timer.start();
         
         transUnits.append(new ClangTranslationUnit(index, info));
         transUnits.last()->parse(includeDirs);
         
         qDebug("   ... took %dms", timer.elapsed());
      }
   }
   
   qDeleteAll(transUnits);
   
   return 0;
}
