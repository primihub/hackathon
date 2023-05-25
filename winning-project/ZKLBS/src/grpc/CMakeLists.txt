cmake_minimum_required(VERSION 3.8)

project(ZKLBS)

include(cmake/common.cmake)

get_filename_component(cs_proto "protos/Client2Server.proto" ABSOLUTE)
get_filename_component(cs_proto_path "${cs_proto}" PATH)

# Generated sources
set(cs_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/Client2Server.pb.cc")
set(cs_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/Client2Server.pb.h")
set(cs_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/Client2Server.grpc.pb.cc")
set(cs_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/Client2Server.grpc.pb.h")
add_custom_command(
      OUTPUT "${cs_proto_srcs}" "${cs_proto_hdrs}" "${cs_grpc_srcs}" "${cs_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${cs_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${cs_proto}"
      DEPENDS "${cs_proto}")

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# hw_grpc_proto
add_library(cs_grpc_proto
  ${cs_grpc_srcs}
  ${cs_grpc_hdrs}
  ${cs_proto_srcs}
  ${cs_proto_hdrs})
target_link_libraries(cs_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})

get_filename_component(cv_proto "protos/Client2Verifier.proto" ABSOLUTE)
get_filename_component(cv_proto_path "${cv_proto}" PATH)

# Generated sources
set(cv_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/Client2Verifier.pb.cc")
set(cv_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/Client2Verifier.pb.h")
set(cv_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/Client2Verifier.grpc.pb.cc")
set(cv_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/Client2Verifier.grpc.pb.h")
add_custom_command(
      OUTPUT "${cv_proto_srcs}" "${cv_proto_hdrs}" "${cv_grpc_srcs}" "${cv_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${cv_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${cv_proto}"
      DEPENDS "${cv_proto}")

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# hw_grpc_proto
add_library(cv_grpc_proto
  ${cv_grpc_srcs}
  ${cv_grpc_hdrs}
  ${cv_proto_srcs}
  ${cv_proto_hdrs})
target_link_libraries(cv_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})

get_filename_component(sv_proto "protos/Server2Verifier.proto" ABSOLUTE)
get_filename_component(sv_proto_path "${sv_proto}" PATH)

# Generated sources
set(sv_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/Server2Verifier.pb.cc")
set(sv_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/Server2Verifier.pb.h")
set(sv_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/Server2Verifier.grpc.pb.cc")
set(sv_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/Server2Verifier.grpc.pb.h")
add_custom_command(
      OUTPUT "${sv_proto_srcs}" "${sv_proto_hdrs}" "${sv_grpc_srcs}" "${sv_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${sv_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${sv_proto}"
      DEPENDS "${sv_proto}")

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# hw_grpc_proto
add_library(sv_grpc_proto
  ${sv_grpc_srcs}
  ${sv_grpc_hdrs}
  ${sv_proto_srcs}
  ${sv_proto_hdrs})
target_link_libraries(sv_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})

add_subdirectory(../circuit ../../.build/circuit EXCLUDE_FROM_ALL)
add_subdirectory(../utils ../../.build/utils EXCLUDE_FROM_ALL)
  
# Targets greeter_[async_](client|server)
foreach(_target
greeterServer greeterVerifier)
add_executable(${_target} "${_target}.cpp")
target_link_libraries(${_target}
  PoseidonClient
  PoseidonServer
  CoordinateUtil
  TimeUtil
  TypeUtil
  RandomUtil
  cs_grpc_proto
  cv_grpc_proto
  sv_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})
endforeach()

# Targets greeter_[async_](client|server)
foreach(_target
greeterClient)
add_library(${_target} "${_target}.cpp")
target_link_libraries(${_target}
  PoseidonClient
  PoseidonServer
  CoordinateUtil
  TimeUtil
  TypeUtil
  RandomUtil
  cs_grpc_proto
  cv_grpc_proto
  sv_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})
endforeach()