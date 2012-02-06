#include "TQTcpServer.h"

#include <stdint.h>

#include <protocol/TProtocol.h>
#include <async/TAsyncProcessor.h>

#include <QSharedPointer>
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

struct TQTcpServer::ConnectionContext {
  shared_ptr<TTransport> transport_;
  shared_ptr<QTcpSocket> connection_;
  shared_ptr<TProtocol> iprot_;
  shared_ptr<TProtocol> oprot_;

  explicit ConnectionContext(shared_ptr<TTransport> transport,
                          shared_ptr<QTcpSocket> connection,
                          shared_ptr<TProtocol> iprot,
                          shared_ptr<TProtocol> oprot)
    : transport_(transport)
    , connection_(connection)
    , iprot_(iprot)
    , oprot_(oprot)
  {}
  
  virtual ~ConnectionContext()
  {
    qDebug("deleting ConnectionContext");
  }
};

TQTcpServer::TQTcpServer(shared_ptr<QTcpServer> server, shared_ptr<TAsyncProcessor> processor, shared_ptr<TProtocolFactory> pfact, QObject *parent)
  : QObject(parent)
  , server_(server)
  , processor_(processor)
  , pfact_(pfact)
{
 connect(server.get(), SIGNAL(newConnection()), SLOT(processIncoming()));
}

TQTcpServer::~TQTcpServer()
{
}

void TQTcpServer::processIncoming()
{
  // todo - create the ConnectionContext here, and hook into more signals
  // coming from the QTcpSocket (error, closed, etc)
  // (currently if a connection is opened but no data is sent, then it will
  // sit around in memory forever)
  
  do {
    QTcpSocket *connection = server_->nextPendingConnection();

    if (!connection) {
      Q_ASSERT(false);
      // invalid state - this slot is only called when the TcpServer newConnection signal is emitted
      return;
    }

    qDebug("TQTcpServer - got connection");
    
    connect(connection, SIGNAL(readyRead()), SLOT(beginDecode()));
  } while (server_->hasPendingConnections());
}

void TQTcpServer::beginDecode()
{
  // take ownership of the QTcpSocket; technically it could be deleted
  // when the QTcpServer is destroyed, but any real app should delete this
  // class before deleting the QTcpServer that we are using
  shared_ptr<QTcpSocket> connection(qobject_cast<QTcpSocket*>(sender()));
  Q_ASSERT(connection.get());

  if (ctxMap_.find(connection) == ctxMap_.end())
  {
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
    
    ctxMap_[connection] =
      shared_ptr<ConnectionContext>(new ConnectionContext(transport, connection, iprot, oprot));
  }
  
  shared_ptr<ConnectionContext> ctx = ctxMap_[connection];
  
  try {
    qDebug("TQTcpServer - processing data");
    processor_->process(
      bind(&TQTcpServer::finish, this,
           ctx, std::tr1::placeholders::_1),
      ctx->iprot_, ctx->oprot_);
  } catch(const TTransportException& ex) {
    std::cerr << "transport exception during processing: '" << ex.what() << "'" << "\n";
    ctxMap_.erase(connection);
  } catch(...) {
    std::cerr << "Failed to process" << "\n";
    ctxMap_.erase(connection);
  }
}

void TQTcpServer::finish(shared_ptr<ConnectionContext> ctx, bool healthy)
{
  qDebug("TQTcpServer - finish");
  
  shared_ptr<QTcpSocket> connection = ctx->connection_;
  
  if (!healthy)
  {
    qDebug("TQTcpServer - processor returned false");
    ctxMap_.erase(connection);
    return;
  }
  
  if (connection->state() != QAbstractSocket::ConnectedState)
  {
    qDebug("TQTcpServer - socket not connected");
    ctxMap_.erase(connection);
    return;
  }
  
  // anything else to check for ?
}

}}} // apache::thrift::async
