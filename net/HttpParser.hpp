/**
 * @file HttpParser.hpp
 * @author yangyf83 (yangyf83@mail2.sysu.edu.cn)
 * @brief 
 * @version 0.1
 * @date 2022-07-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

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

class HttpParser
{
public:
    void setSocket(int socket);
    void parse();

    std::string method() const;
    std::vector<std::string> headerNames() const;
    std::string header(const std::string &headerName);
    std::string path() const;
    std::string protocolVersion() const;
    std::string body() const;

protected:
    int readByte(char &c);
    std::string readLine();
    std::string readn(int max);

private:
    int _fd;
    std::string _method;
    std::string _path;
    std::string _protocolVersion;
    std::map<std::string, std::string> _headers;
    std::string _body;

    int r = 0, w = 0;
    char buffer[1024];
};