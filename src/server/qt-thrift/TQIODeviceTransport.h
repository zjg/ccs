#ifndef _THRIFT_ASYNC_TQIODEVICE_TRANSPORT_H_
#define _THRIFT_ASYNC_TQIODEVICE_TRANSPORT_H_ 1

#include <stdint.h>

#include <QIODevice>

#include <boost/shared_ptr.hpp>

#include <transport/TVirtualTransport.h>

namespace apache { namespace thrift { namespace protocol {
class TProtocol;
}}} // apache::thrift::protocol

/** in it's own file because moc requires each class definition (for a proper QObject) to
 * be in it's own file.
 */
namespace apache { namespace thrift { namespace transport {
  using apache::thrift::transport::TVirtualTransport;
  using boost::shared_ptr;

  /**
   * Manages the request context for a tcp connection established from a QTcpServer
   */
  class TQIODeviceTransport : public TVirtualTransport<TQIODeviceTransport> {
    public:
      /**
       * Once this class is done with the device, it'll call deleteLater on it.
       */
      explicit TQIODeviceTransport(boost::shared_ptr<QIODevice> dev);
      ~TQIODeviceTransport();

      void open();

      /**
       * Whether the QIODevice is alive.
       *
       * @return Is the device alive?
       */
      bool isOpen();
    
      /**
       * Returns whether or not there is data waiting to be read for the
       * QIODevice
       */
      bool peek();
    
      /**
       * Shuts down communications on the QIODevice.
       * This also calls deleteLater on this.
       */
      void close();
    
      /**
       * suboptimal because it blocks until the entire
       * data stream has been read (or an error occurs)
       */
      uint32_t readAll(uint8_t *buf, uint32_t len);

      /**
       * Reads from the underlying socket.
       */
      uint32_t read(uint8_t* buf, uint32_t len);

      /**
       * Writes to the underlying socket.  Loops until done or fail.
       */
      void write(const uint8_t* buf, uint32_t len);

      /**
       * Writes to the underlying QIODevice.  Does single send() and returns result.
       */
      uint32_t write_partial(const uint8_t* buf, uint32_t len);

      void flush();

      uint8_t* borrow(uint8_t* buf, uint32_t* len);
      void consume(uint32_t len);

    private:
      boost::shared_ptr<QIODevice> dev_;
  };
}}} // apache::thrift::transport

#endif // #ifndef _THRIFT_ASYNC_TQIODEVICE_TRANSPORT_H_

