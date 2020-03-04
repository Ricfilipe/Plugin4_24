#include "RPI/rpi_service.h"


#include "primitive.h"
#include <iostream>

DEFINE_LOG_CATEGORY(LogThread);


namespace rpi_service {

RpiService::RpiService() {
	ADD_FUNCTION(Primitive::Sphere);
    //ADD_FUNCTION(Primitive::Box);

	UE_LOG(LogThread, Log, TEXT("Created Thread"));
}

void RpiService::InsertMap(std::string name, std::unique_ptr<reification::HandlerInterface> handler) {
  handlers_map_.insert(std::make_pair(name, std::move(handler)));
}

void rpi_service::RpiService::Run(unsigned short port) {
  try {
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::acceptor acceptor(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port));
    binary_stream::BinaryStream socket_stream(io_service, binary_stream::Endianess::kLittle);
    UE_LOG(LogThread, Warning, TEXT("Waiting for connection"));
    acceptor.accept(socket_stream.GetSocket());
	UE_LOG(LogThread, Warning, TEXT("Connected to Something"));
    boost::system::error_code error;
    for (;;) {
      int id = socket_stream.Read<int>();
      if (socket_stream.GetReadLength() == 0) break;
      if (id != 0) {
        int method_index = id - 1;
        available_handlers_[method_index]->SetArgs(socket_stream);
        available_handlers_[method_index]->Invoke();

        available_handlers_[method_index]->WriteReturn(socket_stream);
      } else {
        std::string operation_str = socket_stream.Read<std::string>();
        available_handlers_.push_back(std::move(handlers_map_.at(operation_str)));
        int return_value = (int)available_handlers_.size();
        socket_stream.Write(return_value);
      }
    }
  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
}

} // namespace rpi_service