
#include <QDateTime>

#include "ClangTranslationUnit.h"

#include "TranslationUnitManager.h"

TranslationUnitManager::TranslationUnitManager(ClangIndex& index)
   : index_(index)
   , tuCache_(15)
{
}

TranslationUnitManager::~TranslationUnitManager()
{
}

ClangTranslationUnit* TranslationUnitManager::translationUnit(QString sourceFile)
{
   qDebug("cache before:");
   foreach (QString name, tuCache_.keys())
   {
      qDebug("    [%s]", qPrintable(name));
   }

   updateTranslationUnit(sourceFile);
   
   qDebug("cache after:");
   foreach (QString name, tuCache_.keys())
   {
      qDebug("    [%s]", qPrintable(name));
   }

   QFileInfo sourceFileInfo(sourceFile);
   QString absPath = sourceFileInfo.absoluteFilePath();
   if (!tuCache_.contains(absPath))
   {
      qDebug("cache doesn't contain TU ! [%s]", qPrintable(sourceFile));
      return NULL;
   }
   return tuCache_[absPath];
}

void TranslationUnitManager::updateTranslationUnit(QString sourceFile)
{
   QFileInfo sourceFileInfo(sourceFile);
   if (!sourceFileInfo.exists())
   {
      qDebug("Source file [%s] doesn't exist", qPrintable(sourceFile));
      return;
   }
   QString absPath = sourceFileInfo.absoluteFilePath();

   if (!tuCache_.contains(absPath))
   {
      ClangTranslationUnit* tu =
         new ClangTranslationUnit(index_, sourceFile);
      
      tu->parse();

      if (tu->isValid())
      {
         // cache takes ownership of pointer
         tuCache_.insert(absPath, tu);
      }
      else
      {
         qDebug("   ... TU is not valid...");
         delete tu;
      }
   }

   if (sourceFileInfo.lastModified() > tuCache_[absPath]->lastModified())
   {
      qDebug("   ... src newer than TU, updating");
      tuCache_[absPath]->update();
   }
}

