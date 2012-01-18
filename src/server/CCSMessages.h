
#ifndef CCSMESSAGES_H
#define CCSMESSAGES_H

#include <QtCore/QString>

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
}

#endif