cmake_minimum_required(VERSION 3.10)


project(sudoku)

add_executable(basic basic.cc sudoku.cc)
target_link_libraries(basic muduo_net muduo_base pthread rt)