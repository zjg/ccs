
#ifndef CLANGTRANSLATIONUNIT_H
#define CLANGTRANSLATIONUNIT_H

#include <clang-c/Index.h>

#include <QtCore/QByteArray>
#include <QtCore/QFileInfo>
#include <QtCore/QStringList>
#include <QtCore/QVector>

class ClangIndex;

class ClangTranslationUnit
{
public:
   explicit ClangTranslationUnit(ClangIndex& index,
                                 QFileInfo srcFile,
                                 QStringList includeDirs);
   virtual ~ClangTranslationUnit();

   QFileInfo fileInfo() const;
   CXTranslationUnit transUnit() const;

   void parse();
   void update();
   
   void loadFromFile(QFileInfo tuFile);
   void saveToFile(QFileInfo tuFile);

private: // functions
   Q_DISABLE_COPY(ClangTranslationUnit);
   
   void disposeTu();

private: // members
   ClangIndex& index_;
   QFileInfo srcFile_;
   CXTranslationUnit tu_;
   
   QList<QByteArray> clangArgsData_;
   QVector<const char*> clangArgs_;
   
private: // static
   static const QStringList defaultIncludeDirs_;
   static const QStringList extraClangArgs_;
   static const unsigned tuOptions_;
};

#endif
