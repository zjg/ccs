
#include <QDateTime>

#include "ClangTranslationUnit.h"

#include "TranslationUnitManager.h"

TranslationUnitManager::TranslationUnitManager(
   ClangIndex& index,
   QStringList includeDirs, QDir tuSaveDir)
   : index_(index)
   , includeDirs_(includeDirs)
   , tuSaveDir_(tuSaveDir)
   , tuCache_(15)
{
   if (!tuSaveDir.mkpath(tuSaveDir.absolutePath()))
   {
      qDebug("Unable to create TU save dir [%s]", qPrintable(tuSaveDir.absolutePath()));
   }
}

TranslationUnitManager::~TranslationUnitManager()
{
}

ClangTranslationUnit* TranslationUnitManager::translationUnit(QString sourceFile)
{
   // qDebug("cache before:");
   // foreach (QString name, tuCache_.keys())
   // {
   //    qDebug("    [%s]", qPrintable(name));
   // }

   updateTranslationUnit(sourceFile);

   // qDebug("cache after:");
   // foreach (QString name, tuCache_.keys())
   // {
   //    qDebug("    [%s]", qPrintable(name));
   // }

   QString tuFile = tuFileFromSourceFile(sourceFile);
   if (!tuCache_.contains(tuFile))
   {
      qDebug("cache doesn't contain TU ! [%s]", qPrintable(tuFile));
      return NULL;
   }
   return tuCache_[tuFile];
}

void TranslationUnitManager::updateTranslationUnit(QString sourceFile)
{
   qDebug("updating TU for [%s]", qPrintable(sourceFile));

   QFileInfo sourceFileInfo(sourceFile);
   if (!sourceFileInfo.exists())
   {
      qDebug("Source file [%s] doesn't exist", qPrintable(sourceFile));
      return;
   }

   QString tuFile = tuFileFromSourceFile(sourceFile);
   QFileInfo tuFileInfo(tuFile);

   if (!tuCache_.contains(tuFile))
   {
      ClangTranslationUnit* tu =
         new ClangTranslationUnit(index_, sourceFile, includeDirs_);

      if (tuFileInfo.exists())
      {
         qDebug("   ... TU file exists, loading");
         tu->loadFromFile(tuFile);
      }
      else
      {
         qDebug("   ... no TU file found, parsing & saving");
         tu->parse();
         tu->saveToFile(tuFile);
      }

      // insert into cache last, since it takes ownership
      tuCache_.insert(tuFile, tu);
   }

   tuFileInfo.refresh();
   if (sourceFileInfo.lastModified() > tuFileInfo.lastModified())
   {
      qDebug("   ... src newer than TU, updating & saving");
      tuCache_[tuFile]->update();
      tuCache_[tuFile]->saveToFile(tuFile);
   }
}

QString TranslationUnitManager::tuFileFromSourceFile(QString sourceFile) const
{
   QFileInfo sourceFileInfo(sourceFile);
   QString relativePath = tuSaveDir_.relativeFilePath(sourceFileInfo.filePath());
   relativePath.remove("../");
   return QFileInfo(tuSaveDir_, relativePath + ".tu").absoluteFilePath();
}

