
#ifndef CODECOMPLETIONSERVICE_H
#define CODECOMPLETIONSERVICE_H

#include "ccs_types.h"

#include "I_CodeCompletionService.h"
class TranslationUnitManager;

class CodeCompletionService : public I_CodeCompletionService
{
public:
   explicit CodeCompletionService(TranslationUnitManager& tuManager);
   virtual ~CodeCompletionService();
   
   virtual ccs::CodeCompletionResponse process(const ccs::CodeCompletionRequest& request);
   
private:
   TranslationUnitManager& tuManager_;
};

#endif
