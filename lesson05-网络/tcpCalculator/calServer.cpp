#include "calServer.hpp"
#include "protocol.hpp"

#include <fstream>
#include <memory>
#include <unordered_map>

using namespace Server;

void Usage() {
    std::cout << "Usage form: ./calServer port" << std::endl;
}

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

int main(int argc, char* argv[]) {
    if (argc != 2) {
        Usage();
        exit(USAGE_ERR);
    }

    uint16_t port = atoi(argv[1]);

    std::unique_ptr<calServer> server(new calServer(port));

    server->initServer();
    server->start(callback);

    return 0;
}
