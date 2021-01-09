cmake_minimum_required(VERSION 3.10)
project(proto_test)
# set(CMAKE_BUILD_TYPE Release)
set(CMAKE_BUILD_TYPE Debug)

add_custom_command(OUTPUT query.pb.cc query.pb.h
  COMMAND protoc
  ARGS --cpp_out . ${CMAKE_CURRENT_SOURCE_DIR}/query.proto -I${CMAKE_CURRENT_SOURCE_DIR}
  DEPENDS query.proto
  VERBATIM )

add_custom_command(OUTPUT test.pb.cc test.pb.h
  COMMAND protoc
  ARGS --cpp_out . ${CMAKE_CURRENT_SOURCE_DIR}/test.proto -I${CMAKE_CURRENT_SOURCE_DIR}
  DEPENDS test.proto
  VERBATIM )

add_executable(main  main.cc test.pb.cc)
target_link_libraries(main protobuf pthread)


add_library(pb_codec codec.cc)
target_link_libraries(pb_codec protobuf  muduo_net z )


add_executable(server server.cc query.pb.cc)
# target_link_libraries(server muduo_base muduo_net z pthread)
target_link_libraries(server pb_codec muduo_base z pthread)


add_executable(client client.cc query.pb.cc)
target_link_libraries(client pb_codec muduo_base z pthread)


