
#include <arpa/inet.h>
#include <strings.h>
#include <sys/socket.h>

#include <errno.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>

namespace Server {

using namespace std;

enum { USAGE_ERR = 1, SOCKET_ERR, BIND_ERR, OPEN_ERR };

const string DEFAULT_IP = "0.0.0.0";

#define gnum 1024

typedef function<void(string, uint16_t, string)> func;

class udpServer {
public:
    udpServer(const uint16_t& _port,
              const func& _callback,
              const string& _ip = DEFAULT_IP)
        : port(_port), ip(_ip), callback(_callback), fd(-1) {}

    void initServer() {
        // 1. 创建套接字
        fd = socket(AF_INET, SOCK_DGRAM, 0);
        if (fd == -1) {
            cerr << "socket error:" << errno << ":" << strerror(errno) << endl;
            exit(SOCKET_ERR);
        }

        struct sockaddr_in local;
        bzero(&local, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(port);
        local.sin_addr.s_addr = inet_addr(ip.c_str());
        // 任意地址绑定
        // local.sin_addr.s_addr = INETADD_ANY;

        // 2. 绑定 port、ip
        int ret = bind(fd, (struct sockaddr*)&local, sizeof(local));
        if (ret == -1) {
            cerr << "bind error:" << errno << ":" << strerror(errno) << endl;
            exit(BIND_ERR);
        }
    }

    void start() {
        char buffer[gnum];
        while (true) {
            // 服务器读取数据：
            struct sockaddr_in peer;
            socklen_t len = sizeof(peer);
            ssize_t n =
                recvfrom(fd, buffer, gnum - 1, 0, (sockaddr*)&peer, &len);
            if (n > 0) {
                // success:
                string clientIp = inet_ntoa(peer.sin_addr);
                uint16_t clientPort = ntohs(peer.sin_port);
                buffer[n] = 0;
                string message = buffer;
                cout << "[" << clientIp + "-" << clientPort << "]# " << message
                     << endl;
                callback(clientIp, clientPort, message);
            }
        }
    }

    ~udpServer() {
        if (fd != -1) {
            close(fd);
            fd = -1;
        }
    }

private:
    // uint16_t : unsigned int 16 bit
    uint16_t port;
    string ip;
    int fd;
    func callback;
};
}  // namespace Server
