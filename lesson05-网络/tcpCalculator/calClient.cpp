#include "calClient.hpp"

#include <memory>

using namespace Client;

void Usage() {
    std::cout << "Usage form: ./calClient server_ip server_port" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        Usage();
        exit(USAGE_ERR);
    }
    string server_ip = argv[1];
    uint16_t server_port = atoi(argv[2]);

    std::unique_ptr<calClient> cilent(new calClient(server_ip, server_port));

    cilent->initClient();
    cilent->run();

    return 0;
}
