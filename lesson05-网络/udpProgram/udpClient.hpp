#include <arpa/inet.h>
#include <strings.h>
#include <sys/socket.h>

#include <errno.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <iostream>

namespace Client {

using namespace std;

enum { USAGE_ERR = 1, SOCKET_ERR, BIND_ERR };

class udpClient {
public:
    udpClient(const string& _serverIp, const uint16_t& _serverPort)
        : serverIp(_serverIp), serverPort(_serverPort), fd(-1) {}

    void initClient() {
        fd = socket(AF_INET, SOCK_DGRAM, 0);
        if (fd == -1) {
            cerr << "socket error:" << errno << ":" << strerror(errno) << endl;
            exit(SOCKET_ERR);
        }
    }

    void run() {
        string message;
        struct sockaddr_in server;
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = inet_addr(serverIp.c_str());
        server.sin_port = htons(serverPort);
        while (true) {
            cout << "[client-Input message]#";
            cin >> message;
            sendto(fd, message.c_str(), sizeof(message), 0, (sockaddr*)&server,
                   sizeof(server));
        }
    }

    ~udpClient() {
        if (fd != -1) {
            close(fd);
            fd = -1;
        }
    }

private:
    string serverIp;
    uint16_t serverPort;
    int fd;
};
}  // namespace Client
