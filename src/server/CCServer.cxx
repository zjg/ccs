
#include <QtDebug>
#include <QTcpServer>

#include "TQTcpServer.h"

#include "CCServer.h"
#include "I_CodeCompletionService.h"

#include <protocol/TBinaryProtocol.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::async;

using boost::shared_ptr;

CCServer::CCServer(I_CodeCompletionService& completionService)
   : completionService_(completionService)
{
   serverSocket_ = shared_ptr<QTcpServer>(new QTcpServer());
   serverSocket_->listen(QHostAddress::Any, 9515);
   qDebug("server listening on %u", serverSocket_->serverPort());
   
   asyncHandler_ =
      shared_ptr<CCSCobHandler>(new CCSCobHandler());
   asyncHandler_->server_ = this;
   
   processor_ =
      shared_ptr<ccs::CCSAsyncProcessor>(
         new ccs::CCSAsyncProcessor(asyncHandler_));
   
   protocolFactory_ =
      shared_ptr<TProtocolFactory>(new TBinaryProtocolFactory());
   
   thriftServer_ =
      shared_ptr<TQTcpServer>(
         new TQTcpServer(serverSocket_, processor_, protocolFactory_));
}

CCServer::~CCServer()
{
}

void CCServer::CCSCobHandler::ping(std::tr1::function<void()> cob)
{
   qDebug("ping!");
   cob();
}

void CCServer::CCSCobHandler::codeCompletion(
   std::tr1::function<void(ccs::CodeCompletionResponse const& _return)> cob,
   const ccs::CodeCompletionRequest& request)
{
   ccs::CodeCompletionResponse response =
      server_->completionService_.process(request);
   cob(response);
}


