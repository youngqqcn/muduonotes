
cmake_minimum_required(VERSION  3.10)


project(filetransfer)


add_executable(filetransferv1 filetransfer_v1.cc)
target_link_libraries(filetransferv1 muduo_net muduo_base pthread rt)



add_executable(filetransferv2 filetransfer_v2.cc)
target_link_libraries(filetransferv2 muduo_net muduo_base pthread rt)


add_executable(filetransferv3 filetransfer_v3.cc)
target_link_libraries(filetransferv3 muduo_net muduo_base pthread rt)
