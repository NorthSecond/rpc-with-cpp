/**
 * @file HttpServer.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-07-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "HttpServer.hpp"
#include <string.h>

void httpProc(int fd, std::string body)
{
    std::string fmt =
        "HTTP/1.1 200 OK \r\n"
        "Accept-Encoding:protobuf, deflate\r\n"
        "Accept-Language:zh-CN,zh;q=0.9\r\n"
        "Cache-Control:no-cache\r\n"
        "Connection:keep-alive\r\n"
        "Content-Length: %d\r\n"
        "\r\n"
        "%s";
    char res[1055];
    sprintf(res, fmt.c_str(), body.size(), body.c_str());
    // body.size(),
    size_t size = strlen(res);
    send(fd, res, size, 0);

    close(fd);
}

std::string const parserHttp(int fd){
    HttpParser req;
    req.setSocket(fd);
    req.parse();

    return req.body();
}