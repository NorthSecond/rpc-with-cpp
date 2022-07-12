/**
 * @file RpcClient.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2022-07-12
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "RpcClient.hpp"
#include "murmurhash3.h"

/*
 * RpcChannel
 */
// RpcChannel 构造函数
RpcChannel::RpcChannel()
{
}

RpcChannel::RpcChannel(RpcClient *client)
    : rpcClient(client), request_id(0)
{
}

RpcChannel::RpcChannel(std::shared_ptr<RpcClient> client)
    : rpcClient(client), request_id(0)
{
}

// RpcChannel 析构函数
RpcChannel::~RpcChannel()
{
}

//实现RpcChannel中的CallMethod方法
void RpcChannel::CallMethod(const google::protobuf::MethodDescriptor *method, google::protobuf::RpcController *controller, const google::protobuf::Message *request, google::protobuf::Message *response, google::protobuf::Closure *done)
{

    // if (!rpcClient->connected.load())
    // {
    //     std::unique_lock<std::mutex>(this->connect_mu);
    //     rpcClient->pipeline = rpcClient->tcpClient.connect(rpcClient->remoteAddress).get();
    //     rpcClient->dispatcher->setPipeline(rpcClient->pipeline);
    //     rpcClient->connected = true;
    // }

    //获取service id 进行farmhash运算 将service name 转为 uint32
    std::string serviceName = method->service()->full_name();
    uint32_t serviceId = murmur3_32(serviceName.c_str(), (uint32_t) serviceName.size(), 17U);

    rpcDemo::RpcMessage Req;
    Req.set_type(rpcDemo::REQUEST);
    Req.set_id(request_id++);
    Req.set_serviceid(serviceId);
    Req.set_methodid(method->index());

    std::string request_str;
    request->SerializeToString(&request_str);
    Req.set_request(std::move(request_str));

    // TODO: 实现同步和异步调用
    
}