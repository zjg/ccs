
#include <QtCore/QSignalMapper>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

#include <ccs.pb.h>

#include <CCSMessaging.h>

namespace
{
   CCSMessages::ClientId nextId = 1;
}

CCSMessaging::CCSMessaging()
   : server_(NULL)
   , socketReadMapper_(NULL)
{
   server_ = new QTcpServer(this);
   server_->listen();
   qDebug("server listening on %u", server_->serverPort());
   
   connect(server_, SIGNAL(newConnection()),
           this, SLOT(handleNewConnection()));
   
   socketReadMapper_ = new QSignalMapper(this);
   connect(socketReadMapper_, SIGNAL(mapped(int)),
           this, SLOT(handleSocketReady(int)));
}

CCSMessaging::~CCSMessaging()
{
}

void CCSMessaging::handleNewConnection()
{
   CCSMessages::ClientId id = nextId++;
   QTcpSocket* socket = server_->nextPendingConnection();
   
   clientSockets_[id] = socket;
   socketReadMapper_->setMapping(socket, id);
   connect(socket, SIGNAL(readyRead()),
           socketReadMapper_, SLOT(map()));
   qDebug("received connection from %s:%u",
          qPrintable(socket->peerAddress().toString()),
          socket->peerPort());
}

void CCSMessaging::handleSocketReady(int id)
{
   QTcpSocket* socket = clientSockets_[id];
   QByteArray data = socket->readAll();
   
   ccs::CCSRequest request;
   bool parseOkay = request.ParseFromArray(data.constData(),
                                           data.size());
   if (parseOkay)
   {
      qDebug("got request: %s %d %d",
             request.completionrequest().filename().c_str(),
             request.completionrequest().line(),
             request.completionrequest().column());
      
      CCSMessages::CodeCompletionRequest completionRequest;
      completionRequest.client = id;
      completionRequest.filename = request.completionrequest().filename().c_str();
      completionRequest.line = request.completionrequest().line();
      completionRequest.column = request.completionrequest().column();
      
      emit requestReceived(completionRequest);
   }
}
