/**
 * @file RegisterCenter.cpp
 * @author yangyf83 (yangyf83@mail2.sysu.edu.cn)
 * @brief
 * @version 0.1
 * @date 2022-07-13
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <bits/stdc++.h>
#include "util/consistent_hash.h"
#include "util/skipList.hpp"
#include "util/murmurhash3.h"

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <iostream>
#include <thread>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

using namespace std;

struct HostMessage
{
    string Hostname;
    uint32_t port;
    HostMessage(string str, uint32_t p)
    {
        Hostname = str;
        port = p;
    }
};

class Servers
{
public:
    ConsistentHash consistHash;
    SkipList<HostMessage> ServerList;
    Servers() : consistHash(0, 100), ServerList(0, HostMessage("", 0))
    {
    }
};

int main(void)
{
    Servers server;
    int hostfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    epoll_event event, evClients[100];

    int epollfd = epoll_create(1024);

    event.data.fd = hostfd;
    event.events = EPOLLIN | EPOLLET;

    epoll_ctl(epollfd, EPOLL_CTL_ADD, hostfd, &event);

    sockaddr_in sin;
    sin.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &sin.sin_addr.s_addr);
    sin.sin_port = 12000;
    bind(hostfd, (const sockaddr *)&sin, sizeof(sin));
    listen(hostfd, 20);

    cout << "server start listen port:" << sin.sin_port << endl;

    int eps = 0;
    socklen_t length = 0;
    while (true)
    {
        eps = epoll_wait(epollfd, evClients, 100, -1);
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
                char buffer[128];
                read(fd, buffer, 127);
                if (strcmp(buffer, "SERVER") == 0)
                {
                    socklen_t addrLen;
                    struct sockaddr_in cliAddr;
                    char *ipAddr;
                    int port;
                    addrLen = sizeof(cliAddr);
                    /*通过已连接的sockfd获取客户端的ip和port*/
                    getpeername(fd, (struct sockaddr *)&cliAddr, &addrLen);
                    strncpy(ipAddr, (const char *)inet_ntoa(cliAddr.sin_addr), 64);
                    port = (int)ntohs(cliAddr.sin_port);

                    uint32_t k = murmur3_32(ipAddr, addrLen);
                    HostMessage newHostMessage(ipAddr, port);
                    server.ServerList.insert(k, newHostMessage);
                    server.consistHash.AddNewNode(k);
                    // thread worker([fd, server]()
                    //               {

                    // });
                }
                else
                {
                    socklen_t addrLen;
                    struct sockaddr_in cliAddr;
                    char *ipAddr;
                    addrLen = sizeof(cliAddr);
                    getpeername(fd, (struct sockaddr *)&cliAddr, &addrLen);
                    strncpy(ipAddr, (const char *)inet_ntoa(cliAddr.sin_addr), 64);
                    // uint32_t k = murmur3_32(ipAddr, addrLen);
                    size_t res = server.consistHash.GetServerIndex(ipAddr);
                    SkipNode<HostMessage> *resNode = server.ServerList.find(res);
                    HostMessage msg = resNode->value;
                    bzero(buffer, 127);
                    sprintf(buffer, "%s %d", msg.Hostname.c_str(), msg.port);
                    send(fd, buffer, strlen(buffer), 0);
                }
                close(fd);
            }
        }
    }
}
