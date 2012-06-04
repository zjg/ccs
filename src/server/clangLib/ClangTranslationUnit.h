
#ifndef CLANGTRANSLATIONUNIT_H
#define CLANGTRANSLATIONUNIT_H

#include <clang-c/Index.h>

#include <QByteArray>
#include <QFileInfo>
#include <QStringList>
#include <QVector>

class ClangIndex;

class ClangTranslationUnit
{
public:
   explicit ClangTranslationUnit(ClangIndex& index,
                                 QString srcFile,
                                 QStringList includeDirs);
   virtual ~ClangTranslationUnit();

   QString sourceFile() const;
   operator CXTranslationUnit() const;

   void parse();
   void update();

   void loadFromFile(QString tuFile);
   void saveToFile(QString tuFile);

private: // functions
   Q_DISABLE_COPY(ClangTranslationUnit);

   void disposeTu();

private: // members
   ClangIndex& index_;
   QString srcFile_;
   CXTranslationUnit tu_;

   QList<QByteArray> clangArgsData_;
   QVector<const char*> clangArgs_;

private: // static
   static const QStringList defaultIncludeDirs_;
   static const QStringList extraClangArgs_;
   static const unsigned tuOptions_;
};

#endif
