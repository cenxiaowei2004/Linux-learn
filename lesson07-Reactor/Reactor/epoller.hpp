#pragma once

#include <sys/epoll.h>

#include "error.hpp"
#include "log.hpp"

const static int defaultsize = 128;
const static int defaultsock = -1;

class epoller {
public:
    epoller() : epfd(defaultsock) {}

    void Create() {
        epfd = epoll_create(defaultsize);
        if (epfd < 0) {
            logMessage(WARNING, "epoll create fail");
            exit(EPOLL_CREATE_ERR);
        }
    }

    void AddEvent(int fd, int events) {
        struct epoll_event ee;
        ee.data.fd = fd;
        ee.events = events;
        epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ee);
    }

    void ModEvent(int fd, int events) {
        struct epoll_event ee;
        ee.data.fd = fd;
        ee.events = events;
        epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ee);
    }
    void DelEvent(int fd, uint16_t events) { epoll_ctl(epfd, EPOLL_CTL_DEL, fd, nullptr); }

    int Wait(struct epoll_event* events, int eventnum, int timeout) {
        int ret = epoll_wait(epfd, events, eventnum, timeout);
        return ret;
    }

    void Control(int fd, uint32_t event, int action) {
        if (action == EPOLL_CTL_MOD) {
            ModEvent(fd, event);
        } else if (action == EPOLL_CTL_DEL) {
            DelEvent(fd, event);
        }
    }

    ~epoller() {
        if (epfd) {
            close(epfd);
        }
    }

private:
    int epfd;
};