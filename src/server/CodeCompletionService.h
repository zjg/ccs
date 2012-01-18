
#ifndef CODECOMPLETIONSERVICE_H
#define CODECOMPLETIONSERVICE_H

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QString>

#include <CCSMessages.h>
class ClangTranslationUnit;

class CodeCompletionService : public QObject
{
   Q_OBJECT

public:
   explicit CodeCompletionService(
      const QMap<QString, ClangTranslationUnit*>& transUnits);
   virtual ~CodeCompletionService();
   
public slots:
   void processRequest(CCSMessages::CodeCompletionRequest request);
   
private:
   const QMap<QString, ClangTranslationUnit*> transUnits_;
};

#endif
