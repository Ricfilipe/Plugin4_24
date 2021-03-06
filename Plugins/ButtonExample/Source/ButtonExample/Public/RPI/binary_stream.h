#ifndef BINARY_STREAM_H_
#define BINARY_STREAM_H_

THIRD_PARTY_INCLUDES_START
#include <boost/asio.hpp>
THIRD_PARTY_INCLUDES_END

namespace binary_stream {

template<typename T>
T SwapBytes(T v) {
  uint8_t* buff;
  buff = reinterpret_cast<uint8_t*>(&v);
  uint8_t temp[sizeof(T)];
  for (uint16_t i = 0; i < sizeof(T); i++)
    temp[i] = buff[sizeof(T) - i - 1];
  return *((T*)temp);
}

template<typename T>
static inline T * OffsetWritePointer(void * ptr, size_t offs) {
  uintptr_t p = reinterpret_cast<uintptr_t>(ptr);
  p += offs;
  return reinterpret_cast<T *>(p);
}

enum Endianess {
  kBig, kLittle
};

Endianess GetSystemEndianess();

class BinaryStream {
public:
    boost::asio::ip::tcp::socket socket_;
  BinaryStream(boost::asio::io_service&, Endianess);

  void VoidReadWrite();

  boost::asio::ip::tcp::socket& GetSocket();
  size_t GetReadLength();

  template<typename T>
  struct Type {};

  template<typename T>
  T Read(Type<T>) {
    T r;
    unsigned char buf[sizeof(T)];
    read_length_ = boost::asio::read(socket_, boost::asio::buffer(buf, sizeof(T)), boost::asio::transfer_all(), error_);
    memcpy(&r, buf, sizeof(T));

    //May swap strings, careful!!
    if (order_ != system_order_) {
      return SwapBytes(r);
    } else {
      return r;
    }
  }

  template<typename T>
  std::vector<T> Read(Type<std::vector<T>>) {
    int32_t len = Read<int32_t>();
    std::vector<T> r;
    for (int32_t i = 0; i < len; i++) {
      r.push_back(Read<T>());
    }

    return r;
  }

  template<>
  std::string Read(Type<std::string>) {
      int32_t len = (int32_t)Read<char>();
    std::vector<char> r;
    for (int32_t i = 0; i < len; i++) {
      r.push_back(Read<char>());
    }
    std::string str(r.begin(), r.end());
    return str;
  }

  template<typename T>
  T Read() {
    return Read(Type<T>{});
  }

  template<typename T>
  void Write(T const& value) {
    unsigned char buf[sizeof(T)];
    memcpy(OffsetWritePointer<T>(buf, 0), &value, sizeof(T));
    boost::asio::write(socket_, boost::asio::buffer(buf, sizeof(T)),
                       boost::asio::transfer_all(), error_);
  }

  template<>
  void Write(std::string const& value) {
    std::vector<char> data(value.begin(), value.end());
    uint_least8_t  size = (uint_least8_t)data.size();
    Write<uint_least8_t>(size);

    for (uint32_t i = 0; i < data.size(); i++) {
      Write(data[i]);
    }
  }

  template<>
  void Write(std::vector<int> const& value) {
    int32_t len = (int32_t)value.size();
    Write<int32_t>(len);
    for (uint32_t i = 0; i < value.size(); i++) {
      Write<int>(value[i]);
    }
  }

  template<>
  void Write(std::vector<double> const& value) {
    int32_t len = (int32_t)value.size();
    Write<int32_t>(len);
    for (uint32_t i = 0; i < value.size(); i++) {
      Write<double>(value[i]);
    }
  }

  template<>
  void Write(std::vector<short> const& value) {
    int32_t len = (int32_t)value.size();
    Write<int32_t>(len);
    for (uint32_t i = 0; i < value.size(); i++) {
      Write<short>(value[i]);
    }
  }

  //Unreal

  template<>
  FVector Read(Type<FVector>) {
      return FVector(Read<float>(), Read<float>(), Read<float>());
       
  }

  template<typename T>
  TArray<T> Read(Type<TArray<T>>) {
      int32_t len = Read<int32_t>();
      TArray<T> r;
      for (int32_t i = 0; i < len; i++) {
          r.Add(Read<T>());
      }

      return r;
  }

private:
 
  boost::system::error_code error_;

  Endianess order_;
  Endianess system_order_;
  size_t read_length_;
};

} // namespace binary_stream

#endif  // BINARY_STREAM_H_