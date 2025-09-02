#include "tcpServer.hpp"
#include "daemon.hpp"

#include <fstream>
#include <memory>
#include <unordered_map>

using namespace Server;

void Usage() {
    std::cout << "Usage form: ./tcpServer port" << std::endl;
}

void routeMessage(int fd, string clientIp, uint16_t clientPort, string message) {
    ;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        Usage();
        exit(USAGE_ERR);
    }

    uint16_t port = atoi(argv[1]);

    std::unique_ptr<tcpServer> server(new tcpServer(port, routeMessage));

    server->initServer();
    server->start();

    return 0;
}
