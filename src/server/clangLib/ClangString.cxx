
#include "ClangString.h"

ClangString::ClangString(CXString string)
   : QString(clang_getCString(string))
   , cxString_(string)
{
}

ClangString::~ClangString()
{
   clang_disposeString(cxString_);
}
