
extern "C" {
   #include <clang-c/Index.h>
}

#include <ClangString.h>

#include <Qt/QtCore>

int main(int argc, char* argv[])
{
   clang_enableStackTraces();

   ClangString version(clang_getClangVersion());
   qDebug("[%s]", qPrintable(version));

   CXIndex index = clang_createIndex(0, 1);
   
   
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
   
   QList<QByteArray> clangArgsData;
   QVector<const char*> clangArgs;
   foreach (QFileInfo info, sourceFiles)
   {
      QByteArray arg(qPrintable("-I" + info.path()));
      if (!clangArgsData.contains(arg))
      {
         clangArgsData.append(arg);
         clangArgs.append(clangArgsData.last().constData());
      }
   }
   
   // gcc headers from: `gcc -print-prog-name=cc1plus` -v
   QStringList extraArgs = QStringList()
      << "-I/usr/lib/gcc/i686-redhat-linux/4.4.6/../../../../include/c++/4.4.6"
      << "-I/usr/lib/gcc/i686-redhat-linux/4.4.6/../../../../include/c++/4.4.6/i686-redhat-linux"
      << "-I/usr/lib/gcc/i686-redhat-linux/4.4.6/../../../../include/c++/4.4.6/backward"
      << "-I/usr/local/include"
      << "-I/usr/lib/gcc/i686-redhat-linux/4.4.6/include"
      << "-I/usr/include";
   foreach (QString extraArg, extraArgs)
   {
      clangArgsData.append(qPrintable(extraArg));
         clangArgs.append(clangArgsData.last().constData());
   }
   
   foreach (const char* arg, clangArgs)
   {
      qDebug("clang arg: %s", arg);
   }
   
   QList<CXTranslationUnit> transUnits;
   
   foreach (QFileInfo info, sourceFiles)
   {
      if (IMPL_SUFFIXES.contains(info.suffix()))
      {
         qDebug("parsing source file: %s", qPrintable(info.filePath()));
         
         QTime timer;
         timer.start();
         
         transUnits.append(
            clang_parseTranslationUnit(
               index, qPrintable(info.filePath()),
               clangArgs.constData(), clangArgs.size(),
               NULL, 0, CXTranslationUnit_CacheCompletionResults));
         
         qDebug("   ... took %dms", timer.elapsed());
      }
   }
   
   
   foreach (CXTranslationUnit tu, transUnits)
   {
      clang_disposeTranslationUnit(tu);
   }
   
   clang_disposeIndex(index);
   
   return 0;
}
