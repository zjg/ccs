
#ifndef CLANGSTRING_H
#define CLANGSTRING_H

#include <clang-c/Index.h>

#include <QString>

class ClangString : public QString
{
public:
   explicit ClangString(CXString string);
   virtual ~ClangString();

private:
   Q_DISABLE_COPY(ClangString);
   
   const CXString cxString_;
};

#endif
