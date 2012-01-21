
#include <ClangIndex.h>
#include <ClangTranslationUnit.h>

// gcc headers from: `gcc -print-prog-name=cc1plus` -v
const QStringList ClangTranslationUnit::defaultIncludeDirs_ = QStringList()

   // archon
   << "/home/doug/local/include"
   << "/usr/include/c++/4.4.6"
   << "/usr/include/c++/4.4.6/i686-redhat-linux"
   << "/usr/include/c++/4.4.6/backward"
   << "/usr/local/include"
   << "/usr/lib/gcc/i686-redhat-linux/4.4.6/include"
   << "/usr/include"
   
   // work VM
   << "/home/drosvick/local/include"
   << "/home/drosvick/sw/3rdparty/qt-4.7.2/include"
   << "/opt/gcc/gcc-4.5.3/lib/gcc/i686-pc-linux-gnu/4.5.3/../../../../include/c++/4.5.3"
   << "/opt/gcc/gcc-4.5.3/lib/gcc/i686-pc-linux-gnu/4.5.3/../../../../include/c++/4.5.3/i686-pc-linux-gnu"
   << "/opt/gcc/gcc-4.5.3/lib/gcc/i686-pc-linux-gnu/4.5.3/../../../../include/c++/4.5.3/backward"
   << "/opt/gcc/gcc-4.5.3/include"
   << "/opt/gcc/gcc-4.5.3/lib/gcc/i686-pc-linux-gnu/4.5.3/include"
   << "/opt/gcc/gcc-4.5.3/lib/gcc/i686-pc-linux-gnu/4.5.3/include-fixed"
   
   // HDVSM
   << "/home/drosvick/workspaces/HDVSMDev/sw/3rdparty/qt-4.7.2/linux/include"
   << "/home/drosvick/workspaces/HDVSMDev/sw/3rdparty/qt-4.7.2/linux/include/QtCore"
   << "/home/drosvick/workspaces/HDVSMDev/sw/3rdparty/qt-4.7.2/linux/include/QtNetwork"
   << "/home/drosvick/workspaces/HDVSMDev/sw/3rdparty/qt-4.7.2/linux/include/QtGui"
   << "/home/drosvick/workspaces/HDVSMDev/sw/3rdparty/qt-4.7.2/linux/include/QtTest"
   << "/home/drosvick/workspaces/HDVSMDev/sw/3rdparty/qt-4.7.2/linux/include/QtXml"
   << "/home/drosvick/workspaces/HDVSMDev/sw/3rdparty/anubis/include"
   << "/home/drosvick/workspaces/HDVSMDev/sw/3rdparty/vcstoolkit/include"
   << "/home/drosvick/workspaces/HDVSMDev/sw/3rdparty/sqlite-3.3.6/include"
   ;

const QStringList ClangTranslationUnit::extraClangArgs_ = QStringList()
   << "-x" << "c++";

const unsigned ClangTranslationUnit::tuOptions_ =
   clang_defaultEditingTranslationUnitOptions()
   | CXTranslationUnit_CacheCompletionResults;

ClangTranslationUnit::ClangTranslationUnit(
   ClangIndex& index, QFileInfo srcFile, QStringList includeDirs)
   : index_(index)
   , srcFile_(srcFile)
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

QFileInfo ClangTranslationUnit::fileInfo() const
{
   return srcFile_;
}

CXTranslationUnit ClangTranslationUnit::transUnit() const
{
   return tu_;
}

void ClangTranslationUnit::parse()
{
   tu_ = clang_parseTranslationUnit(index_, qPrintable(srcFile_.absoluteFilePath()),
                                    clangArgs_.constData(), clangArgs_.size(),
                                    /* unsaved files */ NULL, 0,
                                    tuOptions_);
   if (tu_ == NULL)
   {
      qDebug("Failed to parse TU from file [%s]", qPrintable(srcFile_.absoluteFilePath()));
   }
}

void ClangTranslationUnit::update()
{
   if (tu_ == NULL)
   {
      parse();
      return;
   }
   
   int rval = clang_reparseTranslationUnit(tu_,
                                           /* unsaved files */ 0, NULL,
                                           tuOptions_);
   if (rval != 0)
   {
      qDebug("Failed to update TU, disposing & parsing");
      disposeTu();
      parse();
   }
}

void ClangTranslationUnit::loadFromFile(QFileInfo tuFile)
{
   if (tu_ != NULL)
   {
      disposeTu();
   }
   
   tu_ = clang_createTranslationUnit(index_, qPrintable(tuFile.absoluteFilePath()));
   if (tu_ == NULL)
   {
      qDebug("Failed to load TU from file [%s]", qPrintable(tuFile.absoluteFilePath()));
   }
}

void ClangTranslationUnit::saveToFile(QFileInfo tuFile)
{
   if (tu_ == NULL)
   {
      return;
   }
   
   int rval = clang_saveTranslationUnit(tu_, qPrintable(tuFile.absoluteFilePath()),
                                        CXSaveTranslationUnit_None);
   if (rval != CXSaveError_None)
   {
      QString msg = QString("Failed to save TU to [%1] - %2").arg(tuFile.absoluteFilePath());
      switch (rval)
      {
         case CXSaveError_Unknown:
            msg = msg.arg("I/O error");
            break;
         case CXSaveError_TranslationErrors:
            msg = msg.arg("source file contained parse errors");
            break;
         case CXSaveError_InvalidTU:
            msg = msg.arg("invalid TU");
            break;
      }
      qDebug(qPrintable(msg));
   }
}

