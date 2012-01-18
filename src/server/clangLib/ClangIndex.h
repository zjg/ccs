
#ifndef CLANGINDEX_H
#define CLANGINDEX_H

#include <clang-c/Index.h>

#include <QtCore/QtGlobal>

class ClangIndex
{
public:
   explicit ClangIndex();
   virtual ~ClangIndex();
   
   operator CXIndex() const;
   
private:
   Q_DISABLE_COPY(ClangIndex);
   
   CXIndex index_;
};

#endif
