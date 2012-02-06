
#ifndef CCSERVER_H
#define CCSERVER_H

class QTcpServer;

#include "CCS.h"

namespace apache { namespace thrift { namespace protocol {
class TProtocolFactory;
}}} // apache::thrift::protocol

namespace apache { namespace thrift { namespace async {
class TQTcpServer;
}}} // apache::thrift::async

class I_CodeCompletionService;

class CCServer
{
public:
   explicit CCServer(I_CodeCompletionService& completionService);
   virtual ~CCServer();
   
private:
   class CCSCobHandler : public ccs::CCSCobSvIf
   {
   public:
      CCServer* server_;
      
      virtual void ping(std::tr1::function<void()> cob);
      virtual void codeCompletion(
         std::tr1::function<void(ccs::CodeCompletionResponse const& _return)> cob,
         const ccs::CodeCompletionRequest& request);
   };
   
private:
   I_CodeCompletionService& completionService_;
   
   boost::shared_ptr<CCSCobHandler> asyncHandler_;
   
   boost::shared_ptr<ccs::CCSAsyncProcessor> processor_;
   boost::shared_ptr<apache::thrift::protocol::TProtocolFactory> protocolFactory_;
   
   boost::shared_ptr<QTcpServer> serverSocket_;
   boost::shared_ptr<apache::thrift::async::TQTcpServer> thriftServer_;
};

#endif
