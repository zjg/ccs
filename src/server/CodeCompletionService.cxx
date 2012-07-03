
#include <map>

#include <clang-c/Index.h>

#include <QTime>

#include "clangLib/ClangString.h"

#include "ClangTranslationUnit.h"
#include "CodeCompletionService.h"
#include "TranslationUnitManager.h"

namespace
{
   typedef std::map<CXCompletionChunkKind, ccs::CodeCompletionChunkKind::type> ChunkKindMap;
   
   ChunkKindMap initChunkKindMap()
   {
      ChunkKindMap rtn;
      
      rtn[CXCompletionChunk_TypedText] = ccs::CodeCompletionChunkKind::TypedText;
      rtn[CXCompletionChunk_Text] = ccs::CodeCompletionChunkKind::Text;
      rtn[CXCompletionChunk_Placeholder] = ccs::CodeCompletionChunkKind::Placeholder;
      rtn[CXCompletionChunk_Informative] = ccs::CodeCompletionChunkKind::Informative;
      rtn[CXCompletionChunk_CurrentParameter] = ccs::CodeCompletionChunkKind::CurrentParameter;
      rtn[CXCompletionChunk_ResultType] = ccs::CodeCompletionChunkKind::ResultType;
      rtn[CXCompletionChunk_LeftParen] = ccs::CodeCompletionChunkKind::LeftParen;
      rtn[CXCompletionChunk_RightParen] = ccs::CodeCompletionChunkKind::RightParen;
      rtn[CXCompletionChunk_LeftBracket] = ccs::CodeCompletionChunkKind::LeftBracket;
      rtn[CXCompletionChunk_RightBracket] = ccs::CodeCompletionChunkKind::RightBracket;
      rtn[CXCompletionChunk_LeftBrace] = ccs::CodeCompletionChunkKind::LeftBrace;
      rtn[CXCompletionChunk_RightBrace] = ccs::CodeCompletionChunkKind::RightBrace;
      rtn[CXCompletionChunk_LeftAngle] = ccs::CodeCompletionChunkKind::LeftAngle;
      rtn[CXCompletionChunk_RightAngle] = ccs::CodeCompletionChunkKind::RightAngle;
      rtn[CXCompletionChunk_Comma] = ccs::CodeCompletionChunkKind::Comma;
      rtn[CXCompletionChunk_Colon] = ccs::CodeCompletionChunkKind::Colon;
      rtn[CXCompletionChunk_SemiColon] = ccs::CodeCompletionChunkKind::SemiColon;
      rtn[CXCompletionChunk_Equal] = ccs::CodeCompletionChunkKind::Equal;
      rtn[CXCompletionChunk_HorizontalSpace] = ccs::CodeCompletionChunkKind::Space;
      rtn[CXCompletionChunk_VerticalSpace] = ccs::CodeCompletionChunkKind::Newline;
      
      return rtn;
   }
   const ChunkKindMap chunkKindMap = initChunkKindMap();
   
   bool chunkHasText(ccs::CodeCompletionChunkKind::type ccsKind)
   {
      switch (ccsKind)
      {
         case ccs::CodeCompletionChunkKind::TypedText:
         case ccs::CodeCompletionChunkKind::Text:
         case ccs::CodeCompletionChunkKind::Placeholder:
         case ccs::CodeCompletionChunkKind::Informative:
         case ccs::CodeCompletionChunkKind::CurrentParameter:
         case ccs::CodeCompletionChunkKind::ResultType:
         {
            return true;
         }
         
         case ccs::CodeCompletionChunkKind::LeftParen:
         case ccs::CodeCompletionChunkKind::RightParen:
         case ccs::CodeCompletionChunkKind::LeftBracket:
         case ccs::CodeCompletionChunkKind::RightBracket:
         case ccs::CodeCompletionChunkKind::LeftBrace:
         case ccs::CodeCompletionChunkKind::RightBrace:
         case ccs::CodeCompletionChunkKind::LeftAngle:
         case ccs::CodeCompletionChunkKind::RightAngle:
         case ccs::CodeCompletionChunkKind::Comma:
         case ccs::CodeCompletionChunkKind::Colon:
         case ccs::CodeCompletionChunkKind::SemiColon:
         case ccs::CodeCompletionChunkKind::Equal:
         case ccs::CodeCompletionChunkKind::Space:
         case ccs::CodeCompletionChunkKind::Newline:
         {
            return false;
         }
      }
   }
}

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
   response.__set_request(request);
   
   for (unsigned int i = 0; i < results->NumResults; ++i)
   {
      const CXCompletionResult& result = results->Results[i];
      
      ccs::CodeCompletionCandidate candidate;
      
      QString chunkDebugBase = QString("   [%1] chunk[%2] kind=%3 text='%4'").arg(i);
      unsigned int numChunks = clang_getNumCompletionChunks(result.CompletionString);
      for (unsigned int j = 0; j < numChunks; ++j)
      {
         CXCompletionChunkKind clangKind = clang_getCompletionChunkKind(result.CompletionString, j);
      
         QString chunkDebug = chunkDebugBase.arg(j);
         
         ChunkKindMap::const_iterator chunkKindIt = chunkKindMap.find(clangKind);
         if(chunkKindIt != chunkKindMap.end())
         {
            ccs::CodeCompletionChunk ccsChunk;
            ccsChunk.__set_kind(chunkKindIt->second);
            
            {  // debug
               // std::map<int,const char*> ccs::_CodeCompletionChunkKind_VALUES_TO_NAMES::const_iterator kindNameIt;
               // kindNameIt = ccs::_CodeCompletionChunkKind_VALUES_TO_NAMES.find(ccsChunk.kind);
               // if (kindNameIt != ccs::_CodeCompletionChunkKind_VALUES_TO_NAMES.end())
               //    chunkDebug = chunkDebug.arg(kindNameIt->second);
               // else
               //    chunkDebug = chunkDebug.arg("<unknown>");
               
               chunkDebug = chunkDebug.arg(ccsChunk.kind);
            }
            
            if (chunkHasText(ccsChunk.kind))
            {
               ClangString chunkText(clang_getCompletionChunkText(result.CompletionString, j));
               ccsChunk.__set_text(chunkText.toStdString());
               
               chunkDebug = chunkDebug.arg(chunkText);
            }
            
            candidate.chunks.push_back(ccsChunk);
         }
         
         if (i < 10)
         {
            qDebug("%s", qPrintable(chunkDebug));
         }
      }
      
      response.results.push_back(candidate);
   }
   
   clang_disposeCodeCompleteResults(results);
   
   return response;
}
