/**
 * @file HttpServer.hpp
 * @author yangyf83 (yangyf83@mail2.sysu.edu.cn)
 * @brief 
 * @version 0.1
 * @date 2022-07-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once
#include "HttpParser.hpp"

#define MAX_CLIENT 16

void httpProc(int fd, std::string body);

std::string const parserHttp(int fd);