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

#include <sys/socket.h>
#include <sys/unistd.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "RpcClient.hpp"
#include "util/murmurhash3.h"

RpcClient::RpcClient(std::string host, int port)
{
    this->host = host;
    this->port = port;
    connected = false;
    socketfd = -1;
}

bool RpcClient::myConnect(){
    struct sockaddr_in servaddr;
    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == socketfd){
        return false;
    }
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    inet_pton(AF_INET, host.c_str(), &servaddr.sin_addr);
    servaddr.sin_port = htons(port);
    if (-1 == connect(socketfd, (struct sockaddr *)&servaddr, sizeof(servaddr)))
    {
        printf("Connect error(%d): %s\n", errno, strerror(errno));
        return false;
    }
    return true;
}

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

    while (!rpcClient->connected.load())
    {
        rpcClient->connected = rpcClient->myConnect();
    }

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
    char buffer[1024];
    if (done)
    {
        rpcDemo::RpcMessage Res;
        thread worker([Res,response ,this, Req, buffer, done]() {
        string str = "";
        Req.SerializeToString(&str);
        httpSender(rpcClient->socketfd, str);
        recv(rpcClient->socketfd, (void*) buffer, 1023, 0);
        string tmpStr = string(buffer);
        int index = tmpStr.find("\r\n\r\n");
        tmpStr = tmpStr.substr(index + 4);
        // Res.ParseFromString(tmpStr);
        response->ParseFromString(Res.response());
        done->Run();
        });
        // 异步调用
    }
    else
    {
        rpcDemo::RpcMessage Res;
        // 同步调用
        string str = "";
        Req.SerializeToString(&str);
        httpSender(rpcClient->socketfd, str);
        recv(rpcClient->socketfd, buffer, 1023, 0);
        string tmpStr = string(buffer);
        int index = tmpStr.find("\r\n\r\n");
        tmpStr = tmpStr.substr(index + 4);
        Res.ParseFromString(tmpStr);
        response->ParseFromString(Res.response());
    }
}