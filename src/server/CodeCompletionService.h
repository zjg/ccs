
#ifndef CODECOMPLETIONSERVICE_H
#define CODECOMPLETIONSERVICE_H

#include <QtCore/QObject>

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
   
private:
   TranslationUnitManager& tuManager_;
};

#endif
