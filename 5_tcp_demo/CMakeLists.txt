cmake_minimum_required(VERSION 3.10)


project(tcp_demo)

add_executable(discard discard.cc )
target_link_libraries(discard muduo_net muduo_base pthread rt)

add_executable(daytime daytime.cc )
target_link_libraries(daytime muduo_net muduo_base pthread rt)

add_executable(time time.cc )
target_link_libraries(time muduo_net muduo_base pthread rt)
add_executable(timeclient time_client.cc )
target_link_libraries(timeclient muduo_net muduo_base pthread rt)


add_executable(echo echo.cc )
target_link_libraries(echo muduo_net muduo_base pthread rt)


add_executable(echoclient echo_hack_client.cc )
target_link_libraries(echoclient muduo_net muduo_base pthread rt)


add_executable(epolldemo ./epoll_demo/server.cc )
target_link_libraries(epolldemo pthread rt)
