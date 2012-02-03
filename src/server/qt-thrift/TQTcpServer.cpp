#include "TQTcpServer.h"

#include <stdint.h>

#include <protocol/TProtocol.h>
#include <async/TAsyncProcessor.h>

#include <QTcpSocket>

#include "TQIODeviceTransport.h"

#include <iostream>

using boost::shared_ptr;
using apache::thrift::protocol::TProtocol;
using apache::thrift::protocol::TProtocolFactory;
using apache::thrift::transport::TTransport;
using apache::thrift::transport::TTransportException;
using apache::thrift::transport::TQIODeviceTransport;
using std::tr1::function;
using std::tr1::bind;

QT_USE_NAMESPACE

namespace apache { namespace thrift { namespace async {

struct TQTcpServer::RequestContext {
  shared_ptr<TTransport> transport_;

  explicit RequestContext(shared_ptr<TTransport> transport)
    : transport_(transport)
  {}
};

TQTcpServer::TQTcpServer(QWeakPointer<QTcpServer> server, shared_ptr<TAsyncProcessor> processor, shared_ptr<TProtocolFactory> pfact, QObject *parent)
  : QObject(parent)
  , server_(server)
  , processor_(processor)
  , pfact_(pfact)
{
 connect(server.data(), SIGNAL(newConnection()), SLOT(processIncoming()));
}

TQTcpServer::~TQTcpServer()
{
}

void TQTcpServer::processIncoming()
{
  QTcpServer *server = server_.data();
  Q_ASSERT(server);

  do {
    QTcpSocket *connection = server_.data()->nextPendingConnection();

    if (!connection) {
      Q_ASSERT(false);
      // invalid state - this slot is only called when the TcpServer newConnection signal is emitted
      return;
    }

    connect(connection, SIGNAL(readyRead()), SLOT(beginDecode()));
  } while (server->hasPendingConnections());
}

void TQTcpServer::beginDecode()
{
  QTcpSocket *connection = static_cast<QTcpSocket *>(sender());
  Q_ASSERT(connection);

  RequestContext *ctx;
  shared_ptr<TTransport> transport;
  shared_ptr<TProtocol> iprot;
  shared_ptr<TProtocol> oprot;

  try {
    transport = shared_ptr<TTransport>(new TQIODeviceTransport(connection));

    iprot = shared_ptr<TProtocol>(pfact_->getProtocol(transport));
    oprot = shared_ptr<TProtocol>(pfact_->getProtocol(transport));
  } catch(...) {
    std::cerr << "Failed to initialize transports/protocols" << "\n";
  }

  ctx = new RequestContext(transport);

  try {
    processor_->process(
      bind(
          &TQTcpServer::finish,
          ctx,
          oprot,
          std::tr1::placeholders::_1),
      iprot, oprot);
  } catch(const TTransportException& ex) {
    std::cerr << "transport exception during processing: '" << ex.what() << "'" << "\n";
    delete ctx;
  } catch(...) {
    std::cerr << "Failed to process" << "\n";
    delete ctx;
  }

}

void TQTcpServer::finish(RequestContext *ctx, shared_ptr<TProtocol> oprot, bool healthy)
{
  Q_UNUSED(oprot);
  Q_ASSERT(healthy); // what to do if not healthy?

  delete ctx;
}

}}} // apache::thrift::async
