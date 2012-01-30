
#ifndef TRANSLATIONUNITMANAGER_H
#define TRANSLATIONUNITMANAGER_H

#include <QCache>
#include <QDir>
#include <QObject>

class ClangIndex;
class ClangTranslationUnit;

class TranslationUnitManager : public QObject
{
   Q_OBJECT

public:
   explicit TranslationUnitManager(ClangIndex& index,
                                   QStringList includeDirs,
                                   QDir tuSaveDir);
   virtual ~TranslationUnitManager();
   
   ClangTranslationUnit* translationUnit(QString sourceFile);
   
public slots:
   void updateTranslationUnit(QString sourceFile);
   
private:
   QString tuFileFromSourceFile(QString sourceFile) const;

private:
   ClangIndex& index_;
   QStringList includeDirs_;
   QDir tuSaveDir_;
   
   QCache<QString, ClangTranslationUnit> tuCache_;
};

#endif
