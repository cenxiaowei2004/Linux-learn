
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <memory>

#include <sys/select.h>

#include "epollServer.hpp"

using namespace std;
using namespace Server;

string func(char* buffer) {
    // 复杂的逻辑处理...
    string msg = "client# ";
    return msg + buffer;
}

void usageFormat() {
    cout << "fromat: ./epollServer server_port" << endl;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        usageFormat();
        exit(USAGE_ERR);
    }
    int port = atoi(argv[1]);
    unique_ptr<epollServer> server(new epollServer(port, func));
    server->init();
    server->start();

    return 0;
}