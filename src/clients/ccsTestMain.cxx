
#include <QCoreApplication>
#include <QTcpSocket>
#include <QtDebug>

#include "CCS.h"

#include <protocol/TBinaryProtocol.h>
#include <qt/TQIODeviceTransport.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::async;

using boost::shared_ptr;

using ccs::CCSClient;

int main(int argc, char* argv[])
{
   QCoreApplication app(argc, argv);
   
   shared_ptr<QTcpSocket> socket(new QTcpSocket());
   shared_ptr<TTransport> transport(new TQIODeviceTransport(socket));
   shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
   shared_ptr<CCSClient> client(new CCSClient(protocol));
   
   socket->connectToHost("localhost", 9515);
   qDebug("connected");
   
   
   client->send_ping();
   qDebug("recving ping");
   client->recv_ping();
   qDebug("pinged!");
   
   return 0;
}
