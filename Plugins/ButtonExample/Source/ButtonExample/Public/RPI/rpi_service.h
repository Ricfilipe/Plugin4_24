#ifndef RPI_SERVICE_H_
#define RPI_SERVICE_H_

#include <vector>
#include <map>
#include <memory>

#include "RPI/method_reification.h"

DECLARE_LOG_CATEGORY_EXTERN(LogThread, Log, All);

#define ADD_FUNCTION(function) \
  InsertMap(#function, std::move(reification::MakeHandler(&##function##)));\

namespace rpi_service {

class RpiService {
public:
  RpiService();
  void Run(unsigned short port);
  void InsertMap(std::string, std::unique_ptr <reification::HandlerInterface > );
private:
    binary_stream::BinaryStream* socket_stream;
  boost::asio::io_service io_service;
  boost::asio::ip::tcp::acceptor* acceptor;
  std::map<std::string, std::unique_ptr<reification::HandlerInterface>> handlers_map_;
  std::vector<std::unique_ptr<reification::HandlerInterface>> available_handlers_;
};

} // namespace rpi_service

#endif  // RPI_SERVICE_H_