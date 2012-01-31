
#ifndef CODECOMPLETIONSERVICE_H
#define CODECOMPLETIONSERVICE_H

#include <QObject>

#include "CCSMessages.h"
class TranslationUnitManager;

class CodeCompletionService : public QObject
{
   Q_OBJECT

public:
   explicit CodeCompletionService(TranslationUnitManager& tuManager);
   virtual ~CodeCompletionService();
   
public slots:
   void processRequest(CCSMessages::CodeCompletionRequest request);
   
signals:
   void completionComplete(CCSMessages::CodeCompletionResponse response);
   
private:
   TranslationUnitManager& tuManager_;
};

#endif
