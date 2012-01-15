
#include <QtCore/QByteArray>
#include <QtCore/QVector>

#include <ClangTranslationUnit.h>

// gcc headers from: `gcc -print-prog-name=cc1plus` -v
QStringList ClangTranslationUnit::defaultIncludeDirs_ = QStringList()
   << "/usr/include/c++/4.4.6"
   << "/usr/include/c++/4.4.6/i686-redhat-linux"
   << "/usr/include/c++/4.4.6/backward"
   << "/usr/local/include"
   << "/usr/lib/gcc/i686-redhat-linux/4.4.6/include"
   << "/usr/include";

ClangTranslationUnit::ClangTranslationUnit(
   ClangIndex& index, QFileInfo srcFile)
   : index_(index)
   , srcFile_(srcFile)
   , tu_(0)
{
}

ClangTranslationUnit::~ClangTranslationUnit()
{
   if (tu_)
   {
      clang_disposeTranslationUnit(*tu_);
      delete tu_;
   }
}

void ClangTranslationUnit::parse(QStringList includeDirs)
{
   const unsigned options = clang_defaultEditingTranslationUnitOptions()
                           | CXTranslationUnit_CacheCompletionResults;
   
   QList<QByteArray> clangArgsData;
   QVector<const char*> clangArgs;
   foreach (QString incDir, (defaultIncludeDirs_ + includeDirs))
   {
      QByteArray arg(qPrintable("-I" + incDir));
      clangArgsData.append(arg);
      clangArgs.append(clangArgsData.last().constData());
   }
   
   // extraArgs << "-x" << "c++";   // clang ignores this???
   
   tu_ = new CXTranslationUnit(
      clang_parseTranslationUnit(index_, qPrintable(srcFile_.filePath()),
                                 clangArgs.constData(), clangArgs.size(),
                                 /* unsaved files */ NULL, 0,
                                 options));
}

