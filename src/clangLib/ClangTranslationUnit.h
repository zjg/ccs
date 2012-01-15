
#ifndef CLANGTRANSLATIONUNIT_H
#define CLANGTRANSLATIONUNIT_H

#include <clang-c/Index.h>

#include <QtCore/QFileInfo>
#include <QtCore/QStringList>

#include <ClangIndex.h>

class ClangTranslationUnit
{
public:
   explicit ClangTranslationUnit(ClangIndex& index, QFileInfo srcFile);
   virtual ~ClangTranslationUnit();

   void parse(QStringList includeDirs);

private: // functions
   Q_DISABLE_COPY(ClangTranslationUnit);
   
private: // members
   ClangIndex& index_;
   QFileInfo srcFile_;
   CXTranslationUnit* tu_;
   
private: // static
   static QStringList defaultIncludeDirs_;
};

#endif
