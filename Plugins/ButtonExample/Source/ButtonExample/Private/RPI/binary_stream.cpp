

#include "binary_stream.h"

namespace binary_stream {

Endianess GetSystemEndianess() {
  union {
    uint32_t integer;
    uint8_t bytes[sizeof(uint32_t)];
  } check;
  check.integer = 0x01020304U;
  return (check.bytes[0] == 0x01 ? Endianess::kBig : Endianess::kLittle);
}

BinaryStream::BinaryStream(boost::asio::io_service& io_service, Endianess ord) : socket_(io_service) {
  order_ = ord;
  system_order_ = GetSystemEndianess();
}

void BinaryStream::VoidReadWrite() {

}

boost::asio::ip::tcp::socket& BinaryStream::GetSocket() {
  return socket_;
}

size_t BinaryStream::GetReadLength() {
  return read_length_;
}

} // namespace binary_stream