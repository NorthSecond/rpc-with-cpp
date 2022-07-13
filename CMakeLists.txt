cmake_minimum_required(VERSION 3.0.0)
project(rpc-with-cpp VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/bin)

set(PROTOBUF_DIR ${PROJECT_SOURCE_DIR}/include)
include(CPack)


include_directories(
        ${PROJECT_SOURCE_DIR}
        ${PROTOBUF_DIR}
        ${PROJECT_SOURCE_DIR}/protocoder
)

add_subdirectory(example)

set(SRC_LIB
        ${PROJECT_SOURCE_DIR}/net/HttpClient.cpp
        ${PROJECT_SOURCE_DIR}/net/RpcServer.cpp
        ${PROJECT_SOURCE_DIR}/net/RpcClient.cpp
        ${PROJECT_SOURCE_DIR}/net/HttpServer.cpp
        ${PROJECT_SOURCE_DIR}/net/HttpParser.cpp
        ${PROJECT_SOURCE_DIR}/protocoder/RpcMessage.pb.cc
        ${PROJECT_SOURCE_DIR}/util/consistent_hash.cpp
        ${PROJECT_SOURCE_DIR}/util/murmurhash.cpp
        ${PROJECT_SOURCE_DIR}/util/skipList.cpp
)

link_directories(${PROJECT_SOURCE_DIR})
add_library(Rpc-Lib ${SRC_LIB})

