cmake_minimum_required(VERSION 3.0)


project(finger)


add_executable(finger01 finger01.cc)
target_link_libraries(finger01 muduo_net muduo_base pthread rt)

add_executable(finger02 finger02.cc)
target_link_libraries(finger02 muduo_net muduo_base pthread rt)


add_executable(finger03 finger03.cc)
target_link_libraries(finger03 muduo_net muduo_base pthread rt)

add_executable(finger04 finger04.cc)
target_link_libraries(finger04 muduo_net muduo_base pthread rt)

add_executable(finger05 finger05.cc)
target_link_libraries(finger05 muduo_net muduo_base pthread rt)

add_executable(finger06 finger06.cc)
target_link_libraries(finger06 muduo_net muduo_base pthread rt)