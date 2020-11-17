#include "RPI/rpi_service.h"


#include "primitive.h"
#include <iostream>

DEFINE_LOG_CATEGORY(LogThread);



namespace rpi_service {

RpiService::RpiService() {
    
    ADD_FUNCTION(Primitive::Box);
    ADD_FUNCTION(Primitive::RightCuboid);
    ADD_FUNCTION(Primitive::Cylinder);
    ADD_FUNCTION(Primitive::Pyramid);
    ADD_FUNCTION(Primitive::PyramidFrustum);
    ADD_FUNCTION(Primitive::DeleteAll);
    ADD_FUNCTION(Primitive::Slab);
    ADD_FUNCTION(Primitive::CurrentParent);
    ADD_FUNCTION(Primitive::SetCurrentParent);
    ADD_FUNCTION(Primitive::LoadMaterial);
    ADD_FUNCTION(Primitive::CurrentMaterial);
    ADD_FUNCTION(Primitive::SetCurrentMaterial);
    ADD_FUNCTION(Primitive::LoadResource);
    ADD_FUNCTION(Primitive::CreateBlockInstance);
    ADD_FUNCTION(Primitive::Panel);
    ADD_FUNCTION(Primitive::BeamRectSection);
    ADD_FUNCTION(Primitive::BeamCircSection);
    ADD_FUNCTION(Primitive::InstantiateBIMElement);
    ADD_FUNCTION(Primitive::Subtract);
    ADD_FUNCTION(Primitive::Unite);
    ADD_FUNCTION(Primitive::DeleteMany);
    ADD_FUNCTION(Primitive::PointLight);
    ADD_FUNCTION(Primitive::SetView);
    ADD_FUNCTION(Primitive::ViewCamera);
    ADD_FUNCTION(Primitive::ViewLens);
    ADD_FUNCTION(Primitive::ViewTarget);
    ADD_FUNCTION(Primitive::RenderView);
    ADD_FUNCTION(Primitive::Spotlight);
    ADD_FUNCTION(Primitive::PyramidFrustumWithMaterial);
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
        int32_t id = socket_stream.Read<int32_t>();
      if (socket_stream.GetReadLength() == 0) break;
      if (id != 0) {

       
        int method_index = id - 1;
        available_handlers_[method_index]->SetArgs(socket_stream);


        int max_bytes = socket_stream.GetSocket().available();
        std::string s;
        for (int i = 0; i < max_bytes; i++) {
            s.push_back(socket_stream.Read<char>());
        }
        FString f = FString(s.c_str());
        UE_LOG(LogThread, Warning, TEXT("%s"), *f);


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
  UE_LOG(LogThread, Log, TEXT("Connection Ended"));
}

} // namespace rpi_service