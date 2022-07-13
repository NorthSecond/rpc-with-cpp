cmake_minimum_required(VERSION 3.0.2)

# include_directories(
#     ${PROJECT_SOURCE_DIR}
#     ${PROJECT_SOURCE_DIR}/../include
#     )

# register center
add_executable(ExampleRegisterCenter
    RegisterCenter.cpp
)
target_link_libraries(ExampleRegisterCenter
     Rpc-Lib
)

# server
    add_executable(ExampleServer
    exMessage.pb.cc
    exServer.cpp
)
target_link_libraries(ExampleServer
     Rpc-Lib
)

# Client
add_executable(ExampleSyncClient
    exMessage.pb.cc
    syncClient.cpp
)
target_link_libraries(ExampleSyncClient
     Rpc-Lib
)