
#ifndef CCSMESSAGING_H
#define CCSMESSAGING_H

#include <QtCore/QObject>
#include <QtCore/QMap>
class QSignalMapper;
class QTcpServer;
class QTcpSocket;


typedef int ClientId;

class CCSMessaging : public QObject
{
   Q_OBJECT

public:
   explicit CCSMessaging();
   virtual ~CCSMessaging();
   
public slots:
   
signals:
   // void requestReceived(ClientId id, ccs::CCSRequest request);
   
private slots:
   void handleNewConnection();
   void handleSocketReady(int id);
   
private:
   QTcpServer* server_;
   QMap<ClientId, QTcpSocket*> clientSockets_;
   QSignalMapper* socketReadMapper_;
};

#endif
