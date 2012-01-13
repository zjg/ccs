
#include <cstdio>

extern "C" {
   #include <clang-c/Index.h>
}

#include <QString>

QString toQString(CXString cxString)
{
   QString qString(clang_getCString(cxString));
   clang_disposeString(cxString);
   return qString;
}

int main(int argc, char* argv[])
{
   clang_enableStackTraces();

   printf("[%s]\n", qPrintable(toQString(clang_getClangVersion())));

   return 0;
}
