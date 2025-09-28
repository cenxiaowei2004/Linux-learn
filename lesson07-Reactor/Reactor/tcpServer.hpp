#pragma once

#include <functional>
#include <unordered_map>

#include "epoller.hpp"
#include "error.hpp"
#include "log.hpp"
#include "protocol.hpp"
#include "socket.hpp"
#include "util.hpp"

using namespace std;

namespace Server {

class connection;
class tcpServer;
using func_t = function<void(connection*)>;

const static int timeout = 3000;
const static int eventnum = 10;

/* ET模式下，怎么让程序员全部读取？每个 sock 对应着缓冲区 */
class connection {
public:
    connection(tcpServer* _ts, int _sock) : ts(_ts), sock(_sock) {}

    void Register(func_t _recver, func_t _sender, func_t _excepter) {
        recver = _recver;
        sender = _sender;
        excepter = _excepter;
    }

    ~connection() {}

public:
    int sock;
    // 输入输出缓冲区
    string inbuffer;
    string outbuffer;
    // sock 事件就绪了，要怎么读、写、错误异常
    func_t recver;
    func_t sender;
    func_t excepter;

    tcpServer* ts;
};

class tcpServer {
private:
    /* 非阻塞处理读、写、错误 */
    void Recver(connection* conn) {
        // 怎么全部读完？全部放入输入缓冲区再处理 -> 定制协议！
        int fd = conn->sock;
        char buffer[1024];
        while (true) {
            /* 正常 */
            ssize_t n = recv(fd, buffer, sizeof(buffer) - 1, 0);
            if (n > 0) {
                buffer[n] = 0;
                conn->inbuffer += buffer;
            }
            /* 客户端退出 */
            else if (n == 0) {
                if (conn->excepter) {
                    conn->excepter(conn);
                    return;
                }

            }
            /* 出异常:统一由 excepter 处理 */
            else {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    break;
                } else if (errno == EINTR) {
                    continue;
                } else {
                    if (conn->excepter) {
                        conn->excepter(conn);
                        return;
                    }
                }
            }
        }
        logMessage(DEBUG, "inbuffer = %s", conn->inbuffer);
        calculate(conn);
    }

    void Sender(connection* conn) {
        int fd = conn->sock;
        while (true) {
            ssize_t n = send(fd, conn->outbuffer.c_str(), conn->outbuffer.size(), 0);
            if (n > 0) {
                if (conn->outbuffer.empty()) {
                    break;
                } else
                    conn->outbuffer.erase(0, n);
            } else {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    break;
                } else if (errno == EINTR)
                    continue;
                else {
                    if (conn->excepter) {
                        conn->excepter(conn);
                        return;
                    }
                }
            }
        }
        /* 数据没发完，开启对写事件的关心；数据发完了，关闭对写事件的关心 */
        if (conn->outbuffer.empty()) {
            conn->ts->enableWrite(conn, true, false);
        } else {
            conn->ts->enableWrite(conn, true, true);
        }
    }

    void Excepter(connection* conn) {
        int fd = conn->sock;
        // 先移除再关闭文件描述符
        epoll.Control(fd, 0, EPOLL_CTL_DEL);
        close(fd);
        connections.erase(fd);
        logMessage(DEBUG, "%d has released all resource", fd);
        delete conn;
    }

    void Loop() {
        int n = epoll.Wait(events, eventnum, timeout);
        // n > 0:
        // logMessage(DEBUG, "n = %d", n);
        for (int i = 0; i < n; i++) {
            uint32_t event = events[i].events;
            int fd = events[i].data.fd;

            // 异常问题转换成读写问题
            if ((event & EPOLLHUP) || (event & EPOLLERR)) {
                event |= (EPOLLIN | EPOLLOUT);
            }

            // 套接字就绪
            connection* conn = connections[fd];
            if (connections.find(fd) != connections.end()) {
                logMessage(NORMAL, "fd = %d is ready, recver or sender", fd);
                if ((event & EPOLLIN) && conn->recver) {
                    conn->recver(conn);
                }
                if ((event & EPOLLOUT) && conn->sender) {
                    conn->sender(conn);
                }
            }
        }
    }

    void addConnection(int sock, uint32_t events, func_t recver, func_t sender, func_t excepter) {
        // 1. 为 listensock 创建 connection
        connection* conn = new connection(this, sock);
        conn->Register(recver, sender, excepter);

        // 2. 将 listensock 注册，此前要设置为非阻塞式
        Util::SetNonBlock(sock);
        epoll.AddEvent(sock, events);

        // 3. KV 模型插入组织 map 中
        connections[sock] = conn;
        logMessage(NORMAL, "%d addConnection ok~", sock);
    }

    /* Accepter 只会通知一次，倒逼程序员要把所有到达的链接全部读取 */
    void Accepter(connection* conn) {
        string clientip;
        uint16_t clientport;
        for (;;) {
            int fd = sock.Accept(&clientip, &clientport);
            if (fd > 0) {
                addConnection(fd, EPOLLIN | EPOLLET, std::bind(&tcpServer::Recver, this, std::placeholders::_1),
                              std::bind(&tcpServer::Sender, this, std::placeholders::_1),
                              std::bind(&tcpServer::Excepter, this, std::placeholders::_1));
                logMessage(NORMAL, "have a new link [%s : %d], sock: %d", clientip.c_str(), clientport, fd);
            } else {
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                    break;
                else if (errno == EINTR)
                    continue;
                else
                    break;
            }
        }
    }

    // 事件派发器
    void Dispatcher() {
        while (true) {
            Loop();
        }
    }

public:
    tcpServer(func_t _calculate, uint16_t _port) : calculate(_calculate), port(_port), events(nullptr) {}

    void init() {
        // 1. 创建 socket
        sock.Socket();
        sock.Bind(port);
        sock.Listen();
        // 2. 创建 epoll
        epoll.Create();
        addConnection(sock.Fd(), EPOLLIN | EPOLLET,
                      std::bind(&tcpServer::Accepter, this, std::placeholders::_1) /* 监听套接字只关心读取 */, nullptr,
                      nullptr);
        events = new struct epoll_event[eventnum];
    }

    void start() { Dispatcher(); }

    void enableWrite(connection* conn, bool readable, bool writeable) {
        uint32_t event = (writeable ? EPOLLOUT : 0) | (readable ? EPOLLIN : 0) | EPOLLET;
        epoll.Control(conn->sock, event, EPOLL_CTL_MOD);
    }

    ~tcpServer() {
        if (events) {
            delete[] events;
        }
    }

private:
    uint16_t port;
    Sock sock;
    epoller epoll;
    struct epoll_event* events;
    func_t calculate;
    unordered_map<int, connection*> connections;
};
}  // namespace Server

// lasttime
// threadpoll