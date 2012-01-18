
#ifndef CLANGTRANSLATIONUNIT_H
#define CLANGTRANSLATIONUNIT_H

#include <clang-c/Index.h>

#include <QtCore/QFileInfo>
#include <QtCore/QStringList>

class ClangIndex;

class ClangTranslationUnit
{
public:
   explicit ClangTranslationUnit(ClangIndex& index,
                                 QFileInfo srcFile);
   virtual ~ClangTranslationUnit();

   QFileInfo fileInfo() const;

   void parse(QStringList includeDirs);

   CXTranslationUnit transUnit() const;

private: // functions
   Q_DISABLE_COPY(ClangTranslationUnit);
   
private: // members
   ClangIndex& index_;
   QFileInfo srcFile_;
   CXTranslationUnit tu_;
   
private: // static
   static QStringList defaultIncludeDirs_;
   static QStringList extraClangArgs_;
};

#endif
