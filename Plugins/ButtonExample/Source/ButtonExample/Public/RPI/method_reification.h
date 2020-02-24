#ifndef METHOD_REIFICATION_H_
#define METHOD_REIFICATION_H_

#include <tuple>

#include "RPI/arguments_reification.h"

namespace rpi_service {
namespace reification {

template <typename F, typename... Args, std::size_t... Is>
auto WrapperDetail(F&& f, ArgumentList<Args...> list, std::index_sequence<Is...>) {
  return f(ArgumentGet<Is>::Apply(list)...);
}

template <typename F, typename... Args>
auto WrapperInvoke(F&& f, ArgumentList<Args...> list) {
  return WrapperDetail(f, list, std::index_sequence_for<Args...>());
}

template <typename F>
auto WrapperInvoke(F&& f, ArgumentList<>) {
  return f();
}

class HandlerInterface {
public:
  virtual void SetArgs(binary_stream::BinaryStream &stream) = 0;
  virtual void Invoke() = 0;
  virtual void WriteReturn(binary_stream::BinaryStream &stream) = 0;
  virtual bool IsVoid() = 0;
};

template<typename F>
class Handler : public HandlerInterface {};

template<typename R, typename... Args>
class Handler<R(*)(Args...)> : public HandlerInterface {
public:
  using F = R(*)(Args...);
  Handler(F f) : function_ptr_(f) {}
  void SetArgs(binary_stream::BinaryStream &stream) {
    function_arguments_.Read(stream);
  }
  void Invoke() {
    return_ = WrapperInvoke(function_ptr_, function_arguments_);
  }
  void WriteReturn(binary_stream::BinaryStream &stream) {
    stream.Write<R>(return_);
  }
  bool IsVoid() {
    return false;
  }
private:
  F function_ptr_;
  ArgumentList<Args...> function_arguments_;
  R return_;
};

template<typename... Args>
class Handler<void(*)(Args...)> : public HandlerInterface {
public:
  using F = void(*)(Args...);
  Handler(F f) : function_ptr_(f) {}
  void SetArgs(binary_stream::BinaryStream &stream) {
    function_arguments_.Read(stream);
  }
  void Invoke() {
    WrapperInvoke(function_ptr_, function_arguments_);
  }
  void WriteReturn(binary_stream::BinaryStream &stream) {
    stream.VoidReadWrite();
  }
  bool IsVoid() {
    return true;
  }
private:
  F function_ptr_;
  ArgumentList<Args...> function_arguments_;
};

template<typename F>
std::unique_ptr<Handler<F>> MakeHandler(F&& f) {
  return std::make_unique<Handler<F>>(f);
}

} // namespace reification
} // namespace rpi_service

#endif  // METHOD_REIFICATION_H_