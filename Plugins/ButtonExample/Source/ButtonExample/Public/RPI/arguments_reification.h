

#include "binary_stream.h"

namespace rpi_service {
namespace reification {

/*template<typename T>
struct Value {
void ReadArgument(binary_stream::BinaryStream &stream) {
value_ = stream.readValue<T>();
}
T GetArgument() {
return value_;
}
T value_;
};

template<typename T>
struct Array {
void ReadArgument(binary_stream::BinaryStream &stream) {
array_ = stream.readArray<T>();
}
std::vector<T> GetArgument() {
return array_;
}
std::vector<T> array_;
};*/

template<typename... Types>
class ArgumentList {};

template<typename Head, typename... Tail>
class ArgumentList<Head, Tail...> {
public:
  ArgumentList() {}
  ArgumentList(const Head& head, const Tail&... tail) : head_(head), tail_(tail...) {}
  void Read(binary_stream::BinaryStream& stream) {
    head_ = stream.Read<Head>();
    tail_.Read(stream);
  }
  Head GetHead() { return head_; }
  ArgumentList<Tail...> GetTail() { return tail_; }
private:
  Head head_;
  ArgumentList<Tail...> tail_;
};

template<>
class ArgumentList<> {
public:
  void Read(binary_stream::BinaryStream&) {}
};

template<std::size_t N>
struct ArgumentGet {
  template<typename Head, typename... Tail>
  static auto Apply(ArgumentList<Head, Tail...> t) {
    return ArgumentGet<N - 1>::Apply(t.GetTail());
  }
};

template<>
struct ArgumentGet<0> {
  template<typename Head, typename... Tail>
  static auto Apply(ArgumentList<Head, Tail...> t) {
    return t.GetHead();
  }
};

} // namespace reification
} // namespace rpi_service

