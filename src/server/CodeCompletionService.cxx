
#include <clang-c/Index.h>

#include <QTime>

#include "clangLib/ClangString.h"

#include "ClangTranslationUnit.h"
#include "CodeCompletionService.h"
#include "TranslationUnitManager.h"

CodeCompletionService::CodeCompletionService(
   TranslationUnitManager& tuManager)
   : tuManager_(tuManager)
{
}

CodeCompletionService::~CodeCompletionService()
{
}

ccs::CodeCompletionResponse CodeCompletionService::process(
   const ccs::CodeCompletionRequest& request)
{
   ClangTranslationUnit* tu = tuManager_.translationUnit(request.filename.c_str());
   if (tu == NULL)
   {
      qDebug("unable to get TU for [%s]", request.filename.c_str());
      return ccs::CodeCompletionResponse();
   }
   
   QTime timer;
   timer.start();
   
   qDebug("getting completion for '%s' at %d:%d", request.filename.c_str(),
          request.line, request.column);
   
   CXCodeCompleteResults* results =
      clang_codeCompleteAt(*tu,
                           request.filename.c_str(),
                           request.line, request.column,
                           /* unsaved files */ NULL, 0,
                           clang_defaultCodeCompleteOptions());
   if (results == NULL)
   {
      qWarning("Code completion failed");
      return ccs::CodeCompletionResponse();
   }
   
   qDebug("found %d completion results in %dms",
          results->NumResults, timer.elapsed());
   
   ccs::CodeCompletionResponse response;
   response.request = request;
   
   for (unsigned int i = 0; i < results->NumResults; ++i)
   {
      const CXCompletionResult& result = results->Results[i];
      
      QString output;
      unsigned int numChunks = clang_getNumCompletionChunks(result.CompletionString);
      for (unsigned int j = 0; j < numChunks; ++j)
      {
         ClangString chunk(clang_getCompletionChunkText(result.CompletionString, j));
         output += chunk;
      }
      
      response.results.push_back(output.toStdString());
      
      if (i < 10)
      {
         qDebug("   [%d] '%s'", i, qPrintable(output));
      }
   }
   
   clang_disposeCodeCompleteResults(results);
   
   return response;
}
