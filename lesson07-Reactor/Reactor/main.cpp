
#include <iostream>
#include <memory>

#include "tcpServer.hpp"

using namespace Server;

void callback(const Request& req, Response& resp) {
    // 业务逻辑处理
    switch (req.op) {
        case '+':
            resp.result = req.num1 + req.num2;
            break;
        case '-':
            resp.result = req.num1 - req.num2;

            break;
        case '*':
            resp.result = req.num1 * req.num2;
            break;
        case '/': {
            if (req.num2 == 0) {
                // div zero error!
                resp.exitcode = DIV_ZERO;
            } else {
                resp.result = req.num1 / req.num2;
            }
            break;
        }
        case '%': {
            if (req.num2 == 0) {
                // mod zero error!
                resp.exitcode = MOD_ZERO;
            } else {
                resp.result = req.num1 % req.num2;
            }
            break;
        }
        default:
            resp.exitcode = OP_ERR;
            break;
    }
}

void Calculate(connection* conn) {
    string package;
    string req_str;
    string resp_str;
    while (parsePackage(conn->inbuffer, &package)) {
        if (!deLength(package, &req_str))
            return;
        Request req;
        Response resp;
        if (!req.deserialize(req_str)) {
            return;
        }
        callback(req, resp);
        resp.serialize(&resp_str);
        string send_str = enLength(resp_str);

        // 构建好的报文返回 outbuffer
        // logMessage(DEBUG, "send_str = %s", send_str);
        conn->outbuffer += send_str;
    }
    // 写事件
    if (conn->sender) {
        conn->sender(conn);
    }
}

int main(int argc, char* argv[]) {
    unique_ptr<tcpServer> server(new tcpServer(Calculate, 8080));
    server->init();
    server->start();

    return 0;
}