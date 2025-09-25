#pragma once

#include <vector>

#include <poll.h>

#include "log.hpp"
#include "socket.hpp"

using namespace std;

namespace Server {

const static int fdnum = 1024;
const static int defaultfd = -1;
const static int timeout = 3000;

class pollServer {
private:
    // for debug:
    void print() {
        cout << "fd list: ";
        for (int i = 0; i < fdnum; i++) {
            if (rfds[i].fd != defaultfd)
                cout << rfds[i].fd << " ";
        }
        cout << endl;
    }

    void Accepter() {
        logMessage(NORMAL, "Accepter in...");
        string clientip;
        uint16_t clientport;
        int sock = Sock::Accept(listensock, &clientip, &clientport);
        if (sock < 0) {
            logMessage(ERROR, "accept fail");
            exit(ACCEPT_ERR);
        }
        logMessage(NORMAL, "have a new link [%s : %d], sock: %d", clientip.c_str(), clientport, sock);

        int i = 0;
        for (; i < fdnum; i++) {
            if (rfds[i].fd != defaultfd) {
                continue;
            }
            break;
        }
        if (i == fdnum) {
            logMessage(WARNING, "server have fulled, please wait...");
        } else {
            rfds[i].fd = sock;
            rfds[i].events = POLLIN;
            rfds[i].revents = 0;
        }
        print();
        logMessage(NORMAL, "Accepter off...");
    }

    void Recver(int pos) {
        logMessage(NORMAL, "Recver in...");
        char buffer[1024];
        int fd = rfds[pos].fd;
        ssize_t n = recv(fd, buffer, sizeof(buffer) - 1, 0);
        if (n <= 0) {
            close(fd);
            rfds[pos].fd = defaultfd;
            rfds[pos].events = POLLIN;
            rfds[pos].revents = 0;
            logMessage(WARNING, "client quit");
            return;
        }

        buffer[n] = 0;
        string msg = "client# ";
        msg += buffer;
        write(fd, msg.c_str(), msg.size());
        logMessage(NORMAL, "Recver off...");
    }

    void handlerEvent() {
        if (rfds[0].revents & POLLIN) {
            Accepter();
        }
        for (int i = 0; i < fdnum; i++) {
            if (rfds[i].fd == defaultfd || rfds[i].fd == listensock) {
                continue;
            }
            logMessage(DEBUG, "rfds[%d] = %d", i, rfds[i]);

            if (rfds[i].events & POLLIN && rfds[i].revents & POLLIN) {
                Recver(i);
            }
        }
    }

    void resetRfds(int pos) {
        rfds[pos].fd = defaultfd;
        rfds[pos].events = 0;
        rfds[pos].revents = 0;
    }

public:
    pollServer(uint16_t _port) : port(_port), listensock(-1), rfds(nullptr) {}

    void init() {
        listensock = Sock::Socket();
        Sock::Bind(listensock, port);
        Sock::Listen(listensock);
        rfds = new struct pollfd[fdnum];
        for (int i = 0; i < fdnum; i++) {
            resetRfds(i);
        }
        rfds[0].fd = listensock;
        rfds[0].events = POLLIN;
    }

    void start() {
        while (true) {
            int n = poll(rfds, fdnum, timeout);
            switch (n) {
                case 0:
                    logMessage(NORMAL, "timeout...");
                    break;
                case -1:
                    logMessage(ERROR, "%s", strerror(errno));
                    break;
                default:
                    logMessage(DEBUG, "have %d links", n);
                    handlerEvent();
                    break;
            }
        }
    }

    ~pollServer() {
        close(listensock);
        listensock = -1;
        delete[] rfds;
    }

private:
    uint16_t port;
    int listensock;
    struct pollfd* rfds;
};

}  // namespace Server
