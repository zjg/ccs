
#ifndef CLANGSTRING_H
#define CLANGSTRING_H

extern "C" {
   #include <clang-c/Index.h>
}

#include <QtCore/QString>

class ClangString : public QString
{
public:
   explicit ClangString(CXString string);
   virtual ~ClangString();

private:
   const CXString cxString_;
};

#endif
