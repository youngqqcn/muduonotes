cmake_minimum_required(VERSION 3.10)

project(proto_test)


add_custom_command(OUTPUT test.pb.cc test.pb.h
  COMMAND protoc
  ARGS --cpp_out . ${CMAKE_CURRENT_SOURCE_DIR}/test.proto -I${CMAKE_CURRENT_SOURCE_DIR}
  DEPENDS test.proto
  VERBATIM )

add_executable(main  main.cc test.pb.cc)
target_link_libraries(main protobuf pthread)
