/**
 * @file RpcClient.hpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-07-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include <bits/stdc++.h>
#include <google/protobuf/service.h>
#include "protocoder/RpcMessage.pb.h"
#include "HttpClient.hpp"

using namespace std;

/*
 * RPC Client
 */
class RpcClient
{
public:

    RpcClient(std::string host, int port);
    ~RpcClient();

public:

    std::atomic_bool connected;
    bool myConnect();
    int socketfd;

private:
    char buffer[1055];
    string host;
    int32_t port;
};

/*
 * RPC Channel
 */
class RpcChannel : public google::protobuf::RpcChannel
{
public:
    /*构造函数*/
    RpcChannel();
    RpcChannel(RpcClient *client);
    RpcChannel(std::shared_ptr<RpcClient> client);
    ~RpcChannel();

    /*实现RpcChannel中的CallMethod方法*/
    void CallMethod(const google::protobuf::MethodDescriptor *method, google::protobuf::RpcController *controller,
                    const google::protobuf::Message *request, google::protobuf::Message *response,
                    google::protobuf::Closure *done) override;

private:
    // rpc客户端
    std::shared_ptr<RpcClient> rpcClient;

    //请求id
    std::atomic_long request_id;

    // //互斥锁
    // std::mutex connect_mu;
};
