cmake_minimum_required(VERSION 3.10)

set(CMAKE_BUILD_TYPE Debug)

project(timers)


add_executable(timerprinter timerprinter.cc)
target_link_libraries(timerprinter muduo_net muduo_base z pthread )