
#include <clang-c/Index.h>

#include <QtCore/QTime>

#include <ClangTranslationUnit.h>
#include <CodeCompletionService.h>

CodeCompletionService::CodeCompletionService(
   const QMap<QString, ClangTranslationUnit*>& transUnits)
   : transUnits_(transUnits)
{
}

CodeCompletionService::~CodeCompletionService()
{
}

void CodeCompletionService::processRequest(
   CCSMessages::CodeCompletionRequest request)
{
   QFileInfo fileInfo(request.filename);
   if (!transUnits_.contains(fileInfo.filePath()))
   {
      qWarning("Unable to find file '%s' for completion",
               qPrintable(fileInfo.filePath()));
      return;
   }
   
   QTime timer;
   timer.start();
   
   CXCodeCompleteResults* results =
      clang_codeCompleteAt(transUnits_[fileInfo.filePath()]->transUnit(),
                           qPrintable(request.filename),
                           request.line, request.column,
                           NULL, 0,
                           clang_defaultCodeCompleteOptions());
   if (results == NULL)
   {
      qWarning("Code completion failed");
      return;
   }
   
   qDebug("found %d completion results in %dms",
          results->NumResults, timer.elapsed());
   clang_disposeCodeCompleteResults(results);
}
