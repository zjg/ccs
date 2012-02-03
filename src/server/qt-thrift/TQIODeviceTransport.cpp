#include "TQIODeviceTransport.h"

#include <transport/TBufferTransports.h>

#include <QAbstractSocket>

#include <iostream>

namespace apache { namespace thrift { namespace transport {
  using boost::shared_ptr;

  /**
   * Manages the request context for a tcp connection established from a QTcpServer
   */
  TQIODeviceTransport::TQIODeviceTransport(QWeakPointer<QIODevice> dev)
    : dev_(dev)
  {
    //connect(dev_, SIGNAL(readyRead()), SLOT(fillInputBuffer()));
  }

  TQIODeviceTransport::~TQIODeviceTransport()
  {
    if (dev_.data()) {
      close();
    }
  }

  void TQIODeviceTransport::open()
  {
    if (!isOpen()) {
      throw TTransportException(TTransportException::NOT_OPEN, "open() underlying QIODevice isn't open");
    }
  }

  bool TQIODeviceTransport::isOpen()
  {
    QIODevice* dev = dev_.data();
    if (!dev) {
      throw TTransportException(TTransportException::NOT_OPEN, "isOpen() underlying QIODevice is null");
    }

    return dev->isOpen();
  }

  bool TQIODeviceTransport::peek()
  {
    QIODevice* dev = dev_.data();
    if (!dev) {
      throw TTransportException(TTransportException::NOT_OPEN, "peak() underlying QIODevice is null");
    }

    return dev->bytesAvailable() > 0;
  }

  void TQIODeviceTransport::close()
  {
    QIODevice *dev = dev_.data();
    if (!dev) {
      throw TTransportException(TTransportException::NOT_OPEN, "close() underlying QIODevice is null");
    }

    dev->close();
  }

  uint32_t TQIODeviceTransport::readAll(uint8_t* buf, uint32_t len) {
    uint32_t requestLen = len;
    while (len) {
      uint32_t readSize;
      try {
        readSize = read(buf, len);
      } catch (...) {
        if (len != requestLen) {
          // something read already
          return requestLen - len;
	}
	// error but nothing read yet
	throw;
      }
      if (readSize == 0) {
        dev_.data()->waitForReadyRead(50);
      } else {
        buf += readSize;
	len -= readSize;
      }
    }
    return requestLen;
  }

  uint32_t TQIODeviceTransport::read(uint8_t* buf, uint32_t len)
  {
    QIODevice *dev = dev_.data();
    uint32_t actualSize;
    qint64 readSize;

    if (!dev) {
      throw TTransportException(TTransportException::NOT_OPEN, "read() underlying QIODevice is null");
    }

    if (!dev->isOpen()) {
      throw TTransportException(TTransportException::NOT_OPEN, "read() underlying QIODevice is not open");
    }

    actualSize = (uint32_t)std::min((qint64)len, dev->bytesAvailable());
    readSize = dev->read(reinterpret_cast<char *>(buf), len);

    if (readSize < 0) {
      QAbstractSocket *socket;
      if ((socket = qobject_cast<QAbstractSocket *>(dev))) {
        throw TTransportException(TTransportException::UNKNOWN, "Failed to read() from QAbstractSocket", socket->error());
      }
      throw TTransportException(TTransportException::UNKNOWN, "Failed to read from from QIODevice");
    }

    return (uint32_t)readSize;
  }

  void TQIODeviceTransport::write(const uint8_t* buf, uint32_t len)
  {
    while (len) {
      uint32_t written = write_partial(buf, len);
      len -= written;
      dev_.data()->waitForBytesWritten(50);
    }
  }

  uint32_t TQIODeviceTransport::write_partial(const uint8_t* buf, uint32_t len)
  {
    QIODevice *dev = dev_.data();
    qint64 written;

    if (!dev) {
      throw TTransportException(TTransportException::NOT_OPEN, "write_partial() underlying QIODevice is null");
    }

    if (!dev->isOpen()) {
      throw TTransportException(TTransportException::NOT_OPEN, "write_partial() underlying QIODevice is not open");
    }

    written = dev->write(reinterpret_cast<const char *>(buf), len);
    if (written < 0) {
      QAbstractSocket *socket;
      if ((socket = qobject_cast<QAbstractSocket *>(dev))) {
        throw TTransportException(TTransportException::UNKNOWN, "write_partial() failed to write to QAbstractSocket", socket->error());
      }

      throw TTransportException(TTransportException::UNKNOWN, "write_partial() failed to write to underlying QIODevice");
    }

    return (uint32_t)written;
  }

  void TQIODeviceTransport::flush()
  {
    QIODevice *dev = dev_.data();

    if (!dev) {
      throw TTransportException(TTransportException::NOT_OPEN, "flush() underlying QIODevice is null");
    }

    if (!dev->isOpen()) {
      throw TTransportException(TTransportException::NOT_OPEN, "flush() underlying QIODevice is not open");
    }

    QAbstractSocket *socket;

    if ((socket = qobject_cast<QAbstractSocket *>(dev))) {
      socket->flush();
    } else {
      dev->waitForBytesWritten(1);
    }
  }

  void TQIODeviceTransport::finish(boost::shared_ptr<apache::thrift::protocol::TProtocol> oprot, bool healthy)
  {
    Q_UNUSED(oprot);
    Q_UNUSED(healthy);

    flush();

    std::cerr << "finished" << "\n";
  }

  uint8_t* TQIODeviceTransport::borrow(uint8_t* buf, uint32_t* len)
  {
    return NULL;
  }

  void TQIODeviceTransport::consume(uint32_t len)
  {
    throw TTransportException(TTransportException::UNKNOWN);
  }
}}} // apache::thrift::transport

