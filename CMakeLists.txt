cmake_minimum_required(VERSION 3.0.0)
project(rpc-with-cpp VERSION 0.1.0)

# include(CTest)
# enable_testing()
set(CMAKE_CXX_STANDARD 17)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include_directories(${PROJECT_SOURCE_DIR}/include)
include(CPack)

add_subdirectory(example)

add_library( myrpc
    net/RpcClient.hpp
    net/RpcServer.hpp
    net/RpcRegisterCenter.hpp
    util/skipList.hpp
    protocoder/RpcMessage.pb.h
    net/RpcClient.cpp
    net/RpcServer.cpp
    net/RpcRegisterCenter.cpp
    util/skipList.cpp
    protocoder/RpcMessage.pb.cc
)

INSTALL(
        TARGETS myrpc
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION libstatic
)

INSTALL(
        FILES
        net/RpcClient.hpp
        net/RpcServer.hpp
        net/RpcRegisterCenter.hpp
        util/skipList.hpp
        protocoder/RpcMessage.pb.h

        DESTINATION include
)