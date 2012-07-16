
#ifndef CLANGTRANSLATIONUNIT_H
#define CLANGTRANSLATIONUNIT_H

#include <clang-c/Index.h>

#include <QByteArray>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QStringList>
#include <QVector>

class ClangIndex;

class ClangTranslationUnit
{
public:
   explicit ClangTranslationUnit(ClangIndex& index,
                                 QString srcFile);
   explicit ClangTranslationUnit(ClangIndex& index,
                                 QString srcFile,
                                 QStringList includeDirs);
   virtual ~ClangTranslationUnit();

   QString sourceFile() const;
   operator CXTranslationUnit() const;

   bool isValid() const;
   QDateTime lastModified() const;
   
   void parse();
   void update();

   void loadFromFile(QString tuFile);
   void saveToFile(QString tuFile);

private: // functions
   Q_DISABLE_COPY(ClangTranslationUnit);

   void disposeTu();
   
   void initClangArgs(const QStringList& args);

private: // members
   ClangIndex& index_;
   QString srcFile_;
   QString buildCmdWorkingDir_;
   CXTranslationUnit tu_;
   QDateTime lastModified_;

   QList<QByteArray> clangArgsData_;
   QVector<const char*> clangArgs_;

private: // static
   static const unsigned tuOptions_;
};

#endif
