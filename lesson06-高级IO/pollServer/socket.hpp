#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "log.hpp"
#include "error.hpp"


static const int backlog = 5;

class Sock {
public:
    static int Socket() {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            logMessage(FATAL, "socket fail!");
            exit(SOCKET_ERR);
        }
        // 支持端口复用
        int opt = 1;
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        return sock;
    }

    static void Bind(int sock, int port) {
        struct sockaddr_in local;
        local.sin_family = AF_INET;
        local.sin_addr.s_addr = INADDR_ANY;
        local.sin_port = htons(port);

        int n = bind(sock, (struct sockaddr*)&local, static_cast<socklen_t>(sizeof(local)));
        if (n < 0) {
            logMessage(FATAL, "bind internet error!");
            exit(BIND_ERR);
        }
    }

    static void Listen(int sock) {
        int n = listen(sock, backlog);
        if (n < 0) {
            logMessage(FATAL, "listen socket error!");
            exit(LISTEN_ERR);
        }
    }

    static int Accept(int listensock, string* clientip, uint16_t* clientport) {
        struct sockaddr_in peer;
        socklen_t len = sizeof(peer);

        int sock = accept(listensock, (struct sockaddr*)&peer, &len);
        if (sock < 0) {
            logMessage(ERROR, "accept socket error!");
            exit(ACCEPT_ERR);
        }
        *clientip = inet_ntoa(peer.sin_addr);
        *clientport = ntohs(peer.sin_port);
        logMessage(NORMAL, "accept socket success!");
        return sock;
    }
};
