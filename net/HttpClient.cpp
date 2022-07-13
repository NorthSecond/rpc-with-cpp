/**
 * @file HttpClient.cpp
 * @author yangyf83 (yangyf83@mail2.sysu.edu.cn)
 * @brief 
 * @version 0.1
 * @date 2022-07-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "HttpParser.hpp"
#include <string.h>

bool httpSender(int fd, std::string fill){
    std::string fmt =
        "GET server HTTP/1.1\r\n"
        "Cache-Control:no-cache\r\n"
        "Connection:keep-alive\r\n"
        "Content-Length: %d\r\n"
        "\r\n"
        "%s";

    char res[1024];
    sprintf(res, fmt.c_str(), fill.size(), fill.c_str());
    size_t size = strlen(res);
    send(fd, res, size, 0);
    return true;
}