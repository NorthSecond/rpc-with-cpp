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

void HttpParser::setSocket(int socket)
{
    _fd = socket;
}

void HttpParser::parse()
{
    std::string line = readLine();
    std::istringstream iss(line);
    iss >> _method >> _path >> _protocolVersion;

    // headers
    line = readLine();
    while (!line.empty())
    {
        auto colonIdx = line.find_first_of(':');
        auto name = line.substr(0, colonIdx);
        auto value = line.substr(colonIdx + 1);
        _headers[name] = value;
        line = readLine();
    }

    int contentLength = 0;
    auto Content_Length = _headers["Content-Length"];
    if (!Content_Length.empty())
    {
        contentLength = atoi(Content_Length.c_str());
    }

    _body.assign(readn(contentLength));
}

std::string HttpParser::header(const std::string &headerName)
{
    return _headers[headerName];
}

std::string HttpParser::path() const
{
    return _path;
}

std::string HttpParser::method() const
{
    return _method;
}

std::string HttpParser::protocolVersion() const
{
    return _protocolVersion;
}

std::vector<std::string> HttpParser::headerNames() const
{
    std::vector<std::string> names;
    for (auto &pair : _headers)
    {
        names.push_back(pair.first);
    }
    return names;
}

std::string HttpParser::body() const
{
    return _body;
}

int HttpParser::readByte(char &c)
{
    if (r == w)
    {
        r = 0;
        if ((w = recv(_fd, buffer, 1024, 0)) == 0)
            return 0;
    }
    c = buffer[r++];
    return 1;
}

std::string HttpParser::readLine()
{
    char c = '\0';
    std::string result;
    result.reserve(1024);
    while (true)
    {
        if (!readByte(c))
            break;
        result.push_back(c);
        if (c == '\n')
            break;
    }
    return result.substr(0, result.find_first_of("\r\n"));
}

std::string HttpParser::readn(int max)
{
    char c = '\0';
    std::string result;
    result.reserve(max + 1);
    for (int i = 0; i < max; ++i)
    {
        if (!readByte(c))
            break;
        result.push_back(c);
    }
    return result;
}

void httpProc(int fd, std::string body)
{
    HttpParser req;
    req.setSocket(fd);
    req.parse();

    for (auto &headerName : req.headerNames())
    {
        std::cout << headerName << ": " << req.header(headerName) << std::endl;
    }

    std::string fmt =
        "HTTP/1.1 200 OK \r\n"
        "Server: nginx\r\n"
        "Accept-Encoding:protobuf, deflate\r\n"
        "Accept-Language:zh-CN,zh;q=0.9\r\n"
        "Cache-Control:no-cache\r\n"
        "Connection:keep-alive\r\n"
        "Content-Type:application/json;charset=UTF-8\r\n"
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