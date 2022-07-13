cmake_minimum_required(VERSION 3.0.2)

# server

include_directories(
    ${PROJECT_SOURCE_DIR}
    )
add_executable(ExampleServer
    exMessage.pb.cc
    exServer.cpp
)

# Client
add_executable(ExampleSyncClient
    exMessage.pb.cc
    syncClient.cpp
)