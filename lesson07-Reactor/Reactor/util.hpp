#pragma once

#include <fcntl.h>

class Util {
public:
    static bool SetNonBlock(int fd) {
        int fd1 = fcntl(fd, F_GETFL);
        if (fd1 < 0)
            return false;
        fcntl(fd, F_SETFL, fd1 | O_NONBLOCK);
        return true;
    }
};