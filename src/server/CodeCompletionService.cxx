
#include <clang-c/Index.h>

#include <QTime>

#include "ClangTranslationUnit.h"
#include "TranslationUnitManager.h"
#include "CodeCompletionService.h"

CodeCompletionService::CodeCompletionService(
   TranslationUnitManager& tuManager)
   : tuManager_(tuManager)
{
}

CodeCompletionService::~CodeCompletionService()
{
}

void CodeCompletionService::processRequest(
   CCSMessages::CodeCompletionRequest request)
{
   ClangTranslationUnit* tu = tuManager_.translationUnit(request.filename);
   if (tu == NULL)
   {
      qDebug("unable to get TU for [%s]", qPrintable(request.filename));
      return;
   }
   
   QTime timer;
   timer.start();
   
   CXCodeCompleteResults* results =
      clang_codeCompleteAt(*tu,
                           qPrintable(request.filename),
                           request.line, request.column,
                           /* unsaved files */ NULL, 0,
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
