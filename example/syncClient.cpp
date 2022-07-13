/**
 * @file syncClient.cpp
 * @author yangyf83 (yangyf83@mail2.sysu.edu.cn)
 * @brief 
 * @version 0.1
 * @date 2022-07-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <bits/stdc++.h>

#include "net/RpcClient.hpp"
#include "example/exMessage.pb.h"

using namespace std;

int main(int argc, const char** argv) {
    RpcClient client("127.0.0.1", 8080);
    RpcChannel channel(&client);

    /*
        service Service{
            rpc Add (request) returns (result);
            rpc sub (request) returns (result);
        }
    */

    rpcExample::Service_Stub myService(&channel);
    int i = 0;
    while(i++ < 1e6){
        if(i % 2){
            rpcExample::request *req = new rpcExample::request();
            rpcExample::result *res = new rpcExample::result();

            req->set_a(i);
            req->set_b(i);

            myService.Add(NULL, req, res, NULL);
            printf("%ld + %ld = %ld\n", req->a(), req->b(), res->c());
        }else{
            rpcExample::request *req = new rpcExample::request();
            rpcExample::result *res = new rpcExample::result();

            req->set_a(i);
            req->set_b(i / 2);

            myService.Sub(NULL, req, res, NULL);
            printf("%ld - %ld = %ld\n", req->a(), req->b(), res->c());
        }
    }
    return 0;
}