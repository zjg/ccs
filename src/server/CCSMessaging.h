
#ifndef CCSMESSAGING_H
#define CCSMESSAGING_H

#include <QObject>
#include <QMap>
class QSignalMapper;
class QTcpServer;
class QAbstractSocket;

#include "CCSMessages.h"

class CCSMessaging : public QObject
{
   Q_OBJECT

public:
   explicit CCSMessaging();
   virtual ~CCSMessaging();
   
public slots:
   void sendResponse(CCSMessages::CodeCompletionResponse response);

signals:
   void requestReceived(CCSMessages::CodeCompletionRequest request);
   
private slots:
   void handleNewConnection();
   void handleSocketReady(int id);
   
private:
   QTcpServer* server_;
   QMap<CCSMessages::ClientId, QAbstractSocket*> clientSockets_;
   QSignalMapper* socketReadMapper_;
};

#endif
