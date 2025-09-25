
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <memory>

#include <sys/select.h>

#include "pollServer.hpp"

using namespace std;
using namespace Server;

void usageFormat() {
    cout << "fromat: ./selectServer server_port" << endl;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        usageFormat();
        exit(USAGE_ERR);
    }
    int port = atoi(argv[1]);
    unique_ptr<pollServer> server(new pollServer(port));
    server->init();
    server->start();

    return 0;
}