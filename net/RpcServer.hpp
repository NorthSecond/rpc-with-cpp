/**
 * @file RpcServer.hpp
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
#include "RpcMessage.pb.h"

typedef std::function<void(rpcDemo::RpcMessage, rpcDemo::RpcMessage &)> HandleRpcCallFunc;

class RpcMsgService : public Service<rpcDemo::RpcMessage, rpcDemo::RpcMessage>
{
public:
    /*RpcMsgService构造函数*/
    RpcMsgService(HandleRpcCallFunc arg);

    /*RpcMsgService调用流程*/
    Future<rpcDemo::RpcMessage> operator()(rpcDemo::RpcMessage request) override;

private:
    // HandleRpcCallFunc _handleRpcCall; // RPC call function
};

class RpcServer
{
    public:
        typedef int32_t serviceID;
        typedef int32_t methodID;
        struct MethodData
        {
            const google::protobuf::MethodDescriptor *m_descriptor; // protobuf方法描述
            const google::protobuf::Message *request_proto;         // protobuf请求数据类型
            const google::protobuf::Message *response_proto;        // protobuf响应数据类型
        };
        /*服务元数据*/
        struct ServiceData
        {
            google::protobuf::Service *rpcService;              // protobuf rpc service
            std::unordered_map<methodID, MethodData> methodMap; //存储methodId与方法对应关系（通过方法ID查询相应的方法）
        };

        /*注册服务*/
        bool RegisterService(::google::protobuf::Service *service);

        /*启动RPC服务器*/
        void Start(int bindPort);
    private:
        /*根据serviceId获取service*/
        google::protobuf::Service *GetService(uint32_t serviceId);

        /*根据methodId获取method*/
        MethodData *GetMethod(uint32_t serviceId, uint32_t methodId);

        /*根据RPC请求的serviceId和methodId取出相应的方法去执行并获取执行结果写回响应*/
        void handleRpcCall(rpc::codec::RpcMessage req, rpc::codec::RpcMessage &res);

        /*RPC服务方法调用map*/
        std::unordered_map<serviceID, ServiceData> _rpcCallMap;

        
    public:
        RpcServer(/* args */);
        ~RpcServer();
};

RpcServer::RpcServer(/* args */)
{
}

RpcServer::~RpcServer()
{
}
