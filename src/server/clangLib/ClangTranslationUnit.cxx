
#include <QTime>
#include <QDebug>

#include "ClangIndex.h"
#include "ClangTranslationUnit.h"

// gcc headers from: `gcc -print-prog-name=cc1plus` -v
const QStringList ClangTranslationUnit::defaultIncludeDirs_ = QStringList()

   // archon
   // << "/home/doug/local/include"
   // << "/usr/include/c++/4.4.6"
   // << "/usr/include/c++/4.4.6/i686-redhat-linux"
   // << "/usr/include/c++/4.4.6/backward"
   // << "/usr/local/include"
   // << "/usr/lib/gcc/i686-redhat-linux/4.4.6/include"
   // << "/usr/include"
   // << "/usr/include/Qt"
   // << "/usr/include/QtCore"
   // << "/usr/include/QtNetwork"

   // work VM
   // << "/home/drosvick/local/include"
   // << "/home/drosvick/sw/3rdparty/qt-4.7.2/include"
   // << "/opt/gcc/gcc-4.5.3/lib/gcc/i686-pc-linux-gnu/4.5.3/../../../../include/c++/4.5.3"
   // << "/opt/gcc/gcc-4.5.3/lib/gcc/i686-pc-linux-gnu/4.5.3/../../../../include/c++/4.5.3/i686-pc-linux-gnu"
   // << "/opt/gcc/gcc-4.5.3/lib/gcc/i686-pc-linux-gnu/4.5.3/../../../../include/c++/4.5.3/backward"
   // << "/opt/gcc/gcc-4.5.3/include"
   // << "/opt/gcc/gcc-4.5.3/lib/gcc/i686-pc-linux-gnu/4.5.3/include"
   // << "/opt/gcc/gcc-4.5.3/lib/gcc/i686-pc-linux-gnu/4.5.3/include-fixed"

   // HDVSM
   // << "/home/drosvick/workspaces/HDVSMDev/sw/3rdparty/qt-4.7.2/linux/include"
   // << "/home/drosvick/workspaces/HDVSMDev/sw/3rdparty/qt-4.7.2/linux/include/QtCore"
   // << "/home/drosvick/workspaces/HDVSMDev/sw/3rdparty/qt-4.7.2/linux/include/QtNetwork"
   // << "/home/drosvick/workspaces/HDVSMDev/sw/3rdparty/qt-4.7.2/linux/include/QtGui"
   // << "/home/drosvick/workspaces/HDVSMDev/sw/3rdparty/qt-4.7.2/linux/include/QtTest"
   // << "/home/drosvick/workspaces/HDVSMDev/sw/3rdparty/qt-4.7.2/linux/include/QtXml"
   // << "/home/drosvick/workspaces/HDVSMDev/sw/3rdparty/anubis/include"
   // << "/home/drosvick/workspaces/HDVSMDev/sw/3rdparty/vcstoolkit/include"
   // << "/home/drosvick/workspaces/HDVSMDev/sw/3rdparty/sqlite-3.3.6/include"

   // debian 6
   << "/usr/local/include"
   << "/usr/lib/gcc/i486-linux-gnu/4.4.5/include"
   << "/usr/lib/gcc/i486-linux-gnu/4.4.5/include-fixed"
   // << "/usr/include/c++/4.4"
   << "/usr/include"
   << "/usr/include/qt4"
   << "/usr/include/qt4/QtCore"
   << "/usr/include/qt4/QtNetwork"
   << "/usr/include/qt4/QtGui"
   << "/usr/local/include/CLucene/ext"
   << "/usr/local/include/thrift"
   ;

const QStringList ClangTranslationUnit::extraClangArgs_ = QStringList()
   << "-x" << "c++" << "-w" << "-ansi"
   << "-DHAVE_INTTYPES_H" << "-DHAVE_NETINET_IN_H";

const unsigned ClangTranslationUnit::tuOptions_ =
   clang_defaultEditingTranslationUnitOptions()
   | CXTranslationUnit_CacheCompletionResults;

ClangTranslationUnit::ClangTranslationUnit(
   ClangIndex& index, QString srcFile, QStringList includeDirs)
   : index_(index)
   , srcFile_(srcFile)
   , tu_(NULL)
{
   foreach (QString incDir, (defaultIncludeDirs_ + includeDirs))
   {
      clangArgsData_.append(qPrintable("-I" + incDir));
   }
   foreach (QString extraArg, extraClangArgs_)
   {
      clangArgsData_.append(QByteArray(qPrintable(extraArg)));
   }

   for (int i = 0; i < clangArgsData_.size(); ++i)
   {
      clangArgs_.append(clangArgsData_[i].constData());
   }
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

   tu_ = clang_parseTranslationUnit(index_, qPrintable(srcFile_),
                                    clangArgs_.constData(), clangArgs_.size(),
                                    /* unsaved files */ NULL, 0,
                                    tuOptions_);
   if (tu_ == NULL)
   {
      qDebug("Failed to parse TU from file [%s]", qPrintable(srcFile_));
   }

   qDebug("   parsed [%s] in %dms", qPrintable(srcFile_), timer.elapsed());
   qDebug() << "clang args:" << clangArgs_;
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
}

void ClangTranslationUnit::loadFromFile(QString tuFile)
{
   if (tu_ != NULL)
   {
      qDebug("   -- loading with existing tu_, disposing");
      disposeTu();
   }

   QTime timer;
   timer.start();

   tu_ = clang_createTranslationUnit(index_, qPrintable(tuFile));
   if (tu_ == NULL)
   {
      qDebug("Failed to load TU from file [%s]", qPrintable(tuFile));
   }

   qDebug("   loaded [%s] in %dms", qPrintable(srcFile_), timer.elapsed());
}

void ClangTranslationUnit::saveToFile(QString tuFile)
{
   if (tu_ == NULL)
   {
      qDebug("   -- attempt to save NULL tu_, doing nothing");
      return;
   }

   QTime timer;
   timer.start();

   int rval = clang_saveTranslationUnit(tu_, qPrintable(tuFile),
                                        CXSaveTranslationUnit_None);
   if (rval != CXSaveError_None)
   {
      QString msg = QString("Failed to save TU to [%1] - %2").arg(tuFile);
      switch (rval)
      {
         case CXSaveError_Unknown:
            msg = msg.arg("I/O error");
            break;
         case CXSaveError_TranslationErrors:
            msg = msg.arg("parse errors");
            break;
         case CXSaveError_InvalidTU:
            msg = msg.arg("invalid TU");
            break;
      }
      qDebug(qPrintable(msg));
   }

   qDebug("   saved [%s] in %dms", qPrintable(srcFile_), timer.elapsed());
}

