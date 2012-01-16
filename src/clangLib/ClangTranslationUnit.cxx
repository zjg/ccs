
#include <QtCore/QByteArray>
#include <QtCore/QVector>

#include <ClangIndex.h>
#include <ClangTranslationUnit.h>

// gcc headers from: `gcc -print-prog-name=cc1plus` -v
QStringList ClangTranslationUnit::defaultIncludeDirs_ = QStringList()
   << "/home/doug/local/include"
   << "/usr/include/c++/4.4.6"
   << "/usr/include/c++/4.4.6/i686-redhat-linux"
   << "/usr/include/c++/4.4.6/backward"
   << "/usr/local/include"
   << "/usr/lib/gcc/i686-redhat-linux/4.4.6/include"
   << "/usr/include";

QStringList ClangTranslationUnit::extraClangArgs_ = QStringList()
   << "-x" << "c++";

ClangTranslationUnit::ClangTranslationUnit(
   ClangIndex& index, QFileInfo srcFile)
   : index_(index)
   , srcFile_(srcFile)
{
}

ClangTranslationUnit::~ClangTranslationUnit()
{
   clang_disposeTranslationUnit(tu_);
}

QFileInfo ClangTranslationUnit::fileInfo() const
{
   return srcFile_;
}

CXTranslationUnit ClangTranslationUnit::transUnit() const
{
   return tu_;
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
   
   foreach (QString extraArg, extraClangArgs_)
   {
      clangArgsData.append(QByteArray(qPrintable(extraArg)));
      clangArgs.append(clangArgsData.last().constData());
   }
   
   tu_ = clang_parseTranslationUnit(index_, qPrintable(srcFile_.filePath()),
                                    clangArgs.constData(), clangArgs.size(),
                                    /* unsaved files */ NULL, 0,
                                    options);
}

