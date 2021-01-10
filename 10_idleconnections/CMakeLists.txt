cmake_minimum_required(VERSION 3.10)

set(CMAKE_BUILD_TYPE Debug)

project(idleconnections)


add_executable(client client.cc)
target_link_libraries(client muduo_net muduo_base pthread rt )


# add_executable(echo_client echo_client.cc)
# target_link_libraries(echo_client muduo_net muduo_base pthread rt )

# add_executable(echo_client echo_hack_client.cc)
# target_link_libraries(echo_client muduo_net muduo_base pthread rt )

add_executable(echo_server echo_server.cc)
target_link_libraries(echo_server muduo_net muduo_base z pthread )

# add_executable(cirbuf circular_buf.cc)
