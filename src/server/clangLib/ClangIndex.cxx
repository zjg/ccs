
#include <ClangIndex.h>

ClangIndex::ClangIndex()
{
   index_ = clang_createIndex(0, 1);
}

ClangIndex::~ClangIndex()
{
   clang_disposeIndex(index_);
}

ClangIndex::operator CXIndex() const
{
   return index_;
}
