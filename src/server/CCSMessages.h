
#ifndef CCSMESSAGES_H
#define CCSMESSAGES_H

#include <QList>
#include <QString>

namespace CCSMessages
{
   typedef int ClientId;

   struct CodeCompletionRequest
   {
      ClientId client;
      QString filename;
      int line;
      int column;
   };
   
   struct CodeCompletionResult
   {
      QString text;
   };
   typedef QList<CodeCompletionResult> CodeCompletionResultList;
   
   struct CodeCompletionResponse
   {
      const CodeCompletionRequest request;
      CodeCompletionResultList results;
      
      CodeCompletionResponse(CodeCompletionRequest r) : request(r) {}
   };
}

#endif
