/**
 * @file RpcServer.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2022-07-12
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "RpcServer.hpp"
#include "murmurhash3.h"
#include "HttpServer.hpp"

using namespace std;

bool RpcServer::RegisterService(::google::protobuf::Service *service)
{
    const google::protobuf::ServiceDescriptor *pSerDes = service->GetDescriptor();

    int methodCnt = pSerDes->method_count();

    string serviceName = pSerDes->full_name();
    serviceID serviceId = 1;

    ServiceData servicetmp;

    for (int i = 0; i < methodCnt; i++)
    {
        const google::protobuf::MethodDescriptor *pMethodDes = pSerDes->method(i);
        const google::protobuf::Message *request_proto = &service->GetRequestPrototype(pMethodDes);
        const google::protobuf::Message *response_proto = &service->GetResponsePrototype(pMethodDes);

        MethodData methodtmp;
        methodtmp.m_descriptor = pMethodDes;
        methodtmp.request_proto = request_proto;
        methodtmp.response_proto = response_proto;

        //获取method id
        uint32_t methodId = murmur3_32(serviceName.c_str(), serviceName.size());

        servicetmp.rpcService = service;
        servicetmp.methodMap[methodId] = methodtmp;
    }

    _rpcCallMap[serviceId] = servicetmp;

    return true;
}

void RpcServer::handleRpcCall(rpcDemo::RpcMessage req, rpcDemo::RpcMessage &res)
{
    google::protobuf::Service *rpc_service = GetService(req.serviceid());
    MethodData *this_method = GetMethod(req.serviceid(), req.methodid());

    google::protobuf::Message *request = this_method->request_proto->New();
    google::protobuf::Message *response = this_method->response_proto->New();
    // 反序列化
    request->ParseFromString(req.request());

    rpc_service->CallMethod(this_method->m_descriptor, NULL, request, response, NULL);

    string response_str;
    response->SerializeToString(&response_str);

    // 序列化了又
    res.set_type(rpcDemo::RESPONSE);
    res.set_id(req.id());
    res.set_response(response_str);
    res.set_serviceid(req.serviceid());
    res.set_methodid(req.methodid());
    res.set_error(rpcDemo::NO_ERROR);
}

//根据serviceId获取service
google::protobuf::Service *RpcServer::GetService(uint32_t serviceId)
{
    return _rpcCallMap[serviceId].rpcService;
}

//根据methodId获取method
RpcServer::MethodData *RpcServer::GetMethod(uint32_t serviceId, uint32_t methodId)
{
    return &_rpcCallMap[serviceId].methodMap[methodId];
}

//启动RPC服务器
void RpcServer::Start(int bindPort)
{
    // TODO: 服务器请求
    int hostfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    epoll_event event, evClients[MAX_CLIENT];

    int epollfd = epoll_create(1024);

    event.data.fd = hostfd;
    event.events = EPOLLIN | EPOLLET;

    epoll_ctl(epollfd, EPOLL_CTL_ADD, hostfd, &event);

    sockaddr_in sin;
    sin.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &sin.sin_addr.s_addr);
    sin.sin_port = htons(bindPort);
    bind(hostfd, (const sockaddr *)&sin, sizeof(sin));
    listen(hostfd, 20);

    cout << "server start listen port:" << sin.sin_port << endl;

    int eps = 0;
    socklen_t length = 0;
    while (true)
    {
        eps = epoll_wait(epollfd, evClients, MAX_CLIENT, -1);
        for (int i = 0; i < eps; ++i)
        {
            if (evClients[i].data.fd == hostfd)
            {
                int fdNow = accept(hostfd, (sockaddr *)&sin, &length);
                if (fdNow < 0)
                {
                    cerr << " fdNow < 0" << endl;
                    continue;
                }

#ifdef DEBUG
                char ip[16];
                cout << "accept from: " << inet_ntop(AF_INET, &sin.sin_addr.s_addr, ip, sizeof(sin)) << ":" << sin.sin_port << endl;
#endif

                event.data.fd = fdNow;
                event.events = EPOLLIN | EPOLLET;
                fcntl(fdNow, F_SETFL, fcntl(fdNow, F_GETFL) | O_NONBLOCK);
                epoll_ctl(epollfd, EPOLL_CTL_ADD, fdNow, &event);
            }
            else if (evClients[i].events & EPOLLIN)
            {
                int fd = evClients[i].data.fd;
                if (fd < 0)
                    continue;
                thread worker([fd, this]()
                              { 
                                // 异步处理fd
                                string req = parserHttp(fd);
                                rpcDemo::RpcMessage request;
                                request.ParseFromString(req);
                                rpcDemo::RpcMessage res;
                                handleRpcCall(request, res);
                                httpProc(fd, res.SerializeAsString()); 
                                });

                worker.detach();
            }
        }
    }

    // hostfd.
    return;
}