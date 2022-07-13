/**
 * @file exServer.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-07-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <bits/stdc++.h>
#include "net/RpcServer.hpp"
#include "example/exMessage.pb.h"
#include "protocoder/RpcMessage.pb.h"

using namespace std;

class ServiceImpl : public rpcExample::Service {
public:
    ServiceImpl() : callCount(0L) {}
    /* 实现 MyService::Add() 方法 */
    void Add(::google::protobuf::RpcController *controller, const ::rpcExample::request *request,
             ::rpcExample::result *response, ::google::protobuf::Closure *done) override
    {
        response->set_c(request->a() + request->b());

        cout << "RPC Server Called MyService::Add()  " << callCount++ << endl;

        if (done)
            done->Run();
    }

    void Sub(::google::protobuf::RpcController *controller, const ::rpcExample::request *request, ::rpcExample::result *response, ::google::protobuf::Closure *done) override
    {
        response->set_c(request->a() - request->b());

        cout << "RPC Server Called MyService::Sub()  " << callCount++ << endl;

        if (done)
            done->Run();
    }

private:
    //服务调用次数计数
    std::atomic_long callCount;
};

int main(int argc, char const *argv[])
{
    ServiceImpl *impl = new ServiceImpl();

    RpcServer server;
    server.RegisterService(impl);
    int port;
    if(argc < 2){
        port = 8080;
    }else{
        port = atoi(argv[1]);
    }
    server.Start(port);

    cout << "Server running" << endl;
    return 0;
}
