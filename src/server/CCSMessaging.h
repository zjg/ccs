
#ifndef CCSMESSAGING_H
#define CCSMESSAGING_H

#include <QtCore/QObject>
#include <QtCore/QMap>
class QSignalMapper;
class QTcpServer;
class QTcpSocket;

#include "CCSMessages.h"

class CCSMessaging : public QObject
{
   Q_OBJECT

public:
   explicit CCSMessaging();
   virtual ~CCSMessaging();
   
public slots:
   
signals:
   void requestReceived(CCSMessages::CodeCompletionRequest request);
   
private slots:
   void handleNewConnection();
   void handleSocketReady(int id);
   
private:
   QTcpServer* server_;
   QMap<CCSMessages::ClientId, QTcpSocket*> clientSockets_;
   QSignalMapper* socketReadMapper_;
};

#endif
