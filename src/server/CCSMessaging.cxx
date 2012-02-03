
#include <QSignalMapper>
#include <QTcpServer>
#include <QTcpSocket>

#include "ccs.pb.h"

#include "CCSMessaging.h"

namespace
{
   CCSMessages::ClientId nextId = 1;
}

CCSMessaging::CCSMessaging()
   : server_(NULL)
   , socketReadMapper_(NULL)
{
   server_ = new QTcpServer(this);
   server_->listen(QHostAddress::Any, 9515);
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
   QAbstractSocket* socket = clientSockets_[id];
   QByteArray data = socket->readAll();
   
   ccs::CCSRequest request;
   bool parseOkay = request.ParseFromArray(data.constData(),
                                           data.size());
   if (parseOkay)
   {
      if (request.has_completionrequest())
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
}

void CCSMessaging::sendResponse(CCSMessages::CodeCompletionResponse response)
{
   if (!clientSockets_.contains(response.request.client))
   {
      qDebug("Unable to send response to unknown client %d", response.request.client);
      return;
   }
   
   QAbstractSocket* socket = clientSockets_[response.request.client];
   
   ccs::CodeCompletionRequest wireRequest;
   wireRequest.set_filename(qPrintable(response.request.filename));
   wireRequest.set_line(response.request.line);
   wireRequest.set_column(response.request.column);
   
   ccs::CodeCompletionResponse wireCompletionResponse;
   *(wireCompletionResponse.mutable_request()) = wireRequest;
   
   foreach (CCSMessages::CodeCompletionResult result, response.results)
   {
      ccs::CodeCompletionResponse::CompletionResult wireResult;
      wireResult.set_text(qPrintable(result.text));
      
      *(wireCompletionResponse.add_result()) = wireResult;
   }
   
   ccs::CCSResponse wireResponse;
   *(wireResponse.mutable_completionresponse()) = wireCompletionResponse;
   
   QByteArray wireBytes(wireResponse.ByteSize(), 0);
   wireResponse.SerializeToArray(wireBytes.data(), wireBytes.size());
   socket->write(wireBytes);
}

