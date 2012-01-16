
#ifndef CODECOMPLETIONMESSAGING_H
#define CODECOMPLETIONMESSAGING_H

#include <QtCore/QObject>

class CodeCompletionRequest;

class CodeCompletionMessaging : public QObject
{
   Q_OBJECT
public:
   explicit CodeCompletionMessaging();
   virtual ~CodeCompletionMessaging();
   
};

#endif
