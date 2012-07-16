
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QTime>

#include "ClangIndex.h"
#include "ClangTranslationUnit.h"

const unsigned ClangTranslationUnit::tuOptions_ =
   clang_defaultEditingTranslationUnitOptions()
   | CXTranslationUnit_CacheCompletionResults;

ClangTranslationUnit::ClangTranslationUnit(
   ClangIndex& index, QString srcFile)
   : index_(index)
   , srcFile_(srcFile)
   , tu_(NULL)
{
   // get compiler flags from a file next to the source file
   QFileInfo srcInfo(srcFile);
   QDir srcDir = srcInfo.absoluteDir();
   
   QString buildFileName = QString(".%1.build_cmd").arg(srcInfo.fileName());
   if (srcDir.exists(buildFileName))
   {
      QFile buildFile(srcDir.filePath(buildFileName));
      if (buildFile.open(QIODevice::ReadOnly | QIODevice::Text))
      {
         QTextStream stream(&buildFile);
         buildCmdWorkingDir_ = stream.readLine();
         QString buildCmd = stream.readLine();
         
         qDebug("working dir for [%s]: '%s'", qPrintable(srcFile), qPrintable(buildCmdWorkingDir_));
         qDebug("build cmd for [%s]: '%s'", qPrintable(srcFile), qPrintable(buildCmd));
         
         initClangArgs(buildCmd.split(" ", QString::SkipEmptyParts));
      }
      else
      {
         qDebug("unable to open build cmd file for [%s]", qPrintable(srcFile));
      }
   }
   else
   {
      qDebug("unable to find build cmd file for [%s]", qPrintable(srcFile));
   }
}

ClangTranslationUnit::ClangTranslationUnit(
   ClangIndex& index, QString srcFile, QStringList includeDirs)
   : index_(index)
   , srcFile_(srcFile)
   , tu_(NULL)
{
   QStringList args;
   args << includeDirs.replaceInStrings(QRegExp("^"), "-I");
   args << srcFile;
   initClangArgs(args);
}

ClangTranslationUnit::~ClangTranslationUnit()
{
   disposeTu();
}

void ClangTranslationUnit::disposeTu()
{
   clang_disposeTranslationUnit(tu_);
   tu_ = NULL;
}

void ClangTranslationUnit::initClangArgs(const QStringList& args)
{
   foreach (QString arg, args)
   {
      clangArgsData_.append(qPrintable(arg));
   }
   
   // clangArgsData_.append("-v");
   clangArgsData_.append("-I/usr/local/lib/clang/3.2/include");
   
   for (int i = 0; i < clangArgsData_.size(); ++i)
   {
      clangArgs_.append(clangArgsData_[i].constData());
   }
}

bool ClangTranslationUnit::isValid() const
{
   return (tu_ != NULL);
}

QDateTime ClangTranslationUnit::lastModified() const
{
   return lastModified_;
}

QString ClangTranslationUnit::sourceFile() const
{
   return srcFile_;
}

ClangTranslationUnit::operator CXTranslationUnit() const
{
   return tu_;
}

void ClangTranslationUnit::parse()
{
   QTime timer;
   timer.start();

   QDir oldDir = QDir::current();
   QDir::setCurrent(buildCmdWorkingDir_);
   tu_ = clang_parseTranslationUnit(index_, NULL,
                                    clangArgs_.constData(), clangArgs_.size(),
                                    /* unsaved files */ NULL, 0,
                                    tuOptions_);
   // QDir::setCurrent(oldDir.absolutePath());
   if (tu_ == NULL)
   {
      qDebug("Failed to parse TU from file [%s]", qPrintable(srcFile_));
      return;
   }

   qDebug("   parsed [%s] in %dms", qPrintable(srcFile_), timer.elapsed());
   // qDebug() << "clang args:" << clangArgs_;
   
   lastModified_ = QDateTime::currentDateTime();
}

void ClangTranslationUnit::update()
{
   if (tu_ == NULL)
   {
      qDebug("   -- attempt to update NULL tu_, parsing");
      parse();
      return;
   }

   QTime timer;
   timer.start();

   int rval = clang_reparseTranslationUnit(tu_,
                                           /* unsaved files */ 0, NULL,
                                           tuOptions_);
   if (rval != 0)
   {
      qDebug("Failed to update TU, disposing & parsing");
      disposeTu();
      parse();
      return;
   }

   qDebug("   reparsed [%s] in %dms", qPrintable(srcFile_), timer.elapsed());
   lastModified_ = QDateTime::currentDateTime();
}

