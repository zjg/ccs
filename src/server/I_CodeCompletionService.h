
#ifndef I_CODECOMPLETIONSERVICE_H
#define I_CODECOMPLETIONSERVICE_H

#include "ccs_types.h"

class I_CodeCompletionService
{
public:
   virtual ~I_CodeCompletionService() {}

   virtual ccs::CodeCompletionResponse process(const ccs::CodeCompletionRequest& request) = 0;
};

#endif
