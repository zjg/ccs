#ifndef _THRIFT_TASYNC_QTCP_SERVER_H_
#define _THRIFT_TASYNC_QTCP_SERVER_H_

#include <QObject>
#include <QWeakPointer>
#include <QSharedPointer>
#include <QTcpServer>

#include <boost/shared_ptr.hpp>

#include <tr1/functional>

// compatability if Qt is under a custom namespace
QT_FORWARD_DECLARE_CLASS(QObject)

namespace apache { namespace thrift { namespace protocol {
class TProtocol;
class TProtocolFactory;
}}} // apache::thrift::protocol

namespace apache { namespace thrift { namespace async {

class TAsyncProcessor;

class TQTcpServer : public QObject {
 Q_OBJECT
 public:
  /**
   * Create an abstract server from a QIODevice.
   */
  TQTcpServer(QWeakPointer<QTcpServer> server, boost::shared_ptr<TAsyncProcessor> processor, boost::shared_ptr<apache::thrift::protocol::TProtocolFactory> protocolFactory, QT_PREPEND_NAMESPACE(QObject) *parent = NULL);

  ~TQTcpServer();

 private Q_SLOTS:
  void processIncoming();
  void beginDecode();

 private:
  class RequestContext;

  static void finish(RequestContext *ctx, boost::shared_ptr<apache::thrift::protocol::TProtocol> oprot, bool healthy);

  QWeakPointer<QTcpServer> server_;
  boost::shared_ptr<TAsyncProcessor> processor_;
  boost::shared_ptr<apache::thrift::protocol::TProtocolFactory> pfact_;
};

}}} // apache::thrift::async

#endif // #ifndef _THRIFT_TASYNC_QTCP_SERVER_H_
