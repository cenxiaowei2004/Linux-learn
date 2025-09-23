#pragma once

#include <fcntl.h>
#include <iostream>

void setNonBlock(int fd) {
    int fd1 = fcntl(fd, F_GETFL);
    if (fd1 < 0) {
        return;
    }
    fcntl(fd, F_SETFL, fd1 | O_NONBLOCK);
}