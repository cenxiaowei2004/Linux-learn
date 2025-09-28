#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "error.hpp"
#include "log.hpp"

static const int backlog = 5;

class Sock {
public:
    Sock() : listensock(defaultsock) {}

    int Fd() { return listensock; }

    void Socket() {
        listensock = socket(AF_INET, SOCK_STREAM, 0);
        if (listensock < 0) {
            logMessage(FATAL, "socket fail!");
            exit(SOCKET_ERR);
        }
        // 支持端口复用
        int opt = 1;
        setsockopt(listensock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    }

    void Bind(int port) {
        struct sockaddr_in local;
        local.sin_family = AF_INET;
        local.sin_addr.s_addr = INADDR_ANY;
        local.sin_port = htons(port);

        int n = bind(listensock, (struct sockaddr*)&local, static_cast<socklen_t>(sizeof(local)));
        if (n < 0) {
            logMessage(FATAL, "bind internet error!");
            exit(BIND_ERR);
        }
    }

    void Listen() {
        int n = listen(listensock, backlog);
        if (n < 0) {
            logMessage(FATAL, "listen socket error!");
            exit(LISTEN_ERR);
        }
    }

    int Accept(string* clientip, uint16_t* clientport) {
        struct sockaddr_in peer;
        socklen_t len = sizeof(peer);

        int sock = accept(listensock, (struct sockaddr*)&peer, &len);
        if (sock < 0) {
            logMessage(ERROR, "accept socket error!");
            // exit(ACCEPT_ERR);
        }
        *clientip = inet_ntoa(peer.sin_addr);
        *clientport = ntohs(peer.sin_port);
        logMessage(NORMAL, "accept socket success!");
        return sock;
    }

    ~Sock() {
        if (listensock) {
            close(listensock);
        }
    }

private:
    int listensock;
};
