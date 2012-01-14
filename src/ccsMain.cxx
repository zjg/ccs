
#include <cstdio>

extern "C" {
   #include <clang-c/Index.h>
}

#include <ClangString.h>

int main(int argc, char* argv[])
{
   clang_enableStackTraces();

   ClangString version(clang_getClangVersion());
   printf("[%s]\n", qPrintable(version));

   return 0;
}
